//
// Created by Administrator on 2020/2/19.
//

#ifndef FANCLMMKV_MMKVLOG_H
#define FANCLMMKV_MMKVLOG_H


#include <android/log.h>

#define  MMKV_LOG_TAG

#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, "MMKV", __VA_ARGS__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, "MMKV", __VA_ARGS__)


#endif //FANCLMMKV_MMKVLOG_H
