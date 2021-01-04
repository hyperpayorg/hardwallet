//
// Created by GOD on 2017/4/20.
//

#ifndef _LOGUTILS_H
#define _LOGUTILS_H

#include <time.h>
#include <stdio.h>
#include <cstring>
#include <ctype.h>

#ifdef ANDROID_API_LEVEL_H

#include <jni.h>
#include <android/log.h>

#endif

//**************************************************************************************************

/**
 * 字符串输出
 * @param pro
 * @param szLog
 */
void LogString(int pro, const char *szLog, ...);

/**
 * 16 进制输出
 * @param msg
 * @param pBuf
 * @param ulSize
 */
void LogBinary(const char* msg, const unsigned char *pBuf, unsigned long ulSize);

//**************************************************************************************************


#if defined(DEBUG)

#ifdef ANDROID_API_LEVEL_H

#define TAG "JuBiter"
#define PRINTV(...) { __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__); }
#define PRINTD(...) { __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__); }
#define PRINTI(...) { __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__); }
#define PRINTW(...) { __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__); }
#define PRINTE(...) { __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__); }

#else

#define PRINTE(...) printf(__VA_ARGS__);
#define PRINTW(...) printf(__VA_ARGS__);
#define PRINTI(...) printf(__VA_ARGS__);

#endif  /* ANDROID_API_LEVEL_H */


#define LOG_ERR(...) {                                                                    \
    PRINTE(__VA_ARGS__);                                                                  \
}

#define LOG_WRN(...) {                                                                    \
    PRINTW(__VA_ARGS__)                                                                   \
}

#define LOG_INF(...) {                                                                    \
    PRINTI(__VA_ARGS__);                                                                  \
}

#define LOG_BIN(msg, src, len) {                                                         \
    LogBinary(msg, src, len);                                                             \
}

#else

#define LOG_ERR(...)
#define LOG_WRN(...)
#define LOG_INF(...)
#define LOG_BIN(...)

#endif /* DEBUG */

#endif //_LOGUTILS_H
