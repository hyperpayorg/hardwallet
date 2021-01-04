
#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <string>

#ifdef ANDROID
#include <android/log.h>
#endif

namespace jub {

#define DEBUG_LEVEL 1

#define JUB_DebugLevel(level, ...)  \
{                                   \
    if (DEBUG_LEVEL >= level) {     \
        JUB_DebugLog(__VA_ARGS__);  \
    }                               \
}

#define JUB_Debug(level, STR)       \
{                                   \
    if (DEBUG_LEVEL >= level) {     \
        JUB_DebugLog(STR);          \
    }                               \
}

#ifdef ANDROID
void JUB_DebugLog(android_LogPriority logPriority, const char *format, ...);
void JUB_DebugLog(android_LogPriority logPriority, const std::string &log);
#else
void JUB_DebugLog(const char *format, ...);
void JUB_DebugLog(const std::string &log);
#endif

#if defined(DEBUG) || defined(_DEBUG)
    #ifdef ANDROID
        #define DEBUG_LOG(...) jub::JUB_DebugLog(ANDROID_LOG_DEBUG, __VA_ARGS__)
        #define DEBUG_ERR(...) jub::JUB_DebugLog(ANDROID_LOG_ERROR, __VA_ARGS__)
    #else
        #define DEBUG_LOG(...) jub::JUB_DebugLog(__VA_ARGS__)
        #define DEBUG_ERR(...) jub::JUB_DebugLog(__VA_ARGS__)
    #endif
#else
	#define DEBUG_LOG(...)
    #define DEBUG_ERR(...)
#endif

enum {
	BA2S_AA_BB,     //01 01 03 04
	BA2S_0XAA_0XBB, //0x01 0x02 0x03 0x04
	BA2S_AABB,      //01020304
}; // enum end

std::string ByteArray2String(const uint8_t* buf, uint32_t len, uint8_t type = BA2S_AABB);
} // namespace jub end

#endif // __DEBUG_H__
