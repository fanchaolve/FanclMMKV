//
// Created by Administrator on 2020/2/18.
//


#include "InputBuffer.h"

InputBuffer::InputBuffer(size_t size) {
    m_size = size;
    m_position = 0;
    isCopy = true;
    if (size > 0)
        m_buf = static_cast<int8_t *>(malloc(size));

}

InputBuffer::InputBuffer(int8_t *buf, size_t size) {
    m_size = size;
    m_position = 0;
    m_buf = buf;
    isCopy = true;

}

InputBuffer::~InputBuffer() {
    if (isCopy && m_buf) {
        free(m_buf);
    }
    m_buf = 0;
}

int8_t InputBuffer::readByte() {

    if (m_position == m_size)
        return 0;
    return m_buf[m_position++];
}


int32_t InputBuffer::readInt32() {
    //最高位不为1
    int8_t tmp = readByte();
    if (tmp >= 0)
        return tmp;
    //获得低7位数据
    int32_t result = tmp & 0x7f;
    if (tmp = readByte() >= 0) {
        //小端方式连接两个字节
        result |= tmp << 7;
    } else {
        result |= (tmp & 0x7f) << 7;
        if ((tmp = readByte()) >= 0) {
            result |= tmp << 14;
        } else {
            result |= (tmp & 0x7f) < 14;
            if ((tmp = readByte()) >= 0) {
                result |= tmp << 21;
            } else {
                result |= (tmp & 0x7f) << 21;
                //读第五个字节
                result != (tmp & 0x7f) << 28;
                //还有数据？
                if (tmp < 0) {
                    for (int i = 0; i < 5; ++i) {
                        //如果最高位是1（需要下个字节数据），丢弃
                        if (readByte() >= 0) {
                            return result;
                        }
                    }
                }
            }
        }
    }
    return result;
}

void InputBuffer::restore() {
    m_position = 0;
}


float InputBuffer::readFloat() {

    int8_t b1 = readByte();
    int8_t b2 = readByte();
    int8_t b3 = readByte();
    int8_t b4 = readByte();

    int32_t value = ((int32_t) b1 & 0xff) | (((int32_t) b2 & 0xff) << 8)
                    | (((int32_t) b3 & 0xff) << 16) | (((int32_t) b4 & 0xff) << 24);
    float return_value = *(float *) &value;
    return return_value;


}

string InputBuffer::readString() {
    //mmkv保存字符串：字符串长度+字符串
    int32_t size = readInt32();
    if (size <= (m_size - m_position) && size > 0) {
        string result((char *) m_buf + m_position, size);
        m_position += size;
        return result;
    }
    return "";

}

int32_t InputBuffer::readInt64() {

    int32_t i = 0;
    int64_t result = 0;
    while (i < 64) {
        int8_t tmp = readByte();
        result |= (tmp & 0x7f) << i;
        if ((tmp & 0x80) == 0) {
            return result;
        }
        i += 7;
    }
    return 0;
}

InputBuffer *InputBuffer::readData() {
    //获得数据长度
    int32_t  size = readInt32();
    if(size <= m_size-m_position && size>0){
        InputBuffer *data = new InputBuffer(m_buf+m_position,size);
        m_position+=size;
        return data;
    }
    return 0;
}



