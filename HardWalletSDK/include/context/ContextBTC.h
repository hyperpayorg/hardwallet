#pragma once
#ifndef __ContextBTC__
#define __ContextBTC__

#include "JUB_SDK_BTC.h"

#include <string>
#include <vector>

#include "utility/Singleton.h"
#include "utility/xManager.hpp"

#include "context/Context.h"
#include "utility/util.h"
#include "HDKey/HDKey.hpp"

#include "TrezorCrypto/bip32.h"
#include "TrezorCrypto/curves.h"

#define JUB_CHECK_CONTEXT_BTC(x)                                    \
do {                                                                \
    auto context = jub::ContextManager::GetInstance()->GetOne(x);   \
    JUB_CHECK_NULL(context);                                        \
    const std::type_info& tCtx = typeid(*context);                  \
    const std::type_info& tCtxBTC = typeid(jub::ContextBTC);        \
    const std::type_info& tCtxMultisigBTC = typeid(jub::ContextMultisigBTC);    \
    if (   tCtx.hash_code() !=         tCtxBTC.hash_code()          \
        && tCtx.hash_code() != tCtxMultisigBTC.hash_code()          \
        ) {                                                         \
        return JUBR_ARGUMENTS_BAD;                                  \
    }                                                               \
} while(0)

#define MULTISIG_COSIGNER_MAX_CNT 15

namespace jub {

class ContextBTC : public Context {
public:
    ContextBTC() {};
    ContextBTC(const CONTEXT_CONFIG_BTC& cfg, const JUB_UINT16 deviceID) {
        _mainPath = cfg.mainPath;

        _transType = cfg.transType;
        _deviceID = deviceID;
        _unitType = mBTC;
        _coinType = cfg.coinType;
        _timeout = 120 * 2;
    };
    ~ContextBTC() {};

    virtual JUB_RV GetHDNode(const BIP32_Path& path, std::string& xpub);
    virtual JUB_RV GetMainHDNode(std::string& xpub);

    virtual JUB_RV GetAddress(const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt, const BIP32_Path& path, const JUB_UINT16 tag, std::string& address);
    virtual JUB_RV SetMyAddress(const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt, const BIP32_Path& path, std::string& address);
    virtual JUB_RV SignTX(const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt, const std::vector<INPUT_BTC>& vInputs, const std::vector<OUTPUT_BTC>& vOutputs, const JUB_UINT32 lockTime, std::string& raw);
    virtual JUB_RV SetUnit(const JUB_BTC_UNIT_TYPE& unitType);
    virtual JUB_RV ParseTransaction(const std::string& incRaw,
                                    std::vector<INPUT_BTC>& vInputs,
                                    std::vector<OUTPUT_BTC>& vOutputs,
                                    JUB_UINT32_PTR plockTime);

    virtual JUB_RV BuildUSDTOutputs(IN JUB_CHAR_PTR USDTTo, IN JUB_UINT64 amount, OUT OUTPUT_BTC outputs[2]);
    virtual JUB_RV SetQRC20Token(IN JUB_CHAR_PTR contractAddress, IN JUB_UINT8 decimal, IN JUB_CHAR_PTR symbol);
    virtual JUB_RV BuildQRC20Outputs(IN JUB_UINT64 gasLimit, IN JUB_UINT64 gasPrice, IN JUB_CHAR_PTR contractAddress, IN JUB_CHAR_PTR to, IN JUB_CHAR_PTR value, OUT OUTPUT_BTC outputs[1]);
    virtual JUB_RV ActiveSelf() override;

    //encode & decode
protected:
    virtual JUB_RV SerializeUnsignedTx(const JUB_BTC_TRANS_TYPE type,
                                       const std::vector<INPUT_BTC>& vInputs,
                                       const std::vector<OUTPUT_BTC>& vOutputs,
                                       const JUB_UINT32 lockTime,
                                       uchar_vector& unsignedRaw);

    virtual JUB_RV DeserializeTx(const std::string& hexTx,
                                 std::vector<INPUT_BTC>& vInputs,
                                 std::vector<OUTPUT_BTC>& vOutputs,
                                 JUB_UINT32_PTR plockTime);

protected:
    JUB_ENUM_COINTYPE_BTC _coinType{ COINBTC };
    JUB_BTC_TRANS_TYPE    _transType{ p2pkh };
    JUB_BTC_UNIT_TYPE     _unitType{ mBTC };
    JUB_BYTE _RealAddressFormat(const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt) {
        return ((addrFmt&0x0F) << 4);
    }
    virtual std::string _FullBip32Path(const BIP32_Path& path) override;
}; // class ContextBTC end

//input index, cosigner signature list
typedef std::map<std::string, std::vector<std::string>> MAP_COSIGNER_SIGNATURE;
class ContextMultisigBTC : public ContextBTC {
public:
    ContextMultisigBTC(const CONTEXT_CONFIG_MULTISIG_BTC& cfg, const JUB_UINT16 deviceID, const JUB_UINT32 xpubPrefix=bitcoinXPUB, const JUB_UINT32 xprvPrefix=bitcoinXPRV, const char* curveName=SECP256K1_NAME) {
        _m = cfg.m;
        _n = cfg.n;
        _cosignerIndex = -1;
        _orderedIndex = -1;

        _mainPath = cfg.mainPath;

        _transType = cfg.transType;
        _coinType = cfg.coinType;

        _deviceID = deviceID;
        _unitType = mBTC;
        _timeout = 120 * 2;

        _curveName = curveName;
        _xpubPrefix = xpubPrefix;
        _xprvPrefix = xprvPrefix;

        if (   MULTISIG_COSIGNER_MAX_CNT <= cfg.vCosignerMainXpub.size()
            || MULTISIG_COSIGNER_MAX_CNT <  _m
            || MULTISIG_COSIGNER_MAX_CNT <  _n
            ) {
            _isValid = false;
            return;
        }
        else {
            _isValid = true;
        }

        for (unsigned long i=0; i<cfg.vCosignerMainXpub.size(); ++i) {
            HDNode hdkey;
            uint32_t parentFingerprint;
            if (0 != hdnode_deserialize(cfg.vCosignerMainXpub[i].c_str(),
                                        xpubPrefix, xprvPrefix,
                                        curveName,
                                        &hdkey, &parentFingerprint)) {
                _isValid = false;
                break;
            }

            uchar_vector mainPubKey(hdkey.public_key, sizeof(hdkey.public_key)/sizeof(uint8_t));
            std::string hexMainPubKey = mainPubKey.getHex();
            _vCosignerMainPubKey.push_back(hexMainPubKey);
            _mapCosignerMainKey.insert({hexMainPubKey, cfg.vCosignerMainXpub[i]});
        }
    };
    virtual ~ContextMultisigBTC() {};

    virtual JUB_RV GetAddress(const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt, const BIP32_Path& path, const JUB_UINT16 tag, std::string& address);
    virtual JUB_RV GetP2SHMultisigAddress(const BIP32_Path& path, const JUB_UINT16 tag, std::string& address);
    virtual JUB_RV SignTX(const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt, const std::vector<INPUT_BTC>& vInputs, const std::vector<OUTPUT_BTC>& vOutputs, const JUB_UINT32 lockTime, std::string& raw);

    bool isValid() {
        return _isValid;
    }

    void order(std::string mainXpub) {
        addMyMainXPub(mainXpub);
        sort();
    }

    void setIncTx(std::string hexIncTx) {
        _hexIncTx = hexIncTx;
    }

protected:
    JUB_RV pushAll(std::vector<uchar_vector>& results);
    JUB_RV pushAll(uchar_vector& result);
    JUB_RV pullAll(uchar_vector& result);
    JUB_RV OrderDHPublicKey(const BIP32_Path& path,
                            std::vector<uchar_vector>& vOrderedPublickey);

    JUB_RV OrderSignatures(const std::vector<INPUT_BTC>& vInputs, const std::vector<uchar_vector>& vRedeemScript,
                           const std::vector<OUTPUT_BTC>& vOutputs,
                           const JUB_UINT32 lockTime,
                           const MAP_COSIGNER_SIGNATURE& mapCosignerSignature,
                           const std::vector<uchar_vector>& vSignatureRaw,
                           MAP_COSIGNER_SIGNATURE& mapSignatureOrdered);

private:
    std::string getMainPath(long cosignerIndex, bool isChange) {
        int change = 0;
        if (isChange) {
            change = 1;
        }
        return (_mainPath + "/" + std::to_string(cosignerIndex) + "/" + std::to_string(change));
    }

    long getCosignerIndex() {
        return _cosignerIndex;
    }

    void setOrderedIndex(long orderedIndex) {
        _orderedIndex = orderedIndex;
    }

    long getOrderedIndex() {
        return _orderedIndex;
    }

    std::string getCosignerMainXpub(long cosignerIndex) {
        try {
            if (_vCosignerMainPubKey.size() <= cosignerIndex) {
                return "";
            }
            std::string cosignerMainPubKey = _vCosignerMainPubKey.at(cosignerIndex);
            return getCosignerMainXpub(cosignerMainPubKey);
        }
        catch (...) {
            return "";
        }
    }

    std::string getCosignerMainXpub(std::string mainPubKey) {
        auto search = _mapCosignerMainKey.find(mainPubKey);
        if (search != _mapCosignerMainKey.end()) {
            return search->second;
        }
        else {
            return "";
        }
    }

    std::string getCosignerMainXpub() {
        return getCosignerMainXpub(_mainPubKey);
    }

    std::string getCosignerFullBip32Path(const BIP32_Path& path, const long cosignerIndex);

protected:
    virtual std::string _FullBip32Path(const BIP32_Path& path);
    virtual JUB_RV SerializeTx(const std::vector<INPUT_BTC>& vInputs,
                               const MAP_COSIGNER_SIGNATURE& mapSignature,
                               const std::vector<uchar_vector>& vRedeemScript,
                               const std::vector<OUTPUT_BTC>& vOutputs,
                               const JUB_UINT32 lockTime,
                               uchar_vector& signedIncRaw);

    virtual JUB_RV BuildRedeemScript(const BIP32_Path& path,
                                     uchar_vector& redeemScript);
    virtual JUB_RV BuildRedeemScriptTlv(const BIP32_Path& path,
                                        uchar_vector& redeemScriptTlv);

    virtual JUB_RV DeserializeTx(const std::vector<uchar_vector>& vRedeemScript,
                                 const std::string& hexIncTx,
                                 MAP_COSIGNER_SIGNATURE& mapCosignerSignature,
                                 uchar_vector& unsignedTx);

    void sort() {
        std::sort(_vCosignerMainPubKey.begin(), _vCosignerMainPubKey.end());
        std::vector<std::string>::iterator pos = std::unique(_vCosignerMainPubKey.begin(), _vCosignerMainPubKey.end());
        _vCosignerMainPubKey.erase(pos, _vCosignerMainPubKey.end());

        pos = std::find(_vCosignerMainPubKey.begin(), _vCosignerMainPubKey.end(), _mainPubKey);
        if (-1 == _cosignerIndex) {
            _cosignerIndex = pos - _vCosignerMainPubKey.begin();
        }
    }

    void addMyMainXPub(const std::string mainXpub) {
        HDNode hdkey;
        uint32_t parentFingerprint;
        if (0 != hdnode_deserialize(mainXpub.c_str(),
                                    _xpubPrefix, _xprvPrefix,
                                    _curveName.c_str(),
                                    &hdkey, &parentFingerprint)) {
            return ;
        }

        uchar_vector mainPubKey(hdkey.public_key, sizeof(hdkey.public_key)/sizeof(uint8_t));
        _mainPubKey = mainPubKey.getHex();
        _vCosignerMainPubKey.push_back(_mainPubKey);
        _mapCosignerMainKey.insert({_mainPubKey, mainXpub});
    }

protected:
    unsigned long _m;
    unsigned long _n;
    long _cosignerIndex;
    long _orderedIndex;
    std::string _mainPubKey;
    std::vector<std::string> _vCosignerMainPubKey;
    //hex main public key, xpub
    std::map<std::string, std::string> _mapCosignerMainKey;
    std::string _hexIncTx;
    bool _isValid;

public:
    std::string _curveName={SECP256K1_NAME};
    JUB_UINT32 _xpubPrefix={bitcoinXPUB};
    JUB_UINT32 _xprvPrefix={bitcoinXPRV};
}; // class ContextMultisigBTC end

} // namespace jub end

#endif // #pragma once
