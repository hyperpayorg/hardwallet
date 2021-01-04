//
//  XRPTokenInterface.hpp
//  JubSDK
//
//  Created by Pan Min on 2019/11/25.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#ifndef __XRPTokenInterface__
#define __XRPTokenInterface__

#include "JUB_SDK_XRP.h"

#include <vector>

namespace jub {

class XRPTokenInterface {

public:
    virtual JUB_RV SelectAppletXRP() = 0;
    virtual JUB_RV SetCoinTypeXRP() = 0;
    virtual JUB_RV GetAddressXRP(const std::string& path, const JUB_UINT16 tag, std::string& address) = 0;
    virtual JUB_RV GetHDNodeXRP(const JUB_BYTE format, const std::string& path, std::string& pubkey) = 0;
    virtual JUB_RV SignTXXRP(const std::vector<JUB_BYTE>& vPath,
                             std::vector<JUB_BYTE>& vUnsignedRaw,
                             std::vector<uchar_vector>& vSignatureRaw)  = 0;
}; // class EOSTokenInterface end

} // namespace jub end

#endif
