#pragma once
#ifndef __Context__
#define __Context__

#include "JUB_SDK.h"

#include <string>
#include <vector>
#include <iostream>

#include "utility/Singleton.h"
#include "utility/xManager.hpp"

namespace jub {

class Context {
public:
    Context(JUB_UINT16 deviceID) {
        _deviceID = deviceID;
    };
    Context() {};
    virtual ~Context() {};

    virtual JUB_RV ShowVirtualPwd();
    virtual JUB_RV CancelVirtualPwd();
    virtual JUB_RV VerifyPIN(const JUB_CHAR_PTR pinMix, OUT JUB_ULONG &retry);
    virtual JUB_RV ActiveSelf() = 0;
    virtual JUB_RV SetTimeout(const JUB_UINT16 timeout);

protected:
    JUB_UINT16  _deviceID;
    std::string _mainPath;
    JUB_UINT16  _timeout;

    virtual std::string _FullBip32Path(const BIP32_Path& path);
    std::string _appletVersion;
}; // class Context end

class AutoContextManager : public xManager<jub::Context> {
public:
    jub::Context* GetOne(JUB_UINT16 ID) {

        auto it = _mapList.find(ID);
        if (it != _mapList.end()) {
            if (_last != it->second) {
                it->second->ActiveSelf();
            }
            _last = it->second;

            return it->second;
        }

        return nullptr;
    }
}; // class AutoContextManager end

using ContextManager = Singleton<AutoContextManager>;

} // namespace jub end

#endif // #pragma once
