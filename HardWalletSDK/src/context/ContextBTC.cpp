#include "utility/util.h"
#include "bitcoin/hash.h"

#include "context/ContextBTC.h"
#include "token/interface/TokenInterface.hpp"
#include "libBTC/libBTC.hpp"
#include "TrezorCrypto/secp256k1.h"
#include "libETH/ERC20Abi.h"
#include "airbitz-core/abcd/util/Data.hpp"
#include <TrezorCrypto/base58.h>

namespace jub {
std::string ContextBTC::_FullBip32Path(const BIP32_Path& path) {
    if (JUB_ENUM_COINTYPE_BTC::COINQTUM == _coinType) {
        return _mainPath;
    }

    return _mainPath
            + "/" + std::to_string(path.change)
            + "/" + std::to_string(path.addressIndex);
}

JUB_RV ContextBTC::GetHDNode(const BIP32_Path& path, std::string& xpub) {

    auto token = dynamic_cast<BTCTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string strPath = _FullBip32Path(path);
    JUB_VERIFY_RV(token->GetHDNodeBTC(_transType, strPath, xpub));

    return JUBR_OK;
}

JUB_RV ContextBTC::GetMainHDNode(std::string& xpub) {

    auto token = dynamic_cast<BTCTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->GetHDNodeBTC(_transType, _mainPath, xpub));

    return JUBR_OK;
}

JUB_RV ContextBTC::GetAddress(const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt, const BIP32_Path& path, const JUB_UINT16 tag, std::string& address) {

    auto token = dynamic_cast<BTCTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string strPath = _FullBip32Path(path);
    JUB_VERIFY_RV(token->GetAddressBTC(addrFmt, _transType, strPath, tag, address));

    return JUBR_OK;
}

JUB_RV ContextBTC::SetMyAddress(const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt, const BIP32_Path& path, std::string& address) {

    auto token = dynamic_cast<BTCTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string strPath = _FullBip32Path(path);
    JUB_VERIFY_RV(token->GetAddressBTC(addrFmt, _transType, strPath, 0x02, address));

    return JUBR_OK;
}

JUB_RV ContextBTC::SetUnit(const JUB_BTC_UNIT_TYPE& unitType) {

    _unitType = unitType;

    auto token = dynamic_cast<BTCTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->SetUnitBTC(_unitType));

    return JUBR_OK;
}

JUB_RV ContextBTC::ActiveSelf() {

    auto token  = dynamic_cast<BTCTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    auto ctoken = dynamic_cast<CommonTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);
    JUB_CHECK_NULL(ctoken);
    JUB_VERIFY_RV(token->SelectAppletBTC());
    JUB_VERIFY_RV(ctoken->SetTimeout(_timeout));
    JUB_VERIFY_RV(token->SetUnitBTC(_unitType));
    JUB_VERIFY_RV(token->SetCoinTypeBTC(_coinType));

    return JUBR_OK;
}

JUB_RV ContextBTC::BuildUSDTOutputs(IN JUB_CHAR_PTR USDTTo, IN JUB_UINT64 amount, OUT OUTPUT_BTC outputs[2]) {

    //build return0 output
    outputs[0].type = SCRIPT_BTC_TYPE::RETURN0;
    outputs[0].return0.amount = 0;
    outputs[0].return0.dataLen = 20;
    uchar_vector usdtData("6f6d6e69000000000000001f");
    uint8_t amountBE[8] = { 0x00, };
    WriteBE64(amountBE, amount);
    usdtData.insert(usdtData.end(), amountBE, amountBE + 8);
    memcpy(outputs[0].return0.data, &usdtData[0], 20);

    //build dust output
    outputs[1].type = SCRIPT_BTC_TYPE::P2PKH;
    outputs[1].stdOutput.address = USDTTo;
    outputs[1].stdOutput.amount = 546;
    outputs[1].stdOutput.changeAddress = BOOL_FALSE;

    return JUBR_OK;
}

JUB_RV ContextBTC::SetQRC20Token(IN JUB_CHAR_PTR contractAddress, IN JUB_UINT8 decimal, IN JUB_CHAR_PTR symbol) {

    //use ETHTokenInterface may case error later. JubiterBLD has no problem.
    auto token = dynamic_cast<ETHTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string tokenName = std::string(symbol);
//    abcd::DataChunk vContractAddress;
//    bool rv = base58::DecodeBase58Check(contractAddress, vContractAddress);
//    if (!rv) {
//        JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
//    }
//    vContractAddress.erase(vContractAddress.begin());
//       uchar_vector address = contractAddress;

    std::string _contractAddress = std::string(ETH_PRDFIX) + contractAddress;
    JUB_VERIFY_RV(token->SetERC20ETHToken(tokenName,
                                          decimal,
                                          _contractAddress));

    return JUBR_OK;
}

JUB_RV ContextBTC::BuildQRC20Outputs(IN JUB_UINT64 gasLimit, IN JUB_UINT64 gasPrice, IN JUB_CHAR_PTR contractAddress, IN JUB_CHAR_PTR to, IN JUB_CHAR_PTR value, OUT OUTPUT_BTC outputs[1]) {
    outputs[0].type = SCRIPT_BTC_TYPE::QRC20;

    uchar_vector data;

    data << (JUB_UINT8)0x01;
    data << (JUB_UINT8)0x04;
    data << (JUB_UINT8)0x08;
    data << (uint64_t)gasLimit;
    data << (JUB_UINT8)0x08;
    data << (uint64_t)gasPrice;

//    abcd::DataChunk vContractAddress;
//    bool rv = base58::DecodeBase58Check(contractAddress, vContractAddress);
//    if (!rv) {
//        JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
//    }
//    vContractAddress.erase(vContractAddress.begin());

    uchar_vector vContractAddress = std::string(contractAddress);

    abcd::DataChunk vToAddress;
    bool rv = false;
    int toAddressLen = (int)strlen(to);
    uint8_t* toAddress = new uint8_t[toAddressLen];
    memset(toAddress, 0x00, toAddressLen);
    toAddressLen = base58_decode_check(to, HasherType::HASHER_SHA2D, toAddress, toAddressLen);
    if (toAddressLen > 0) {
        uchar_vector v(toAddress, toAddressLen);
        vToAddress = v;
        rv = true;
    }
    delete[] toAddress; toAddress = NULL;
    if (!rv) {
        JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
    }
    vToAddress.erase(vToAddress.begin());

    std::vector<JUB_BYTE> vValue = jub::HexStr2CharPtr(DecStringToHexString(std::string(value)));
    uchar_vector vAbi = jub::eth::ERC20Abi::serialize(vToAddress, vValue);

    data && vAbi;
    data && vContractAddress;
    data << (JUB_UINT8)0xc2;

    //build qrc20 here
    outputs[0].qrc20.dataLen = data.size();
    memcpy(outputs[0].qrc20.data,&data[0],data.size());

    return JUBR_OK;
}

JUB_RV ContextBTC::SerializeUnsignedTx(const JUB_BTC_TRANS_TYPE type,
                                       const std::vector<INPUT_BTC>& vInputs,
                                       const std::vector<OUTPUT_BTC>& vOutputs,
                                       const JUB_UINT32 lockTime,
                                       uchar_vector& unsignedRaw) {

    JUB_VERIFY_RV(jub::btc::serializeUnsignedTx(type, vInputs, vOutputs, lockTime, unsignedRaw));

    return JUBR_OK;
}
   

JUB_RV ContextBTC::DeserializeTx(const std::string& hexTx,
                                 std::vector<INPUT_BTC>& vInputs,
                                 std::vector<OUTPUT_BTC>& vOutputs,
                                 JUB_UINT32_PTR plockTime) {

    if (dynamic_cast<jub::ContextMultisigBTC*>(this)) {
        dynamic_cast<jub::ContextMultisigBTC*>(this)->setIncTx(hexTx);
    }

    bool bWitness = false;

    JUB_VERIFY_RV(jub::btc::deserializeTx(bWitness,
                                          hexTx,
                                          vInputs,
                                          vOutputs,
                                          plockTime));

    return JUBR_OK;
}

JUB_RV ContextBTC::SignTX(const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt, const std::vector<INPUT_BTC>& vInputs, const std::vector<OUTPUT_BTC>& vOutputs, const JUB_UINT32 lockTime, std::string& raw) {
    JUB_RV ret = JUBR_OK;

    auto token = dynamic_cast<BTCTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    //JUB_VERIFY_RV(token->setUnit_BTC(_unitType));
    //JUB_VERIFY_RV(token->setTimeout_BTC(_timeout));

    //deal inputs
    std::vector<JUB_UINT64> vInputAmount;
    std::vector<std::string> vInputPath;
    for (auto input : vInputs) {
        vInputAmount.push_back(input.amount);
        vInputPath.push_back(_FullBip32Path(input.path));
    }

    //deal outputs
    std::vector<JUB_UINT16> vChangeIndex;
    std::vector<std::string> vChangePath;
    for (std::size_t i = 0, e = vOutputs.size(); i != e; ++i) {
        if (SCRIPT_BTC_TYPE::P2PKH == vOutputs[i].type) {
            if (vOutputs[i].stdOutput.changeAddress) {
                vChangeIndex.push_back((JUB_UINT16)i);
                vChangePath.push_back(_FullBip32Path(vOutputs[i].stdOutput.path));
            }
        }
    }

    //build unsinged transaction
    uchar_vector unsignedTrans;
    if (   COINBTC  == _coinType
        || COINBCH  == _coinType
        || COINLTC  == _coinType
        || COINUSDT == _coinType
        || COINQTUM == _coinType
        ) { //BTC&LTC
        ret = jub::btc::serializeUnsignedTx(_transType, vInputs, vOutputs, lockTime, unsignedTrans);
    }
    else {
        ret = JUBR_IMPL_NOT_SUPPORT;
    }
    JUB_VERIFY_RV(ret);

    uchar_vector vRaw;
    JUB_VERIFY_RV(token->SignTXBTC(addrFmt,
                                   _transType,
                                   (JUB_UINT16)vInputs.size(),
                                   vInputAmount,
                                   vInputPath,
                                   vChangeIndex,
                                   vChangePath,
                                   unsignedTrans,
                                   vRaw));

    raw = vRaw.getHex();

    return JUBR_OK;
}

JUB_RV ContextMultisigBTC::SerializeTx(const std::vector<INPUT_BTC>& vInputs,
                                       const MAP_COSIGNER_SIGNATURE& mapSignatureRaw,
                                       const std::vector<uchar_vector>& vRedeemScript,
                                       const std::vector<OUTPUT_BTC>& vOutputs,
                                       const JUB_UINT32 lockTime,
                                       uchar_vector& signedIncRaw) {

    JUB_VERIFY_RV(jub::btc::serializeTx(_coinType,
                                        _transType,
                                        _m,
                                        vInputs, mapSignatureRaw, vRedeemScript,
                                        vOutputs,
                                        lockTime,
                                        signedIncRaw));

    return JUBR_OK;
}

JUB_RV ContextMultisigBTC::OrderDHPublicKey(const BIP32_Path& path,
                                            std::vector<uchar_vector>& vOrderedPublickey) {
    JUB_RV rv = JUBR_ERROR;

    // derive key
    std::vector<std::string> vPublickey;
    //hex main xpub, derive xpub
    std::map<std::string, std::string> mapDeriveXpub;
    for (int i=0; i<_n; ++i) {
        // derive sub pubkey
        std::string mainXpub = getCosignerMainXpub(i);
        if ("" == mainXpub) {
            JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
            break;
        }
        std::string fullPath = getCosignerFullBip32Path(path, i);

        HDNode hdkey;
        JUB_UINT32 parentFingerprint = 0;

        rv = hdnode_pub_ckd(mainXpub, fullPath, _curveName, _xpubPrefix, _xprvPrefix, &hdkey, &parentFingerprint);
        if (JUBR_OK != rv) {
            break;
        }

        uchar_vector publicKey(hdkey.public_key, sizeof(hdkey.public_key)/sizeof(uint8_t));
        std::string xpub = publicKey.getHex();
        vPublickey.push_back(xpub);
        mapDeriveXpub.insert({mainXpub, xpub});
    }
    JUB_VERIFY_RV(rv);

    // order
    std::sort(vPublickey.begin(), vPublickey.end());
    // ordered Index
    std::string myMainXpub = getCosignerMainXpub();
    auto search = mapDeriveXpub.find(myMainXpub);
    if (mapDeriveXpub.end() == search) {
        JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
    }
    auto pos = std::find(vPublickey.begin(), vPublickey.end(), search->second);
    long orderedIndex = pos - vPublickey.begin();
    setOrderedIndex(orderedIndex);

    // out
    for (const auto& pubkey:vPublickey) {
        uchar_vector vPubkey;
        vPubkey.setHex(pubkey);
        vOrderedPublickey.push_back(vPubkey);
    }

    return JUBR_OK;
}

JUB_RV ContextMultisigBTC::BuildRedeemScript(const BIP32_Path& path,
                                             uchar_vector& redeemScript) {

    if (-1 == getCosignerIndex()) {
        JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
    }

    std::vector<uchar_vector> vOrderedPublickey;
    JUB_VERIFY_RV(OrderDHPublicKey(path,
                                   vOrderedPublickey));

    JUB_VERIFY_RV(jub::btc::buildRedeemScript(_transType,
                                              _m,
                                              _n,
                                              vOrderedPublickey,
                                              redeemScript));

    return JUBR_OK;
}

JUB_RV ContextMultisigBTC::BuildRedeemScriptTlv(const BIP32_Path& path,
                                                uchar_vector& redeemScriptTlv) {

    if (-1 == getCosignerIndex()) {
        JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
    }

    std::vector<uchar_vector> vOrderedPublickey;
    JUB_VERIFY_RV(OrderDHPublicKey(path,
                                   vOrderedPublickey));

    JUB_VERIFY_RV(jub::btc::buildRedeemScriptTlv(_transType,
                                                 _m,
                                                 _n,
                                                 vOrderedPublickey,
                                                 _orderedIndex,
                                                 redeemScriptTlv));

    return JUBR_OK;
}

JUB_RV ContextMultisigBTC::GetAddress(const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt, const BIP32_Path& path, const JUB_UINT16 tag, std::string& address) {

    auto token = dynamic_cast<BTCTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    uchar_vector redeemScriptTlv;
    JUB_VERIFY_RV(BuildRedeemScriptTlv(path, redeemScriptTlv));

    std::string strPath = _FullBip32Path(path);
    JUB_VERIFY_RV(token->GetAddressMultisigBTC(addrFmt, _transType, strPath, tag, redeemScriptTlv, address));

    return JUBR_OK;
}

JUB_RV ContextMultisigBTC::DeserializeTx(const std::vector<uchar_vector>& vRedeemScript,
                                         const std::string& hexIncTx,
                                         MAP_COSIGNER_SIGNATURE& mapCosignerSignature,
                                         uchar_vector& unsignedTx) {

    bool bWitness = false;

    JUB_VERIFY_RV(jub::btc::deserializeTx(bWitness,
                                          vRedeemScript,
                                          hexIncTx,
                                          mapCosignerSignature,
                                          unsignedTx));

    return JUBR_OK;
}

JUB_RV ContextMultisigBTC::GetP2SHMultisigAddress(const BIP32_Path& path, const JUB_UINT16 tag, std::string& address) {

    auto token = dynamic_cast<BTCTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    uchar_vector redeemScript;
    JUB_VERIFY_RV(BuildRedeemScript(path, redeemScript));
    std::vector<unsigned char> hRedeemScript;
    size_t hRedeemScriptSz = Hash160(redeemScript, hRedeemScript);
    if (0 == hRedeemScriptSz) {
        JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
    }

    uchar_vector script;
    script << (unsigned char)jub::btc::p2shVersion;
    script << hRedeemScript;
    int scpLen = (int)script.size();
    uint8_t *scp = new uint8_t[scpLen+1];
    memset(scp, 0x00, scpLen+1);
    std::copy(script.begin(), script.end(), scp);

    int strsize = scpLen*3;
    char *str = new char[strsize];
    memset(str, 0x00, strsize);
    strsize = base58_encode_check(scp, scpLen, HasherType::HASHER_SHA2D, str, strsize);
    delete[] scp; scp = NULL;
    if (strsize > 0) {
        address = std::string(str);
    }

    delete[] str; str = NULL;

    return JUBR_OK;
}

std::string ContextMultisigBTC::getCosignerFullBip32Path(const BIP32_Path& path, const long cosignerIndex) {

    return _mainPath
            + "/" + std::to_string(cosignerIndex)
            + "/" + std::to_string(path.change)
            + "/" + std::to_string(path.addressIndex);
}

std::string ContextMultisigBTC::_FullBip32Path(const BIP32_Path& path) {

    if (-1 == _cosignerIndex) {
        return "";
    }

    return getCosignerFullBip32Path(path, _cosignerIndex);
}

JUB_RV ContextMultisigBTC::pushAll(std::vector<uchar_vector>& results) {

    std::vector<uchar_vector> vSignatureRaw;
    for (const auto& result:results) {
        uchar_vector raw(result);
        pushAll(raw);

        vSignatureRaw.push_back(raw);
    }

    results.clear();
    results = vSignatureRaw;

    return JUBR_OK;
}

JUB_RV ContextMultisigBTC::pushAll(uchar_vector& result) {

    uint8_t hashType = 0x01;//[ALL]
    uint8_t hashTypeLen = sizeof(hashType)/sizeof(uint8_t);

    switch (_coinType) {
        case COINBTC:
        case COINLTC:
        case COINUSDT:
        case COINDASH:
        case COINQTUM:
            hashType |= 0x00;
            break;
        case COINBCH:
            hashType |= 0x40;
            break;
        default:
            hashTypeLen = 0x00;
            break;
    }

    uchar_vector raw;
    raw.push_back(result.size()+hashTypeLen);
    raw << result;

    if (hashTypeLen) {
        raw.push_back(hashType);
    }

    result.clear();
    result = raw;

    return JUBR_OK;
}

JUB_RV ContextMultisigBTC::pullAll(uchar_vector& result) {

    uint8_t hashType = 0x01;//[ALL]
    uint8_t hashTypeLen = sizeof(hashType)/sizeof(uint8_t);

    switch (_coinType) {
        case COINBTC:
        case COINLTC:
        case COINUSDT:
        case COINDASH:
        case COINQTUM:
            hashType |= 0x00;
            break;
        case COINBCH:
            hashType |= 0x40;
            break;
        default:
            hashTypeLen = 0x00;
            break;
    }

    if (hashTypeLen) {
        if (hashType == result[result.size()-1]) {
            result.pop_back();
        }
        else {
            return JUBR_ARGUMENTS_BAD;
        }
    }

    return JUBR_OK;
}

JUB_RV ContextMultisigBTC::OrderSignatures(const std::vector<INPUT_BTC>& vInputs, const std::vector<uchar_vector>& vRedeemScript,
                                           const std::vector<OUTPUT_BTC>& vOutputs,
                                           const JUB_UINT32 lockTime,
                                           const MAP_COSIGNER_SIGNATURE& mapCosignerSignature,
                                           const std::vector<uchar_vector>& vSignatureRaw,
                                           MAP_COSIGNER_SIGNATURE& mapSignatureOrdered) {

    for (size_t inputIndex=0; inputIndex<vInputs.size(); ++inputIndex) {

        uchar_vector preimageRaw;
        JUB_VERIFY_RV(jub::btc::serializePreimage(_coinType,
                                                  _transType,
                                                  inputIndex,
                                                  vInputs, vRedeemScript,
                                                  vOutputs,
                                                  lockTime,
                                                  preimageRaw));

        std::vector<uchar_vector> vOrderedPublickey;
        JUB_VERIFY_RV(OrderDHPublicKey(vInputs[inputIndex].path,
                                       vOrderedPublickey));

        std::vector<std::string> vSignature;
        bool isFind = false;
        for (size_t orderIndex=0; orderIndex<vOrderedPublickey.size(); ++orderIndex) {

            isFind = false;
            // enum signatures in incTx
            std::vector<std::string> vSignatureFromIncTx = getVectorFromMap(std::to_string(inputIndex), mapCosignerSignature);
            for (size_t signatureFromIncTxIndex=0; signatureFromIncTxIndex<vSignatureFromIncTx.size(); ++signatureFromIncTxIndex) {
                uchar_vector signatureFromIncTxRaw;
                signatureFromIncTxRaw.setHex(vSignatureFromIncTx[signatureFromIncTxIndex]);
                uint8_t ulSignatureLen = signatureFromIncTxRaw[0];
                uchar_vector derSignatureFromIncTx(signatureFromIncTxRaw.begin()+1, signatureFromIncTxRaw.begin()+1+ulSignatureLen);

                pullAll(derSignatureFromIncTx);

                uchar_vector signatureFromIncTx(64);
                if (1 != ecdsa_der_to_sig(&derSignatureFromIncTx[0], &signatureFromIncTx[0])) {
                    JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
                }

                if (0 == ecdsa_verify(&secp256k1, HasherType::HASHER_SHA2D,
                                      &vOrderedPublickey[orderIndex][0], &signatureFromIncTx[0],
                                      &preimageRaw[0], (uint32_t)preimageRaw.size())
                    ) {
                    vSignature.push_back(vSignatureFromIncTx[signatureFromIncTxIndex]);
                    isFind = true;
                }
            }
            // enum signature that device gen
            if (!isFind) {
                uint8_t ulSignatureLen = vSignatureRaw[inputIndex][0];
                uchar_vector derSignature(vSignatureRaw[inputIndex].begin()+1, vSignatureRaw[inputIndex].begin()+1+ulSignatureLen);

                pullAll(derSignature);

                uchar_vector signature(64);
                if (1 != ecdsa_der_to_sig(&derSignature[0], &signature[0])) {
                    JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
                }

                if (0 == ecdsa_verify(&secp256k1, HasherType::HASHER_SHA2D,
                                      &vOrderedPublickey[orderIndex][0], &signature[0],
                                      &preimageRaw[0], (uint32_t)preimageRaw.size())
                    ) {

                    vSignature.push_back(vSignatureRaw[inputIndex].getHex());
                    isFind = true;
                }
            }
        }

        mapSignatureOrdered.insert({std::to_string(inputIndex), vSignature});
    }

    if (0 == mapSignatureOrdered.size()) {
        JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
    }

    return JUBR_OK;
}

JUB_RV ContextMultisigBTC::SignTX(const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt, const std::vector<INPUT_BTC>& vInputs, const std::vector<OUTPUT_BTC>& vOutputs, const JUB_UINT32 lockTime, std::string& raw) {
    JUB_RV ret = JUBR_OK;

    auto token = dynamic_cast<BTCTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    //JUB_VERIFY_RV(token->setUnit_BTC(_unitType));
    //JUB_VERIFY_RV(token->setTimeout_BTC(_timeout));

    //deal inputs
    std::vector<JUB_UINT64> vInputAmount;
    std::vector<std::string> vInputPath;
    std::vector<uchar_vector> vRedeemScriptTlv;
    std::vector<uchar_vector> vRedeemScript;
    for (auto input : vInputs) {
        vInputAmount.push_back(input.amount);
        vInputPath.push_back(_FullBip32Path(input.path));

        uchar_vector redeemScript;
        JUB_VERIFY_RV(BuildRedeemScript(input.path, redeemScript));
        vRedeemScript.push_back(redeemScript);

        uchar_vector redeemScriptTlv;
        JUB_VERIFY_RV(BuildRedeemScriptTlv(input.path, redeemScriptTlv));
        vRedeemScriptTlv.push_back(redeemScriptTlv);
    }

    //deal outputs
    std::vector<JUB_UINT16> vChangeIndex;
    std::vector<std::string> vChangePath;
    std::vector<uchar_vector> vChangeRedeemScriptTlv;
    for (std::size_t i = 0, e = vOutputs.size(); i != e; ++i) {
        if (SCRIPT_BTC_TYPE::RETURN0 == vOutputs[i].type) {
            ret = JUBR_ARGUMENTS_BAD;
            break;
        }
        if (vOutputs[i].stdOutput.changeAddress) {
            vChangeIndex.push_back((JUB_UINT16)i);
            vChangePath.push_back(_FullBip32Path(vOutputs[i].stdOutput.path));

            if (SCRIPT_BTC_TYPE::P2SH_MULTISIG == vOutputs[i].type) {
                // The change address is the same as the transfer address
                uchar_vector changeRedeemScriptTlv;
                JUB_VERIFY_RV(BuildRedeemScriptTlv(vOutputs[i].stdOutput.path,changeRedeemScriptTlv));
                vChangeRedeemScriptTlv.push_back(changeRedeemScriptTlv);
            }
        }
    }
    if (JUBR_OK != ret) {
        JUB_VERIFY_RV(ret);
    }

    //build unsinged transaction
    uchar_vector unsignedTrans;
    MAP_COSIGNER_SIGNATURE mapCosignerSignature;
    if (   COINBTC  == _coinType
        || COINBCH  == _coinType
        || COINLTC  == _coinType
//        || COINUSDT == _coinType
        ) {
        ret = SerializeUnsignedTx(_transType, vInputs, vOutputs, lockTime, unsignedTrans);
    }
    else {
        ret = JUBR_IMPL_NOT_SUPPORT;
    }
    if (JUBR_OK != ret) {
        JUB_VERIFY_RV(ret);
    }

    if (0 != _hexIncTx.compare("")) {
        uchar_vector unsignedTx;
        JUB_VERIFY_RV(DeserializeTx(vRedeemScript,
                                    _hexIncTx,
                                    mapCosignerSignature,
                                    unsignedTx));
        if (unsignedTx != unsignedTrans) {
            JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
        }
    }

//    uchar_vector hash(32);
//    for (size_t inputIndex=0; inputIndex<vInputs.size(); ++inputIndex) {
//        uchar_vector preimageRaw;
//        JUB_VERIFY_RV(jub::btc::serializePreimage(_transType,
//                                                  inputIndex,
//                                                  vInputs, vRedeemScript,
//                                                  vOutputs,
//                                                  lockTime,
//                                                  preimageRaw));
//        std::cout << "preimage: " << preimageRaw.getHex() << std::endl;
//        size_t szHashLen = dHash256(preimageRaw, hash);
//        std::cout << "dHash256: " << hash.getHex() << std::endl;
//    }

    std::vector<uchar_vector> vSignatureRaw;
    JUB_VERIFY_RV(token->SignTXMultisigBTC(addrFmt,
                                           _transType,
                                           (JUB_UINT16)vInputs.size(),
                                           vInputAmount, vInputPath, vRedeemScriptTlv,
                                           vChangeIndex, vChangePath, vChangeRedeemScriptTlv,
                                           unsignedTrans,
                                           vSignatureRaw));

    //pushAll
    JUB_VERIFY_RV(pushAll(vSignatureRaw));

    //order signature
    MAP_COSIGNER_SIGNATURE mapSignatureOrdered;
    JUB_VERIFY_RV(OrderSignatures(vInputs, vRedeemScript,
                                  vOutputs,
                                  lockTime,
                                  mapCosignerSignature, vSignatureRaw,
                                  mapSignatureOrdered));

    //build transaction
    uchar_vector vRaw;
    JUB_RV rv = SerializeTx(vInputs, mapSignatureOrdered, vRedeemScript,
                            vOutputs,
                            lockTime,
                            vRaw);
    if (            JUBR_OK != rv
        && JUBR_MULTISIG_OK != rv
        ) {

        JUB_VERIFY_RV(rv);
    }

    raw = vRaw.getHex();

    JUB_VERIFY_RV(rv);

    return JUBR_OK;
}

JUB_RV ContextBTC::ParseTransaction(const std::string& incRaw,
                                    std::vector<INPUT_BTC>& vInputs,
                                    std::vector<OUTPUT_BTC>& vOutputs,
                                    JUB_UINT32_PTR plockTime) {

    JUB_VERIFY_RV(DeserializeTx(incRaw,
                               vInputs,
                               vOutputs,
                               plockTime));

    return JUBR_OK;
}

} // namespace jub end
