//
//  EOSTokenInterface.hpp
//  JubSDK
//
//  Created by Pan Min on 2019/10/09.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#ifndef __EOSTokenInterface__
#define __EOSTokenInterface__

#include "JUB_SDK_EOS.h"
#include "EOS/Prefixes.h"

#include <vector>

namespace jub {

class EOSTokenInterface {

public:
    virtual JUB_RV SelectAppletEOS() = 0;
    virtual JUB_RV SetCoinTypeEOS() = 0;
    virtual JUB_RV GetAddressEOS(const TW::EOS::Type& type, const std::string& path, const JUB_UINT16 tag, std::string& address) = 0;
    virtual JUB_RV GetHDNodeEOS(const JUB_BYTE format, const std::string& path, std::string& pubkey) = 0;
    virtual JUB_RV SignTXEOS(const TW::EOS::Type& type,
                             const std::vector<JUB_BYTE>& vPath,
                             const std::vector<JUB_BYTE>& vChainId,
                             const std::vector<JUB_BYTE>& vRaw,
                             std::vector<uchar_vector>& vSignatureRaw,
                             const bool bWithType=false) = 0;
}; // class EOSTokenInterface end

} // namespace jub end

#endif
