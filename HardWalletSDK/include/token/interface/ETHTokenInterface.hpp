#ifndef __ETHTokenInterface__
#define __ETHTokenInterface__

#include "JUB_SDK_ETH.h"

#include <vector>

// ETH token extension apdu
#define APPLET_VERSION_SUPPORT_EXT_TOKEN "01040109"

namespace jub {

class ETHTokenInterface {

public:
    virtual JUB_RV SelectAppletETH() = 0;
    virtual JUB_RV GetAppletVersionETH(std::string& version) = 0;
    virtual JUB_RV GetAddressETH(const std::string& path, const JUB_UINT16 tag, std::string& address) = 0;
    virtual JUB_RV GetHDNodeETH(const JUB_BYTE format, const std::string& path, std::string& pubkey) = 0;
    virtual JUB_RV SignTXETH(const bool bERC20,
                             const std::vector<JUB_BYTE>& vNonce,
                             const std::vector<JUB_BYTE>& vGasPrice,
                             const std::vector<JUB_BYTE>& vGasLimit,
                             const std::vector<JUB_BYTE>& vTo,
                             const std::vector<JUB_BYTE>& vValue,
                             const std::vector<JUB_BYTE>& vData,
                             const std::vector<JUB_BYTE>& vPath,
                             const std::vector<JUB_BYTE>& vChainID,
                             std::vector<JUB_BYTE>& vRaw) = 0;
    virtual JUB_RV SetERC20ETHToken(const std::string& tokenName,
                                    const JUB_UINT16 unitDP,
                                    const std::string& contractAddress) = 0;
    virtual JUB_RV SignContractETH(const JUB_BYTE inputType,
                                   const std::vector<JUB_BYTE>& vNonce,
                                   const std::vector<JUB_BYTE>& vGasPrice,
                                   const std::vector<JUB_BYTE>& vGasLimit,
                                   const std::vector<JUB_BYTE>& vTo,
                                   const std::vector<JUB_BYTE>& vValue,
                                   const std::vector<JUB_BYTE>& vInput,
                                   const std::vector<JUB_BYTE>& vPath,
                                   const std::vector<JUB_BYTE>& vChainID,
                                   std::vector<JUB_BYTE>& signatureRaw) = 0;
    virtual JUB_RV SignContractHashETH(const JUB_BYTE inputType,
                                       const std::vector<JUB_BYTE>& vGasLimit,
                                       const std::vector<JUB_BYTE>& vInputHash,
                                       const std::vector<JUB_BYTE>& vUnsignedTxHash,
                                       const std::vector<JUB_BYTE>& vPath,
                                       const std::vector<JUB_BYTE>& vChainID,
                                       std::vector<JUB_BYTE>& signatureRaw) = 0;
}; // class ETHTokenInterface end

} // namespace jub end

#endif
