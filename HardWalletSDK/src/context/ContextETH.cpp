#include "utility/util.h"

#include "context/ContextETH.h"
#include "token/interface/TokenInterface.hpp"
#include "libETH/ERC20Abi.h"
#include "libETH/libETH.hpp"
#include "TrustWallet/wallet-core/src/Hash.h"
#include "TrezorCrypto/ecdsa.h"
#include "TrezorCrypto/secp256k1.h"

namespace jub {

JUB_RV ContextETH::ActiveSelf() {

    auto token = dynamic_cast<ETHTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    auto ctoken = dynamic_cast<CommonTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);
    JUB_CHECK_NULL(ctoken);

    JUB_VERIFY_RV(token->SelectAppletETH());
    JUB_VERIFY_RV(token->GetAppletVersionETH(_appletVersion));
    JUB_VERIFY_RV(ctoken->SetTimeout(_timeout));

    //ETH don`t set unit
    return JUBR_OK;
}

JUB_RV ContextETH::GetAddress(const BIP32_Path& path, const JUB_UINT16 tag, std::string& address) {

    auto token = dynamic_cast<ETHTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string strPath = _FullBip32Path(path);
    JUB_VERIFY_RV(token->GetAddressETH(strPath, tag, address));

    return JUBR_OK;
}

JUB_RV ContextETH::GetMainHDNode(const JUB_BYTE format, std::string& xpub) {

    auto token = dynamic_cast<ETHTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->GetHDNodeETH(format, _mainPath, xpub));

    return JUBR_OK;
}

JUB_RV ContextETH::SetMyAddress(const BIP32_Path& path, std::string& address) {

    auto token = dynamic_cast<ETHTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string strPath = _FullBip32Path(path);
    JUB_VERIFY_RV(token->GetAddressETH(strPath, 0x02, address));

    return JUBR_OK;
}

JUB_RV ContextETH::GetHDNode(const JUB_BYTE& format, const BIP32_Path& path, std::string& pubkey) {

    auto token = dynamic_cast<ETHTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string strPath = _FullBip32Path(path);
    JUB_VERIFY_RV(token->GetHDNodeETH(format, strPath, pubkey));

    return JUBR_OK;
}

JUB_RV ContextETH::SignTransaction(const BIP32_Path& path,
                                   const JUB_UINT32 nonce,
                                   const JUB_UINT32 gasLimit,
                                   const std::string& gasPriceInWei,
                                   const std::string& to,
                                   IN JUB_CHAR_CPTR valueInWei,
                                   IN JUB_CHAR_CPTR input,
                                   OUT std::string& strRaw) {

    auto token = dynamic_cast<ETHTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::vector<JUB_BYTE> vNonce = jub::HexStr2CharPtr(numberToHexString(nonce));
    std::vector<JUB_BYTE> vGasLimit = jub::HexStr2CharPtr(numberToHexString(gasLimit));
    std::vector<JUB_BYTE> vGasPriceInWei = jub::HexStr2CharPtr(DecStringToHexString(std::string(gasPriceInWei)));
    std::vector<JUB_BYTE> vTo = jub::ETHHexStr2CharPtr(to);
    std::vector<JUB_BYTE> vValueInWei;
    if (nullptr != valueInWei
        &&    0 != strlen(valueInWei)
        ) {
        vValueInWei = jub::HexStr2CharPtr(DecStringToHexString(std::string(valueInWei)));
    }

    std::vector<JUB_BYTE> vInput;
    if (nullptr != input
        &&    0 != strlen(input)
        ) {
        vInput = jub::ETHHexStr2CharPtr(input);
        if (0 == vInput.size()) {
            vInput = jub::HexStr2CharPtr(input);
        }
        if (0 == vInput.size()) {
            return JUBR_ARGUMENTS_BAD;
        }
    }

    std::string strPath = _FullBip32Path(path);
    std::vector<JUB_BYTE> vPath(strPath.begin(), strPath.end());

    std::vector<JUB_BYTE> vChainID;
    vChainID.push_back(_chainID);

    bool bERC20 = false;
    if (0 == memcmp(uchar_vector(vInput).getHex().c_str(), ABI_METHOD_ID_TRANSFER, strlen(ABI_METHOD_ID_TRANSFER))) { // erc20 function sign
        bERC20 = true;
    }

    uchar_vector raw;
    JUB_VERIFY_RV(token->SignTXETH(bERC20,
                                   vNonce,
                                   vGasPriceInWei,
                                   vGasLimit,
                                   vTo,
                                   vValueInWei,
                                   vInput,
                                   vPath,
                                   vChainID,
                                   raw));
    strRaw = std::string(ETH_PRDFIX) + raw.getHex();

    return JUBR_OK;
}

JUB_RV ContextETH::BuildERC20Abi(JUB_CHAR_CPTR to, JUB_CHAR_CPTR value, std::string& abi) {

    std::vector<JUB_BYTE> vTo = jub::ETHHexStr2CharPtr(to);
    std::vector<JUB_BYTE> vValue = jub::HexStr2CharPtr(DecStringToHexString(std::string(value)));
    uchar_vector vAbi = jub::eth::ERC20Abi::serialize(vTo, vValue);
    abi = std::string(ETH_PRDFIX) + vAbi.getHex();

    return JUBR_OK;
}

JUB_RV ContextETH::SetERC20ETHToken(JUB_CHAR_CPTR pTokenName,
                                    JUB_UINT16 unitDP,
                                    JUB_CHAR_CPTR pContractAddress) {

    // ETH token extension apdu
    if (0 > _appletVersion.compare(APPLET_VERSION_SUPPORT_EXT_TOKEN)) {
        return JUBR_OK;
    }

    JUB_CHECK_NULL(pTokenName);
    JUB_CHECK_NULL(pContractAddress);

    auto token = dynamic_cast<ETHTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::string tokenName = std::string(pTokenName);
    std::string contractAddress = std::string(pContractAddress);
    JUB_VERIFY_RV(token->SetERC20ETHToken(tokenName,
                                          unitDP,
                                          contractAddress));

    return JUBR_OK;
}

JUB_RV ContextETH::SignContract(const BIP32_Path& path,
                                const JUB_UINT32 nonce,
                                const JUB_UINT32 gasLimit,
                                const std::string& gasPriceInWei,
                                const std::string& to,
                                IN JUB_CHAR_CPTR valueInWei,
                                IN JUB_CHAR_CPTR input,
                                OUT std::string& strRaw) {

    auto token = dynamic_cast<ETHTokenInterface*>(jub::TokenManager::GetInstance()->GetOne(_deviceID));
    JUB_CHECK_NULL(token);

    std::vector<JUB_BYTE> vNonce = jub::HexStr2CharPtr(numberToHexString(nonce));
    std::vector<JUB_BYTE> vGasLimit = jub::HexStr2CharPtr(numberToHexString(gasLimit));
    std::vector<JUB_BYTE> vGasPriceInWei = jub::HexStr2CharPtr(DecStringToHexString(std::string(gasPriceInWei)));
    std::vector<JUB_BYTE> vTo = jub::ETHHexStr2CharPtr(to);
    std::vector<JUB_BYTE> vValueInWei;
    if (nullptr != valueInWei
        &&    0 != strlen(valueInWei)
        ) {
        vValueInWei = jub::HexStr2CharPtr(DecStringToHexString(std::string(valueInWei)));
    }

    std::vector<JUB_BYTE> vInput;
    if (nullptr != input
        &&    0 != strlen(input)
        ) {
        vInput = jub::ETHHexStr2CharPtr(input);
    }
    // parse input
    JUB_BYTE inputType = (JUB_BYTE)jub::eth::ENUM_CONTRACT_ABI::CREATE_CONTRACT;
    if (0 < vTo.size()) {
        inputType = _getInputType(std::string(std::string(ETH_PRDFIX)+jub::eth::ContractAbi::parseMethodID(vInput)));
    }
    // add support UniswapV2Router02
//    if (jub::eth::ENUM_CONTRACT_ABI::NS_ITEM == inputType) {
//        return JUBR_ARGUMENTS_BAD;
//    }

    std::string strPath = _FullBip32Path(path);
    std::vector<JUB_BYTE> vPath(strPath.begin(), strPath.end());

    std::vector<JUB_BYTE> vChainID;
    vChainID.push_back(_chainID);

    uchar_vector vPreimageHash;
    JUB_VERIFY_RV(jub::eth::preimageHash(vNonce,
                                         vGasPriceInWei,
                                         vGasLimit,
                                         vTo,
                                         vValueInWei,
                                         vInput,
                                         vChainID,
                                         vPreimageHash));

    uchar_vector signatureRaw;
    if (jub::eth::ENUM_CONTRACT_ABI::CREATE_CONTRACT != inputType) {
        JUB_VERIFY_RV(token->SignContractETH(inputType,
                                             vNonce,
                                             vGasPriceInWei,
                                             vGasLimit,
                                             vTo,
                                             vValueInWei,
                                             vInput,
                                             vPath,
                                             vChainID,
                                             signatureRaw));
    }
    else {
        TW::Data vInputHash = TW::Hash::sha256(vInput);

        JUB_VERIFY_RV(token->SignContractHashETH(inputType,
                                                 vGasLimit,
                                                 vInputHash,
                                                 vPreimageHash,
                                                 vPath,
                                                 vChainID,
                                                 signatureRaw));
    }

    std::string pubkey;
    JUB_VERIFY_RV(token->GetHDNodeETH(HEX, strPath, pubkey));
    uchar_vector vPubkey(pubkey);
    uint8_t uncompressed[65] = {0x00,};
    if (1 != ecdsa_uncompress_pubkey(&secp256k1, &vPubkey[1], uncompressed)) {
        return JUBR_ARGUMENTS_BAD;
    }

    uint8_t v = 0xFF;
    uint8_t rpub_key[65] = {0x00,};
    for (int i=0; i<3; ++i) {
        if ( 0 == ecdsa_recover_pub_from_sig(&secp256k1, rpub_key, &signatureRaw[0], &vPreimageHash[0], i)) {
            if (0 == std::memcmp(uncompressed, rpub_key, 65)) {
                v = i;
                v += (vChainID[0]*2 + 35);
                signatureRaw[64] = v;
                break;
            }
        }
    }
    if (0xFF == v) {
        return JUBR_ARGUMENTS_BAD;
    }

    uchar_vector raw;
    JUB_VERIFY_RV(jub::eth::serializeTx(vNonce,
                                        vGasPriceInWei,
                                        vGasLimit,
                                        vTo,
                                        vValueInWei,
                                        vInput,
                                        signatureRaw,
                                        raw));

    strRaw = std::string(ETH_PRDFIX) + raw.getHex();

    return JUBR_OK;
}

//JUB_RV ContextETH::BuildContractWithAddrAbi(JUB_CHAR_CPTR methodID,
//                                            JUB_CHAR_CPTR address,
//                                            std::string& abi) {
//    _addContrFunc(std::string(methodID), jub::eth::ENUM_CONTRACT_ABI::WITH_ADDRESS);
//
//    std::vector<JUB_BYTE> vMethodID = jub::ETHHexStr2CharPtr(methodID);
//    std::vector<JUB_BYTE> vAddress = jub::ETHHexStr2CharPtr(address);
//    uchar_vector vAbi = jub::eth::ContractAbi::serialize(vMethodID, vAddress);
//    abi = std::string(ETH_PRDFIX) + vAbi.getHex();
//
//    return JUBR_OK;
//}
//
//JUB_RV ContextETH::BuildContractWithAddrAmtAbi(JUB_CHAR_CPTR methodID,
//                                               JUB_CHAR_CPTR address, JUB_CHAR_CPTR amount,
//                                               std::string& abi) {
//    _addContrFunc(std::string(methodID), jub::eth::ENUM_CONTRACT_ABI::WITH_ADDRESS_AMOUNT);
//
//    std::vector<JUB_BYTE> vMethodID = jub::ETHHexStr2CharPtr(methodID);
//    std::vector<JUB_BYTE> vAddress = jub::ETHHexStr2CharPtr(address);
//    std::vector<JUB_BYTE> vAmount = jub::HexStr2CharPtr(DecStringToHexString(std::string(amount)));
//    uchar_vector vAbi = jub::eth::ContractAbi::serialize(vMethodID, vAddress, vAmount);
//    abi = std::string(ETH_PRDFIX) + vAbi.getHex();
//
//    return JUBR_OK;
//}
//
JUB_RV ContextETH::BuildContractWithTxIDAbi(JUB_CHAR_CPTR methodID,
                                            JUB_CHAR_CPTR transactionID,
                                            std::string& abi) {
    _addContrFunc(std::string(methodID), jub::eth::ENUM_CONTRACT_ABI::WITH_TXID);

    std::vector<JUB_BYTE> vMethodID = jub::ETHHexStr2CharPtr(methodID);
    std::vector<JUB_BYTE> vTransactionID = jub::ETHHexStr2CharPtr(transactionID);
    uchar_vector vAbi = jub::eth::ContractAbi::serializeWithTxID(vMethodID, vTransactionID);
    abi = std::string(ETH_PRDFIX) + vAbi.getHex();

    return JUBR_OK;
}

//JUB_RV ContextETH::BuildContractWithAmtAbi(JUB_CHAR_CPTR methodID,
//                                           JUB_CHAR_CPTR amount,
//                                           std::string& abi) {
//    _addContrFunc(std::string(methodID), jub::eth::ENUM_CONTRACT_ABI::WITH_AMOUNT);
//
//    std::vector<JUB_BYTE> vMethodID = jub::ETHHexStr2CharPtr(methodID);
//    std::vector<JUB_BYTE> vAmount = jub::HexStr2CharPtr(DecStringToHexString(std::string(amount)));
//    uchar_vector vAbi = jub::eth::ContractAbi::serializeWithAmt(vMethodID, vAmount);
//    abi = std::string(ETH_PRDFIX) + vAbi.getHex();
//
//    return JUBR_OK;
//}

JUB_RV ContextETH::BuildContractWithAddrAmtDataAbi(JUB_CHAR_CPTR methodID,
                                                   JUB_CHAR_CPTR address, JUB_CHAR_CPTR amount, JUB_CHAR_CPTR data,
                                                   std::string& abi) {
    _addContrFunc(std::string(methodID), jub::eth::ENUM_CONTRACT_ABI::WITH_ADDRESS_AMOUNT_DATA);

    std::vector<JUB_BYTE> vMethodID = jub::ETHHexStr2CharPtr(methodID);
    std::vector<JUB_BYTE> vAddress = jub::ETHHexStr2CharPtr(address);
    std::vector<JUB_BYTE> vAmount = jub::HexStr2CharPtr(DecStringToHexString(std::string(amount)));
    std::vector<JUB_BYTE> vData = jub::ETHHexStr2CharPtr(data);
    uchar_vector udata(vData);
    uchar_vector vAbi = jub::eth::ContractAbi::serialize(vMethodID, vAddress, vAmount, vData);
    std::string _abi = vAbi.getHex();

    if (0 == memcmp(udata.getHex().c_str(), ABI_METHOD_ID_TRANSFER, strlen(ABI_METHOD_ID_TRANSFER))) { // erc20 function sign
        // 28 Zero
        _abi += "00000000000000000000000000000000000000000000000000000000";
    }
    abi = std::string(ETH_PRDFIX) + _abi;

    return JUBR_OK;
}
} // namespace jub end
