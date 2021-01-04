#include "token/JubiterBLDImpl.h"

namespace jub {

constexpr JUB_BYTE kPKIAID_ETH[16] = {
    0xD1, 0x56, 0x00, 0x01, 0x32, 0x83, 0x00, 0x42, 0x4C, 0x44, 0x00, 0x00, 0x45, 0x54, 0x48, 0x01
};

#define SWITCH_TO_ETH_APP                       \
do {				                            \
    JUB_VERIFY_RV(_SelectApp(kPKIAID_ETH, 16)); \
} while (0);                                    \

JUB_RV JubiterBLDImpl::SelectAppletETH() {
    SWITCH_TO_ETH_APP;
    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::GetAppletVersionETH(std::string &version) {

    uchar_vector appID(kPKIAID_ETH, 16);
    JUB_VERIFY_RV(GetAppletVersion(CharPtr2HexStr(appID), version));

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::GetAddressETH(const std::string& path, const JUB_UINT16 tag, std::string& address) {

    uchar_vector data(path.begin(), path.end());

    APDU apdu(0x00, 0xf6, 0x00, (JUB_BYTE)tag, (JUB_ULONG)data.size(), data.data(), 0x14);
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    uchar_vector vAddress(retData, (unsigned int)ulRetDataLen);
    address = std::string(ETH_PRDFIX) + vAddress.getHex();

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::GetHDNodeETH(const JUB_BYTE format, const std::string& path, std::string& pubkey) {

    //path = "m/44'/60'/0'";
    uchar_vector vPath;
    vPath << path;
    uchar_vector apduData = ToTlv(0x08, vPath);

    //0x00 for hex, 0x01 for xpub
    if (   JUB_ENUM_PUB_FORMAT::HEX  != format
        && JUB_ENUM_PUB_FORMAT::XPUB != format
        ) {
        return JUBR_ERROR_ARGS;
    }

    APDU apdu(0x00, 0xe6, 0x00, format, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    if (JUB_ENUM_PUB_FORMAT::HEX == format) {
        uchar_vector vPubkey(retData, (unsigned int)ulRetDataLen);
        pubkey = std::string(ETH_PRDFIX) + vPubkey.getHex();
    }
    else if (JUB_ENUM_PUB_FORMAT::XPUB == format) {
        pubkey = (JUB_CHAR_PTR)retData;
    }

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::SignTXETH(const bool bERC20,
                                 const std::vector<JUB_BYTE>& vNonce,
                                 const std::vector<JUB_BYTE>& vGasPrice,
                                 const std::vector<JUB_BYTE>& vGasLimit,
                                 const std::vector<JUB_BYTE>& vTo,
                                 const std::vector<JUB_BYTE>& vValue,
                                 const std::vector<JUB_BYTE>& vInput,
                                 const std::vector<JUB_BYTE>& vPath,
                                 const std::vector<JUB_BYTE>& vChainID,
                                 std::vector<JUB_BYTE>& vRaw) {

    uchar_vector data;

    if (0x00 == vNonce[0]) {
        data << (JUB_BYTE)0x41;
        data << (JUB_BYTE)0x00;
    }
    else {
        data << ToTlv(0x41, vNonce);
    }

    data << ToTlv(0x42, vGasPrice);
    data << ToTlv(0x43, vGasLimit);
    data << ToTlv(0x44, vTo);
    data << ToTlv(0x45, vValue);
    data << ToTlv(0x46, vInput);
    data << ToTlv(0x47, vPath);
    data << ToTlv(0x48, vChainID);

    JUB_BYTE ins = 0x2a;
    if (bERC20) {
        ins = 0xc8;
    }

    //one pack can do it
    APDU apdu(0x00, ins, 0x01, 0x00, (JUB_ULONG)data.size(), data.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    vRaw.clear();
    vRaw.insert(vRaw.end(), retData, retData + ulRetDataLen);

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::SetERC20ETHToken(const std::string& tokenName, const JUB_UINT16 unitDP, const std::string& contractAddress) {

    uchar_vector lvName = Tollv(tokenName);
    uchar_vector address;
    address << ETHHexStr2CharPtr(contractAddress);

    uchar_vector data;
    data << (uint8_t)unitDP;
    data << (uint8_t)lvName.size();
    data << lvName;
    data << (uint8_t)address.size();
    data << address;

    APDU apdu(0x00, 0xc7, 0x00, 0x00, (JUB_ULONG)data.size(), data.data());
    JUB_UINT16 ret = 0;
    JUB_VERIFY_RV(_SendApdu(&apdu, ret));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::SignContractETH(const JUB_BYTE inputType,
                                       const std::vector<JUB_BYTE>& vNonce,
                                       const std::vector<JUB_BYTE>& vGasPrice,
                                       const std::vector<JUB_BYTE>& vGasLimit,
                                       const std::vector<JUB_BYTE>& vTo,
                                       const std::vector<JUB_BYTE>& vValue,
                                       const std::vector<JUB_BYTE>& vInput,
                                       const std::vector<JUB_BYTE>& vPath,
                                       const std::vector<JUB_BYTE>& vChainID,
                                       std::vector<JUB_BYTE>& signatureRaw) {

    constexpr JUB_UINT32 kSendOnceLen = 230;

    uchar_vector apduData;

    if (0x00 == vNonce[0]) {
        apduData << (JUB_BYTE)0x41;
        apduData << (JUB_BYTE)0x00;
    }
    else {
        apduData << ToTlv(0x41, vNonce);
    }

    apduData << ToTlv(0x42, vGasPrice);
    apduData << ToTlv(0x43, vGasLimit);
    apduData << ToTlv(0x44, vTo);
    apduData << ToTlv(0x45, vValue);
    // Too length to send at here
//    apduData << ToTlv(0x46, vInput);
    apduData << ToTlv(0x47, vPath);
    apduData << ToTlv(0x48, vChainID);

    //  first pack
    APDU apdu(0x00, 0xF8, 0x01, 0x00, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_VERIFY_RV(_SendApdu(&apdu, ret));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }
    apduData.clear();

    uchar_vector tlvInput;
    tlvInput << ToTlv(inputType, vInput);
    apduData << ToTlv(0x46, tlvInput);
    unsigned long iCnt = apduData.size()/kSendOnceLen;
    JUB_UINT32 iRemainder = apduData.size()%kSendOnceLen;
    if (iCnt) {
        int bOnce = false;
        for (unsigned long i=0; i<iCnt; ++i) {
            if (   (i+1) == iCnt
                &&    0  == iRemainder
                ) {
                bOnce = true;
            }
            uchar_vector apduDataPart(&apduData[i*kSendOnceLen], kSendOnceLen);
            JUB_VERIFY_RV(_TranPack(apduDataPart, 0x00, 0x00, kSendOnceLen, bOnce));  // last data or not.
        }
    }
    if (iRemainder) {
        uchar_vector apduDataPart(&apduData[iCnt*kSendOnceLen], iRemainder);
        JUB_VERIFY_RV(_TranPack(apduDataPart, 0x00, 0x00, kSendOnceLen, true));  // last data.
    }
    apduData.clear();

    //  sign transactions
    JUB_BYTE ins = 0xc9;
    apdu.SetApdu(0x00, ins, 0x00, 0x00, 0);
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x6f09 == ret) {
        return JUBR_USER_CANCEL;
    }
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    signatureRaw.clear();
    signatureRaw.insert(signatureRaw.end(), retData, retData + ulRetDataLen);

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::SignContractHashETH(const JUB_BYTE inputType,
                                           const std::vector<JUB_BYTE>& vGasLimit,
                                           const std::vector<JUB_BYTE>& vInputHash,
                                           const std::vector<JUB_BYTE>& vUnsignedTxHash,
                                           const std::vector<JUB_BYTE>& vPath,
                                           const std::vector<JUB_BYTE>& vChainID,
                                           std::vector<JUB_BYTE>& signatureRaw) {

    uchar_vector data;

    data << ToTlv(0x43, vGasLimit);
    data << ToTlv(0x07, vUnsignedTxHash);
    // Too length to send at here
//    data << ToTlv(0x46, vInput);
    data << ToTlv(0x47, vPath);
    data << ToTlv(0x48, vChainID);
    uchar_vector tlvInput;
    tlvInput << ToTlv(inputType, vInputHash);
    data << ToTlv(0x46, tlvInput);

    JUB_BYTE ins = 0xca;

    //one pack can do it
    APDU apdu(0x00, ins, 0x01, 0x00, (JUB_ULONG)data.size(), data.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    signatureRaw.clear();
    signatureRaw.insert(signatureRaw.end(), retData, retData + ulRetDataLen);

    return JUBR_OK;
}
} // namespace jub end
