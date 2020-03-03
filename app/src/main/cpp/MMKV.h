//
// Created by Administrator on 2020/2/18.
//

#ifndef FANCLMMKV_MMKV_H
#define FANCLMMKV_MMKV_H

#include <string>
#include <unordered_map>
#include "OutputBuffer.h"

using namespace std;

class  MMKV {
public:
    MMKV(const string &mmapID);

    ~MMKV();

    void close();

public:
    static void initializeMMKV(const char *path);

    static MMKV *defaultMMKV();

    static MMKV *mmkvWithID(const string &mmapID);

    int32_t getInt(const char *key, int defaultValue);

    void putInt(const char *key, int value);

    void appendDataWithKey(string key, InputBuffer *value);

private:
    void loadFromFile();

private:
    string m_mapID;
    string m_path;//mmkv持久化文件地址
    int m_fd;//打开的文件句柄
    size_t m_size;//文件大小
    int8_t *m_ptr;//映射文件的数据区
    size_t m_actualSize;//有效数据长度

    //记录的可以-value
    unordered_map<string, InputBuffer *> m_dic;

    OutputBuffer *m_output;

    void writeAcutalSize(size_t size);


};


#endif //FANCLMMKV_MMKV_H
