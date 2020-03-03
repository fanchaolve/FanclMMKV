//
// Created by Administrator on 2020/2/18.
//

#ifndef FANCLMMKV_INPUTBUFFER_H
#define FANCLMMKV_INPUTBUFFER_H

#include <string>


using namespace std;


class InputBuffer {

    int8_t *m_buf;
    size_t m_size;
    size_t m_position;//当前读取位置
    bool isCopy;
public:
    InputBuffer(size_t size);

    InputBuffer(int8_t *buf, size_t size);

    ~InputBuffer();

    bool isAtEnd() {
        return m_position == m_size;
    }

    int32_t length() {
        return m_size;
    }

    int8_t *data() {
        return m_buf;
    }

    int32_t readInt32();

    int32_t readInt64();

    float readFloat();

    string readString();

    InputBuffer *readData();

    void restore();

private:
    int8_t readByte();


};

#endif //FANCLMMKV_INPUTBUFFER_H
