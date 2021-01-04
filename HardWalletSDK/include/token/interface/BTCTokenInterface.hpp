#ifndef __BTCTokenInterface__
#define __BTCTokenInterface__

#include "JUB_SDK_BTC.h"

#include <vector>
#include "utility/util.h"

namespace jub {

class BTCTokenInterface {

public:
    virtual JUB_RV SelectAppletBTC() = 0;
    virtual JUB_RV SetUnitBTC(const JUB_BTC_UNIT_TYPE& unit) = 0;
    virtual JUB_RV SetCoinTypeBTC(const JUB_ENUM_COINTYPE_BTC& type) = 0;
    virtual JUB_RV SignTXBTC(const JUB_BYTE addrFmt,
                             const JUB_BTC_TRANS_TYPE& type,
                             const JUB_UINT16 inputCount,
                             const std::vector<JUB_UINT64>& vInputAmount,
                             const std::vector<std::string>& vInputPath,
                             const std::vector<JUB_UINT16>& vChangeIndex,
                             const std::vector<std::string>& vChangePath,
                             const std::vector<JUB_BYTE>& vUnsigedTrans,
                             std::vector<JUB_BYTE>& vRaw) = 0;
    virtual JUB_RV SignTXMultisigBTC(const JUB_BYTE addrFmt,
                                     const JUB_BTC_TRANS_TYPE& type,
                                     const JUB_UINT16 inputCount,
                                     const std::vector<JUB_UINT64>& vInputAmount,
                                     const std::vector<std::string>& vInputPath,
                                     const std::vector<uchar_vector>& vRedeemScriptTlv,
                                     const std::vector<JUB_UINT16>& vChangeIndex,
                                     const std::vector<std::string>& vChangePath,
                                     const std::vector<uchar_vector>& vChangeRedeemScriptTlv,
                                     const std::vector<JUB_BYTE>& vUnsigedTrans,
                                     std::vector<uchar_vector>& vSignatureRaw) = 0;

    virtual JUB_RV GetHDNodeBTC(const JUB_BTC_TRANS_TYPE& type, const std::string& path, std::string& xpub) = 0;
    virtual JUB_RV GetAddressBTC(const JUB_BYTE addrFmt,
                                 const JUB_BTC_TRANS_TYPE& type,
                                 const std::string& path,
                                 const JUB_UINT16 tag,
                                 std::string& address) = 0;
    virtual JUB_RV GetAddressMultisigBTC(const JUB_BYTE addrFmt,
                                         const JUB_BTC_TRANS_TYPE& type,
                                         const std::string& path,
                                         const JUB_UINT16 tag,
                                         const uchar_vector& vRedeemScriptTlv,
                                         std::string& address) = 0;
}; // class BTCTokenInterface end

} // namespace jub end

#endif
