#pragma once
#ifndef __ContextETH__
#define __ContextETH__

#include "JUB_SDK_ETH.h"

#include <string>
#include <vector>

#include "utility/Singleton.h"
#include "utility/xManager.hpp"

#include "context/Context.h"
#include "libETH/ERC20Abi.h"

#define JUB_CHECK_CONTEXT_ETH(x)                                    \
do {                                                                \
    auto context = jub::ContextManager::GetInstance()->GetOne(x);   \
    JUB_CHECK_NULL(context);                                        \
    const std::type_info& tCtx = typeid(*context);                  \
    const std::type_info& tCtxETH = typeid(jub::ContextETH);        \
    if (tCtx.hash_code() != tCtxETH.hash_code()) {                  \
        return JUBR_ARGUMENTS_BAD;                                  \
    }                                                               \
} while(0)

namespace jub {

class ContextETH : public Context {
public:
    ContextETH(const CONTEXT_CONFIG_ETH& cfg, const JUB_UINT16 deviceID) {
        _mainPath = cfg.mainPath;
        _chainID = cfg.chainID;
        _deviceID = deviceID;
        _timeout = 120 * 2;
        _appletVersion = "";

        _contrFuncList.clear();
    };
    ~ContextETH() {};

    virtual JUB_RV GetAddress(const BIP32_Path& path, const JUB_UINT16 tag, std::string& address);
    virtual JUB_RV SetMyAddress(const BIP32_Path& path, std::string& address);
    virtual JUB_RV GetHDNode(const JUB_BYTE& format, const BIP32_Path& path, std::string& pubkey);
    virtual JUB_RV GetMainHDNode(const JUB_BYTE format, std::string& xpub);

    virtual JUB_RV SignTransaction(const BIP32_Path& path,
                                   const JUB_UINT32 nonce,
                                   const JUB_UINT32 gasLimit,
                                   const std::string& gasPriceInWei,
                                   const std::string& to,
                                   IN JUB_CHAR_CPTR valueInWei,
                                   IN JUB_CHAR_CPTR input,
                                   OUT std::string& raw);

    virtual JUB_RV BuildERC20Abi(JUB_CHAR_CPTR to, JUB_CHAR_CPTR value, std::string& abi);
    virtual JUB_RV SetERC20ETHToken(JUB_CHAR_CPTR pTokenName,
                                    JUB_UINT16 unitDP,
                                    JUB_CHAR_CPTR pContractAddress);
    virtual JUB_RV ActiveSelf() override;

    virtual JUB_RV SignContract(const BIP32_Path& path,
                                const JUB_UINT32 nonce,
                                const JUB_UINT32 gasLimit,
                                const std::string& gasPriceInWei,
                                const std::string& to,
                                IN JUB_CHAR_CPTR valueInWei,
                                IN JUB_CHAR_CPTR input,
                                OUT std::string& raw);

//    virtual JUB_RV BuildContractWithAddrAbi(JUB_CHAR_CPTR methodID, JUB_CHAR_CPTR address, std::string& abi);
//    virtual JUB_RV BuildContractWithAddrAmtAbi(JUB_CHAR_CPTR methodID, JUB_CHAR_CPTR address, JUB_CHAR_CPTR amount, std::string& abi);
    virtual JUB_RV BuildContractWithTxIDAbi(JUB_CHAR_CPTR methodID, JUB_CHAR_CPTR transactionID, std::string& abi);
//    virtual JUB_RV BuildContractWithAmtAbi(JUB_CHAR_CPTR methodID, JUB_CHAR_CPTR amount, std::string& abi);
    virtual JUB_RV BuildContractWithAddrAmtDataAbi(JUB_CHAR_CPTR methodID, JUB_CHAR_CPTR address, JUB_CHAR_CPTR amount, JUB_CHAR_CPTR data, std::string& abi);

private:
    void _addContrFunc(const std::string& methodID, const jub::eth::ENUM_CONTRACT_ABI& type) {
        if (_contrFuncList.end() == _contrFuncList.find(methodID)) {
            _contrFuncList.insert({methodID, (int)type});
        }
    }
    jub::eth::ENUM_CONTRACT_ABI _getInputType(const std::string& methodID) {
        std::map<std::string, int>::iterator it = _contrFuncList.find(methodID);
        if (_contrFuncList.end() != it) {
            return (jub::eth::ENUM_CONTRACT_ABI)(it->second);
        }
        return jub::eth::ENUM_CONTRACT_ABI::NS_ITEM;
    }

    int _chainID;
    // "methodID" - input type
    std::map<std::string, int> _contrFuncList;
}; // class ContextETH end

} // namespace jub end

#endif // #pragma once
