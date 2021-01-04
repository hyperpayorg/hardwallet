#ifndef __TokenInterface__
#define __TokenInterface__
#include "JUB_SDK.h"

#include <string>
#include <vector>

#include "utility/Singleton.h"
#include "utility/xManager.hpp"

#include "BTCTokenInterface.hpp"
#include "ETHTokenInterface.hpp"
#include "EOSTokenInterface.hpp"
#include "XRPTokenInterface.hpp"

namespace jub {

class CommonTokenInterface {
public:
    virtual ~CommonTokenInterface() = default;

    /* functions */
    virtual JUB_RV ConnectToken() = 0;
    virtual JUB_RV DisconnectToken() = 0;
    virtual JUB_RV ShowVirtualPwd() = 0;
    virtual JUB_RV CancelVirtualPwd() = 0;
    virtual bool   IsInitialize() = 0;
    virtual bool   IsBootLoader() = 0;
    virtual JUB_RV GetSN(JUB_BYTE sn[24]) = 0;
    virtual JUB_RV GetLabel(JUB_BYTE label[32]) = 0;
    virtual JUB_RV GetPinRetry(JUB_BYTE& retry) = 0;
    virtual JUB_RV GetPinMaxRetry(JUB_BYTE& maxRetry) = 0;
    virtual JUB_RV GetBleVersion(JUB_BYTE bleVersion[4]) = 0;
    virtual JUB_RV GetFwVersion(JUB_BYTE fwVersion[4]) = 0;
    virtual JUB_RV EnumApplet(std::string& appletList) = 0;
    virtual JUB_RV GetAppletVersion(const std::string& appID, std::string& version) = 0;
    virtual JUB_RV EnumSupportCoins(std::string& coinList) = 0;
    virtual JUB_RV GetDeviceCert(std::string& cert) = 0;
    virtual JUB_RV SendOneApdu(const std::string& apdu, std::string& response) = 0;

    virtual JUB_RV QueryBattery(JUB_BYTE &percent) = 0;

    virtual JUB_RV VerifyPIN(const std::string &pinMix, OUT JUB_ULONG &retry) = 0;
    virtual JUB_RV SetTimeout(const JUB_UINT16 timeout) = 0;
}; // class TokenInterface end

using TokenManager = Singleton<xManager<jub::CommonTokenInterface>>;

}  // namespace jub end
#endif  // __TokenInterface__
