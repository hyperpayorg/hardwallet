#ifndef __DeviceIOLogHelper__
#define __DeviceIOLogHelper__

#include "JUB_SDK.h"

#include "mSIGNA/stdutils/uchar_vector.h"

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define ORANGE  "\033[33m"
#define GRAY    "\033[90m"
#define Red     "\033[31m"
#define Green   "\033[32m"

#define Bold    "\033[1m"
#define Dim     "\033[2m"
#define Underlined  "\033[4m"
#define Blink   "\033[5m"
#define Reverse "\033[7m"
#define Hidden  "\033[8m"

#define BackgroundDarkGray "\033[100m"

class DeviceIOLogHelper {

public:
    DeviceIOLogHelper(JUB_BYTE_CPTR sendData,
                      JUB_ULONG ulSendLen,
                      JUB_BYTE_PTR retData,
                      JUB_ULONG_PTR pulRetDataLen) {

        _sendData = sendData;
        _ulSendLen = ulSendLen;
        _retData = retData;
        _pulRetDataLen = pulRetDataLen;
        uchar_vector logSend(_sendData, _sendData + _ulSendLen);
        auto style = Green ;
        std::cout << GRAY <<"<<" << style << logSend.getHex() << std::endl;
    }
    ~DeviceIOLogHelper() {
        if (nullptr == _pulRetDataLen) {
            return ;
        }

        JUB_UINT16 wRet = _retData[*_pulRetDataLen - 2] * 0x100 + _retData[*_pulRetDataLen - 1];
        auto style = Green ;
        if (0x9000 != wRet) {
            style = Red;
        }

        uchar_vector logRecv(_retData, _retData + *_pulRetDataLen);
        std::cout << GRAY <<">>" << style << logRecv.getHex() << std::endl;

        std::cout << "------------------------------" << std::endl;

        std::cout << RESET;
    }

private:
    JUB_BYTE_CPTR _sendData;
    JUB_ULONG     _ulSendLen;
    JUB_BYTE_PTR  _retData;
    JUB_ULONG_PTR _pulRetDataLen;
}; // class DeviceIOLogHelper end

#endif //DeviceIOLogHelper
