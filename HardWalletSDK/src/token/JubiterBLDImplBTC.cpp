#include "token/JubiterBLDImpl.h"

namespace jub {

constexpr JUB_BYTE kMainnetP2PKH = 0x00;
//constexpr JUB_BYTE kMainnetP2SH = 0x01;
//constexpr JUB_BYTE kMainnetP2WPKH = 0x02;
//constexpr JUB_BYTE kMainnetP2WSH = 0x03;
constexpr JUB_BYTE kMainnetP2SH_P2WPKH = 0x04;
//constexpr JUB_BYTE kMainnetP2SH_P2WSH = 0x05;

constexpr JUB_BYTE kMainnetP2SH_Multisig = 0x11;

constexpr JUB_BYTE PKIAID_BTC[16] = {
    0xD1, 0x56, 0x00, 0x01, 0x32, 0x83, 0x00, 0x42, 0x4C, 0x44, 0x00, 0x00, 0x42, 0x54, 0x43, 0x01
};

#define APPLET_BTC_SUPPORT_LEGACY_ADDRESS_VERSION "01090205"

bool JubiterBLDImpl::_isSupportLegacyAddress() {
    if (0 <= _appletVersion.compare(APPLET_BTC_SUPPORT_LEGACY_ADDRESS_VERSION)) {
        return true;
    }

    return false;
};

JUB_BYTE JubiterBLDImpl::_RealAddressFormat(const JUB_ULONG& addrFmt) {
    return _HighMark((JUB_ULONG)addrFmt);
}

JUB_BYTE JubiterBLDImpl::_RealAddressFormat(const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt) {
    return _RealAddressFormat((JUB_ULONG)addrFmt);
}

#define SWITCH_TO_BTC_APP                       \
do {				                            \
    JUB_VERIFY_RV(_SelectApp(PKIAID_BTC, 16));  \
} while (0)                                     \

JUB_RV JubiterBLDImpl::SelectAppletBTC() {
    SWITCH_TO_BTC_APP;
    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::GetHDNodeBTC(const JUB_BTC_TRANS_TYPE& type, const std::string& path, std::string& xpub) {

    uchar_vector vPath;
    vPath << path;
    uchar_vector apduData = ToTlv(0x08, vPath);
    JUB_BYTE p2 = 0x00;
    switch (type) {
    case p2pkh:
    case p2sh_multisig:
    case p2wsh_multisig:
    {
        p2 = 0x00;   //xpub  format
        break;
    } // case p2pkh/p2sh_multisig end
    case p2sh_p2wpkh:
    {
        p2 = 0x01;   //ypub  format
        break;
    } // case p2sh_p2wpkh end
    default:
        break;
    } // switch (type) end

    APDU apdu(0x00, 0xe6, 0x00, p2, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    xpub = (JUB_CHAR_PTR)retData;

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::GetAddressBTC(const JUB_BYTE addrFmt,
                                     const JUB_BTC_TRANS_TYPE& type,
                                     const std::string& path,
                                     const JUB_UINT16 tag,
                                     std::string& address) {

    JUB_BYTE p1 = (JUB_BYTE)tag;
    if (_isSupportLegacyAddress()) {
        p1 |= _RealAddressFormat(addrFmt);
    }
    JUB_BYTE sigType;
    switch (type) {
    case p2pkh:
    {
        sigType = kMainnetP2PKH;
        break;
    } // case p2pkh end
    case p2sh_p2wpkh:
    {
        sigType = kMainnetP2SH_P2WPKH;
        break;
    } // case p2sh_p2wpkh end
    default:
        return JUBR_IMPL_NOT_SUPPORT;
    } // switch (type) end

    uchar_vector vPath;
    vPath << path;
    uchar_vector apduData = ToTlv(0x08, vPath);
    APDU apdu(0x00, 0xf6, p1, sigType, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    address = (JUB_CHAR_PTR)retData;

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::GetAddressMultisigBTC(const JUB_BYTE addrFmt,
                                             const JUB_BTC_TRANS_TYPE& type,
                                             const std::string& path,
                                             const JUB_UINT16 tag,
                                             const uchar_vector& vRedeemScriptTlv,
                                             std::string& address) {

    constexpr JUB_UINT32 kSendOnceLen = 230;

    JUB_BYTE p1 = 0x01;
    if (_isSupportLegacyAddress()) {
        p1 |= _RealAddressFormat(addrFmt);
    }

    JUB_BYTE sigType;
    switch (type) {
    case p2sh_multisig:
    case p2wsh_multisig:
    {
        sigType = kMainnetP2SH_Multisig;
        break;
    } // case p2sh_multisig end
    default:
        return JUBR_IMPL_NOT_SUPPORT;
    } // switch (type) end

    uchar_vector apduData;
    //path LV
    uchar_vector pathLV;
    pathLV << (JUB_BYTE)(path.size());
    pathLV << path;

    apduData << (JUB_BYTE)tag;
    apduData << ToTlv(0x0F, pathLV);

    //  first pack
    APDU apdu(0x00, 0xF7, p1, sigType, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_VERIFY_RV(_SendApdu(&apdu, ret));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }
    apduData.clear();

    //redeemScript TLV
    uchar_vector redeemScriptTlv;
    redeemScriptTlv << Tolv(vRedeemScriptTlv);

    apduData << ToTlv(0x1F, redeemScriptTlv);

//    apdu.SetApdu(0x00, 0xF7, 0x03, sigType, (JUB_ULONG)apduData.size(), apduData.data(), 0x00);
    JUB_BYTE retData[2048] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);

    JUB_BYTE highMark = 0x00;
    if (_isSupportLegacyAddress()) {
        highMark |= _RealAddressFormat(addrFmt);
    }
    JUB_VERIFY_RV(_TranPackApdu(0x00, 0xF7,
                                apduData,
                                highMark,
                                sigType,
                                kSendOnceLen,
                                retData, &ulRetDataLen,
                                true)); // last data.
//    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
//    if (0x9000 != ret) {
//        return JUBR_TRANSMIT_DEVICE_ERROR;
//    }

    address = (JUB_CHAR_PTR)retData;

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::SignTXBTC(const JUB_BYTE addrFmt,
                                 const JUB_BTC_TRANS_TYPE& type,
                                 const JUB_UINT16 inputCount,
                                 const std::vector<JUB_UINT64>& vInputAmount,
                                 const std::vector<std::string>& vInputPath,
                                 const std::vector<JUB_UINT16>& vChangeIndex,
                                 const std::vector<std::string>& vChangePath,
                                 const std::vector<JUB_BYTE>& vUnsigedTrans,
                                 std::vector<JUB_BYTE>& vRaw) {
    //SWITCH_TO_BTC_APP

    constexpr JUB_UINT32 kSendOnceLen = 230;

    JUB_BYTE p1 = 0x01;
    if (_isSupportLegacyAddress()) {
        p1 |= _RealAddressFormat(addrFmt);
    }

    JUB_BYTE sigType;
    switch (type) {
    case p2pkh:
    {
        sigType = kMainnetP2PKH;
        break;
    } // case p2pkh end
    case p2sh_p2wpkh:
    {
        sigType = kMainnetP2SH_P2WPKH;
        break;
    } // case p2sh_p2wpkh end
    default:
        return JUBR_IMPL_NOT_SUPPORT;
    } // switch (type) end

    // number of input
    uchar_vector apduData;
    apduData << (JUB_BYTE)(inputCount);
    // ammountTLV
    uchar_vector amountTLV;
    for (auto amount : vInputAmount) {
        amountTLV << (uint64_t)amount;
    }

    apduData << ToTlv(0x0e, amountTLV);

    //  first pack
    APDU apdu(0x00, 0xF8, p1, sigType, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_VERIFY_RV(_SendApdu(&apdu, ret));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }
    apduData.clear();

    // pathTLV
    uchar_vector pathLV;
    for (auto path : vInputPath) {
        pathLV << (JUB_BYTE)(path.size());
        pathLV << path;
    }

    apduData << ToTlv(0x0F, pathLV);

    JUB_BYTE highMark = 0x00;
    if (_isSupportLegacyAddress()) {
        highMark |= _RealAddressFormat(addrFmt);
    }
    JUB_VERIFY_RV(_TranPack(apduData, highMark, sigType, kSendOnceLen));
    apduData.clear();

    //tx TLV
    apduData << ToTlv(0x0D, vUnsigedTrans);

    JUB_VERIFY_RV(_TranPack(apduData, highMark, sigType, kSendOnceLen));
    apduData.clear();

    //change TLV
    uchar_vector changeLV;
    changeLV << (JUB_BYTE)vChangeIndex.size();
    for (size_t i = 0; i < vChangeIndex.size(); i++) {
        changeLV << (JUB_BYTE)vChangeIndex[i];
        changeLV << (JUB_BYTE)vChangePath[i].length();
        changeLV << vChangePath[i];
    }

    uchar_vector changeIndexTLV;
    changeIndexTLV = ToTlv(0x10, changeLV);
    apduData << changeIndexTLV;

    JUB_VERIFY_RV(_TranPack(apduData, highMark, sigType, kSendOnceLen, true)); // last data.
    apduData.clear();

    //  sign transactions
    JUB_BYTE retData[2] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    apdu.SetApdu(0x00, 0x2A, 0x00, 0x00, 0);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x6f09 == ret) {
        return JUBR_USER_CANCEL;
    }
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    // get transactions (pack by pack)
    if (2 != ulRetDataLen) { // total length
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    JUB_UINT16 totalReadLen = ReadBE16(retData);
    abcd::DataChunk sigRawTx(totalReadLen, 0x00);

    constexpr JUB_UINT16 kReadOnceLen = 256;
    apdu.le = kReadOnceLen;
    JUB_ULONG ulRetLen = kReadOnceLen;

    apdu.SetApdu(0x00, 0xF9, 0x00, 0x00, 0x00);
    JUB_UINT16 times = 0;
    for (times = 0; times < (totalReadLen / kReadOnceLen); times++) {

        JUB_UINT16 offset = times * kReadOnceLen;
        apdu.p1 = offset >> 8;
        apdu.p2 = offset & 0x00ff;

        JUB_VERIFY_RV(_SendApdu(&apdu, ret, sigRawTx.data() + times * kReadOnceLen, &ulRetLen));
        if (0x9000 != ret) {
            return JUBR_TRANSMIT_DEVICE_ERROR;
        }
    }

    apdu.le = totalReadLen % kReadOnceLen;
    if (apdu.le) {
        JUB_UINT16 offset = times * kReadOnceLen;
        apdu.p1 = offset >> 8;
        apdu.p2 = offset & 0x00ff;

        ulRetLen = totalReadLen - times * kReadOnceLen;

        JUB_VERIFY_RV(_SendApdu(&apdu, ret, sigRawTx.data() + times * kReadOnceLen, &ulRetLen));
        if (0x9000 != ret) {
            return JUBR_TRANSMIT_DEVICE_ERROR;
        }
    }

    vRaw.clear();
    vRaw = sigRawTx;

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::SignTXMultisigBTC(const JUB_BYTE addrFmt,
                                         const JUB_BTC_TRANS_TYPE& type,
                                         const JUB_UINT16 inputCount,
                                         const std::vector<JUB_UINT64>& vInputAmount,
                                         const std::vector<std::string>& vInputPath,
                                         const std::vector<uchar_vector>& vRedeemScriptTlv,
                                         const std::vector<JUB_UINT16>& vChangeIndex,
                                         const std::vector<std::string>& vChangePath,
                                         const std::vector<uchar_vector>& vChangeRedeemScriptTlv,
                                         const std::vector<JUB_BYTE>& vUnsigedTrans,
                                         std::vector<uchar_vector>& vSignatureRaw) {
    //SWITCH_TO_BTC_APP

    constexpr JUB_UINT32 kSendOnceLen = 230;

    JUB_BYTE p1 = 0x01;
    if (_isSupportLegacyAddress()) {
        p1 |= _RealAddressFormat(addrFmt);
    }

    JUB_BYTE sigType;
    switch (type) {
    case p2sh_multisig:
    case p2wsh_multisig:
    {
        sigType = kMainnetP2SH_Multisig;
        break;
    } // case p2sh_multisig end
    default:
        return JUBR_IMPL_NOT_SUPPORT;
    } // switch (type) end

    // number of input
    uchar_vector apduData;
    apduData << (JUB_BYTE)(inputCount);
    // amountTLV
    uchar_vector amountTLV;
    for (auto amount : vInputAmount) {
        amountTLV << (uint64_t)amount;
    }

    apduData << ToTlv(0x0e, amountTLV);

    //  first pack
    APDU apdu(0x00, 0xF8, p1, sigType, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_VERIFY_RV(_SendApdu(&apdu, ret));
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }
    apduData.clear();

    // pathTLV
    uchar_vector pathLV;
    for (auto path : vInputPath) {
        pathLV << (JUB_BYTE)(path.size());
        pathLV << path;
    }

    apduData << ToTlv(0x0F, pathLV);

    JUB_BYTE highMark = 0x00;
    if (_isSupportLegacyAddress()) {
        highMark |= _RealAddressFormat(addrFmt);
    }
    JUB_VERIFY_RV(_TranPack(apduData, highMark, sigType, kSendOnceLen));
    apduData.clear();

    //tx TLV
    apduData << ToTlv(0x0D, vUnsigedTrans);

    JUB_VERIFY_RV(_TranPack(apduData, highMark, sigType, kSendOnceLen));
    apduData.clear();

    //change TLV
    uchar_vector changeLV;
    changeLV << (JUB_BYTE)vChangeIndex.size();
    for (size_t i = 0; i < vChangeIndex.size(); i++) {
        changeLV << (JUB_BYTE)vChangeIndex[i];
        changeLV << (JUB_BYTE)vChangePath[i].length();
        changeLV << vChangePath[i];

        //redeemScriptTlv for change
        changeLV << Tolv(vChangeRedeemScriptTlv[i]);
    }

    uchar_vector changeIndexTLV;
    changeIndexTLV = ToTlv(0x10, changeLV);
    apduData << changeIndexTLV;

    JUB_VERIFY_RV(_TranPack(apduData, highMark, sigType, kSendOnceLen));
    apduData.clear();

    //redeemScript TLV
    uchar_vector redeemScriptTlv;
    for (int i=0; i<vRedeemScriptTlv.size(); ++i) {
        //redeemScriptTlv
        redeemScriptTlv << Tolv(vRedeemScriptTlv[i]);
    }
    apduData << ToTlv(0x1F, redeemScriptTlv);

    JUB_VERIFY_RV(_TranPack(apduData, highMark, sigType, kSendOnceLen, true)); // last data.
    apduData.clear();

    //  sign transactions
    JUB_BYTE retData[2] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    apdu.SetApdu(0x00, 0x2A, 0x00, 0x00, 0);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x6f09 == ret) {
        return JUBR_USER_CANCEL;
    }
    if (0x9000 != ret) {
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    // get transactions (pack by pack)
    if (2 != ulRetDataLen) { // total length
        return JUBR_TRANSMIT_DEVICE_ERROR;
    }

    JUB_UINT16 totalReadLen = ReadBE16(retData);
    abcd::DataChunk signedRaw(totalReadLen, 0x00);

    constexpr JUB_UINT16 kReadOnceLen = 256;
    apdu.le = kReadOnceLen;
    JUB_ULONG ulRetLen = kReadOnceLen;

    apdu.SetApdu(0x00, 0xE9, 0x00, 0x00, 0x00);
    JUB_UINT16 times = 0;
    for (times = 0; times < (totalReadLen / kReadOnceLen); times++) {

        JUB_UINT16 offset = times * kReadOnceLen;
        apdu.p1 = offset >> 8;
        apdu.p2 = offset & 0x00ff;

        JUB_VERIFY_RV(_SendApdu(&apdu, ret, signedRaw.data() + times * kReadOnceLen, &ulRetLen));
        if (0x9000 != ret) {
            return JUBR_TRANSMIT_DEVICE_ERROR;
        }
    }

    apdu.le = totalReadLen % kReadOnceLen;
    if (apdu.le) {
        JUB_UINT16 offset = times * kReadOnceLen;
        apdu.p1 = offset >> 8;
        apdu.p2 = offset & 0x00ff;

        ulRetLen = totalReadLen - times * kReadOnceLen;

        JUB_VERIFY_RV(_SendApdu(&apdu, ret, signedRaw.data() + times * kReadOnceLen, &ulRetLen));
        if (0x9000 != ret) {
            return JUBR_TRANSMIT_DEVICE_ERROR;
        }
    }

    //vSignatureRaw
    uint8_t len = 0;
    uint8_t offset = 0;
    uint8_t offsetTotal = 0;
    for (JUB_UINT16 i=0; i<inputCount; ++i) {
        uint8_t* b = signedRaw.data() + offsetTotal;
        len = *b;

        offset = len + 1;
        uchar_vector signatureRaw(b + 1, b + offset);
        offsetTotal += offset;

        vSignatureRaw.push_back(signatureRaw);
    }

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::SetUnitBTC(const JUB_BTC_UNIT_TYPE& unit) {

    APDU apdu(0x00, 0xfa, JUB_BYTE(unit), 0x00, 0x00);
    JUB_UINT16 ret = 0;
    JUB_VERIFY_RV(_SendApdu(&apdu, ret));
    if (0x9000 == ret) {
        return JUBR_OK;
    }

    return JUBR_ERROR;
}

JUB_RV JubiterBLDImpl::SetCoinTypeBTC(const JUB_ENUM_COINTYPE_BTC& type) {

    return _SetCoinType((JUB_BYTE)type);
}

} // namespace jub end
