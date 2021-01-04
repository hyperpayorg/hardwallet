#ifndef __DeviceTypeBase__
#define __DeviceTypeBase__

#include "JUB_SDK.h"

#include <string>

namespace jub {

class DeviceTypeBase {

public:
    /* functions */
    virtual JUB_RV Connect(const std::string path ) = 0;
    virtual JUB_RV Disconnect() = 0;

    virtual JUB_RV SendData(IN JUB_BYTE_CPTR sendData, IN JUB_ULONG ulSendLen,
                            OUT JUB_BYTE_PTR retData, INOUT JUB_ULONG_PTR pulRetDataLen,
                            IN JUB_ULONG ulMiliSecondTimeout = 1200000) = 0;
}; // class DeviceTypeBase end

#define FT3KHN_READWRITE_SIZE_ONCE_NEW 4096
} // namespace jub end

#endif // __DeviceTypeBase__
