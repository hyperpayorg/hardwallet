//#include <string>
#include <sstream>
#include <iomanip>
#include <stdarg.h>
#include <stdio.h>
//#include <time.h>
#include <vector>
#include <mutex>

#include "utility/Debug.hpp"

namespace jub {

//#define MAX_LOG_SIZE (1 << 19) // Max size 512 KiB

void trim(std::string& str, char drop = ' ') {
	// trim right
	str.erase(str.find_last_not_of(drop) + 1);
	// trim left	
	str.erase(0, str.find_first_not_of(drop));
}

static std::mutex gDebugMutex;
//static FILE *gLogFile = nullptr;

std::string ByteArray2String(const uint8_t* buf, uint32_t len, uint8_t type/* = BA2S_AABB*/) {
	std::string head, back;
	//  string temp;
	std::ostringstream temp;
	temp.setf(std::ios::hex, std::ios::basefield);
	temp.setf(std::ios::uppercase);
	temp.fill('0');
//    uint8_t add = 0;
	switch (type) {
    case BA2S_0XAA_0XBB:
    {
        head = "0x";
        back = " ,";
        break;
    } // case BA2S_0XAA_0XBB end
    case BA2S_AABB:
    {
        head = "";
        back = "";
        break;
    } // case BA2S_AABB end
    case BA2S_AA_BB:
    default:
    {
        head = "";
        back = " ";
        break;
    } // case BA2S_AA_BB end
	} // switch (type) end

	for (uint32_t i = 0; i < len; i++) {
		temp << head << std::setw(2) << (uint32_t)buf[i] << back;
	}
	std::string str = temp.str();
	trim(str);

	return str;
}

#ifdef ANDROID
void JUB_DebugLog(android_LogPriority logPriority, const char *format, ...)
#else
void JUB_DebugLog(const char *format, ...)
#endif
{
#if (defined(_DEBUG) || defined (DEBUG))
	time_t t = time(nullptr);
	struct tm *utc = gmtime(&t);

	std::stringstream date;
	date << std::setfill('0');
	date << std::setw(4) << utc->tm_year + 1900 << '-';
	date << std::setw(2) << utc->tm_mon + 1 << '-';
	date << std::setw(2) << utc->tm_mday << ' ';
	date << std::setw(2) << utc->tm_hour << ':';
	date << std::setw(2) << utc->tm_min << ':';
	date << std::setw(2) << utc->tm_sec << " JUB_Log: ";

	// Get the message length:
	va_list args;
	va_start(args, format);
	char temp[1];
	int size = vsnprintf(temp, sizeof(temp)/sizeof(char), format, args);
	va_end(args);
    if (0 > size) {
		return;
    }

	// Format the message:
	va_start(args, format);
	std::vector<char> message(size + 1);
	vsnprintf(message.data(), message.size(), format, args);
	va_end(args);

	// Put the pieces together:
	std::string out = date.str();
	out.append(message.begin(), message.end() - 1);
	if ('\n' != out.back())
		out.append(1, '\n');

#ifdef ANDROID
	__android_log_print(ANDROID_LOG_DEBUG, "JUB", "%s", out.c_str());
#else
	fprintf(stderr, "%s", out.c_str());
#endif

//    if (   gLogFile
//        && MAX_LOG_SIZE < ftell(gLogFile)
//        ) {
//        debugLogRotate().log();
//    }

//    if (gLogFile) {
//        std::lock_guard<std::mutex> lock(gDebugMutex);
//        fwrite(out.c_str(), 1, out.size(), gLogFile);
//        fflush(gLogFile);
//    }

#endif
}

#ifdef ANDROID
void JUB_DebugLog(android_LogPriority logPriority, const std::string &log) {
	JUB_DebugLog(logPriority, "%s", log.c_str());
}
#else
void JUB_DebugLog(const std::string &log) {
	JUB_DebugLog("%s", log.c_str());
}
#endif

} // namespace jub end
