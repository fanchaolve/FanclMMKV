//
// Created by Administrator on 2020/2/18.
//


#include "MMKV.h"

#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "MMKVLog.h"

#include "PBUtility.h"

static unordered_map<string, MMKV *> *g_instanceDic = 0;

static string g_rootDir;


//默认的mmkv文件
#define  DEFAULT_MMAP_ID "mmkv.default"

//系统给我们提供真正的内存时，用页为单位提供
//内存分页大小 一分页的大小
const int DEFAULT_MMAP_SIZE = getpagesize();

const int32_t Fixed32Size = 4;

void MMKV::initializeMMKV(const char *path) {
    g_instanceDic = new unordered_map<string, MMKV *>;
    g_rootDir = path;
    //创建目录
    mkdir(g_rootDir.c_str(), 0777);

}

MMKV *MMKV::defaultMMKV() {
    return mmkvWithID(DEFAULT_MMAP_ID);
}

MMKV *MMKV::mmkvWithID(const string &mmapID) {
    auto itr = g_instanceDic->find(mmapID);
    if (itr != g_instanceDic->end()) {
        MMKV *kv = itr->second;
        return kv;
    }
    // 创建并放入集合
    auto kv = new MMKV(mmapID);
    (*g_instanceDic)[mmapID] = kv;
    return kv;

}

MMKV::MMKV(const string &mmapID) : m_mapID(mmapID), m_path(g_rootDir + "/" + mmapID) {
    loadFromFile();
}

MMKV::~MMKV() {
    delete m_output;
    m_output = 0;
    munmap(m_ptr, m_size);
    m_ptr = 0;
    ::close(m_fd);
    auto iter = m_dic.begin();
    while (iter != m_dic.end()) {
        delete iter->second;
        iter = m_dic.erase(iter);
    }


}

void MMKV::loadFromFile() {
    m_fd = open(m_path.c_str(), O_RDWR | O_CREAT, S_IRWXU);

    if (m_fd < 0) {
        //打开失败
        LOGI("打开文件:%s 失败", m_path.c_str());

    }
    // 读取文件的大小
    struct stat st = {0};
    if (fstat(m_fd, &st) != -1) {
        m_size = st.st_size;

    }

    LOGI("打开文件:%s[%d]", m_path.c_str(), m_size);

    m_ptr = static_cast<int8_t *>(mmap(nullptr, m_size, PROT_READ | PROT_WRITE,
                                       MAP_SHARED, m_fd, 0));

//    char* ptr = (char*) mmap(nullptr, m_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);

    if (m_ptr == MAP_FAILED) {

        LOGI("mmap里面");
    }
    LOGI("memcpy之前");
    //文件头4个字节写了数据有效区长度
    memcpy(&m_actualSize, m_ptr, Fixed32Size);
    bool loadFromFile = false;
    LOGI("memcpy之后");
    if (m_actualSize > 0) {
        if (m_actualSize + Fixed32Size <= m_size) {
            loadFromFile = true;
        }
        //其他情况,MMKV是交给用户选择
        //
    }
    /**
     *  解析MMKV文件中的数据 保存到map集合中
     */
    if (loadFromFile) {
        InputBuffer intputBuffer(m_ptr + Fixed32Size, m_actualSize);
        while (!intputBuffer.isAtEnd()) {
            string key = intputBuffer.readString();
            if (key.length() > 0) {
                //读取value（包含value长度+value数据）
                InputBuffer *value = intputBuffer.readData();
                //unordered_map<string,InputBuffer*>::iterator iter;
                auto iter = m_dic.find(key);
                // 集合中找到了老数据
                if (iter != m_dic.end()) {
                    //清理老数据
                    delete iter->second;
                    m_dic.erase(key);

                }
                //本次数据有效，加入集合
                if (value && value->length() > 0) {
                    m_dic.emplace(key, value);
                }
            }
        }
        m_output = new OutputBuffer(m_ptr + Fixed32Size + m_actualSize,
                                    m_size - Fixed32Size - m_actualSize);

    } else {
        //todo 文件有问题，忽略文件已存在的数据
    }

}

int32_t MMKV::getInt(const char *key, int defaultValue) {
    auto itr = m_dic.find(key);
    //找到了
    if (itr != m_dic.end()) {
        //获取value->解码器
        InputBuffer *buf = itr->second;
        int32_t returnValue = buf->readInt32();
        buf->restore();
        return returnValue;
    }
    return defaultValue;

}

void MMKV::putInt(const char *key, int value) {
    size_t size = computeInt32Size(value);
    //编码 value
    auto *buf = new InputBuffer(size);
    //编码
    OutputBuffer buffer(buf->data(), buf->length());
    buffer.writeInt32(value);
    //记录到内存
    auto itr = m_dic.find(key);
    if (itr != m_dic.end()) {
        delete itr->second;
    }
    m_dic[key] = buf;

    //同步到映射区(文件)
    appendDataWithKey(key, buf);

}

void MMKV::appendDataWithKey(string key, InputBuffer *value) {
    //计算保存这个key-value需要多少字节
    size_t item_size = computeItemSize(key, value);
    if (item_size > m_output->spaceLeft()) {
        // 计算去重key后的数据 所需的存储空间
        size_t needSize = computeMapSize(m_dic);
        needSize += Fixed32Size;
        //小于文件大小
        if (needSize >= m_size) {
            int32_t oldSize = m_size;
            do {
                //扩充一倍  为什么？？？ mmap规则限制：整数倍
                m_size *= 2;
            } while (needSize >= m_size);
            //重新设定文件大小
            ftruncate(m_fd, m_size);
            //解除映射
            munmap(m_ptr, oldSize);

            m_ptr = static_cast<int8_t *>(mmap(m_ptr, m_size, PROT_READ | PROT_WRITE, MAP_SHARED,
                                               m_fd, 0));

        }

        //全量更新
        writeAcutalSize(needSize - Fixed32Size);
        delete m_output;
        //创建输出
        m_output = new OutputBuffer(m_ptr + Fixed32Size, m_size
                                                         - Fixed32Size);
        //把map写入文件
        auto iter = m_dic.begin();
        for (; iter != m_dic.end(); iter++) {
            auto k = iter->first;
            auto v = iter->second;
            m_output->writeString(k);
            m_output->writeData(v);
        }

    } else {
        //增量更新
        writeAcutalSize(m_actualSize + item_size);
        //写入key
        m_output->writeString(key);
        //写入value
        m_output->writeData(value);

    }
}


void MMKV::writeAcutalSize(size_t size) {
    memcpy(m_ptr, &size, Fixed32Size);
    m_actualSize = size;
}