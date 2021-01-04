#include "token/JubiterBLDImpl.h"

namespace jub {

stAppInfos JubiterBLDImpl::g_appInfo[] = {
    {
        {0xD1, 0x56, 0x00, 0x01, 0x32, 0x83, 0x00, 0x42, 0x4C, 0x44, 0x00, 0x00, 0x42, 0x54, 0x43, 0x01},
        "BTC",
        "0000000"
    },
    {
        {0xD1, 0x56, 0x00, 0x01, 0x32, 0x83, 0x00, 0x42, 0x4C, 0x44, 0x00, 0x00, 0x45, 0x54, 0x48, 0x01},
        "ETH",
        "0000000"
    },
    // BTC and ETH index position fixed, start adding new apps below:
    {
        {0xD1, 0x56, 0x00, 0x01, 0x32, 0x83, 0x00, 0x42, 0x4C, 0x44, 0x00, 0x00, 0x45, 0x54, 0x48, 0x01},
        "ETC",
        "01010000"
    },
    {
        {0xD1, 0x56, 0x00, 0x01, 0x32, 0x83, 0x00, 0x42, 0x4C, 0x44, 0x00, 0x00, 0x42, 0x54, 0x43, 0x01},
        "BCH",
        "01070003"
    },
    {
        {0xD1, 0x56, 0x00, 0x01, 0x32, 0x83, 0x00, 0x42, 0x4C, 0x44, 0x00, 0x00, 0x42, 0x54, 0x43, 0x01},
        "LTC",
        "01070003",
    },
    {
        {0xD1, 0x56, 0x00, 0x01, 0x32, 0x83, 0x00, 0x42, 0x4C, 0x44, 0x00, 0x00, 0x42, 0x54, 0x43, 0x01},
        "USDT",
        "01080002"
    },
    {
        {0x63, 0x6F, 0x6D, 0x2E, 0x66, 0x74, 0x2E, 0x68, 0x63, 0x72, 0x61, 0x73, 0x68, 0x01},
        "HC",
        "00000000"
    },
    {
        {0xD1, 0x56, 0x00, 0x01, 0x32, 0x83, 0x00, 0x42, 0x4C, 0x44, 0x00, 0x00, 0x42, 0x54, 0x43, 0x01},
        "QTUM",
        "01090204"
    },
    // EOS-independent applet JUBR_PKIAID_INVALID
    {
        {0xD1, 0x56, 0x00, 0x01, 0x32, 0x03, 0x00, 0x42, 0x4C, 0x44, 0x00, 0x00, 0x45, 0x54, 0x49, 0x01},
        "EOS",
        "01000009"
    },
    // MISC applet, start adding new apps below:
    {
        {0xD1, 0x56, 0x00, 0x01, 0x32, 0x03, 0x00, 0x42, 0x4C, 0x44, 0x00, 0x6D, 0x69, 0x73, 0x63, 0x01},
        "EOS",
        "01000001"
    },
    {
        {0xD1, 0x56, 0x00, 0x01, 0x32, 0x03, 0x00, 0x42, 0x4C, 0x44, 0x00, 0x6D, 0x69, 0x73, 0x63, 0x01},
        "XRP",
        "01000001"
    },
};

JubiterBLDImpl::JubiterBLDImpl(std::string path)
    :_apduBuiler(std::make_shared<JubApudBuiler>()),
     _device(std::make_shared<DeviceType>()),
     _path(path) {

};

JubiterBLDImpl::JubiterBLDImpl(DeviceType* device)
    :_apduBuiler(std::make_shared<JubApudBuiler>()),
     _device(device) {

}

JubiterBLDImpl::~JubiterBLDImpl() {};

JUB_RV JubiterBLDImpl::ConnectToken() {
    return _device->Connect(_path);
}

JUB_RV JubiterBLDImpl::DisconnectToken() {
    return _device->Disconnect();
}

JUB_RV JubiterBLDImpl::EnumApplet(std::string& appletList) {

    {
        // select main safe scope
        APDU apdu(0x00, 0xa4, 0x04, 0x00, 0x00);
        JUB_UINT16 ret = 0;
        JUB_VERIFY_RV(_SendApdu(&apdu, ret));
        if (0x9000 != ret) {
            JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
        }
    }

    // send apdu, then decide which coin types supports.
    APDU apdu(0x80, 0xCB, 0x80, 0x00, 0x05,
        (const JUB_BYTE *)"\xDF\xFF\x02\x81\x06");
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[1024] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
    }

    abcd::DataChunk tlvData(retData, retData + ulRetDataLen);
    auto appList = ParseTlv(tlvData);

    for (auto appID : appList) {
        uchar_vector id(appID);
        appletList += id.getHex();
        appletList += " ";
    }

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::EnumSupportCoins(std::string& coinList) {

    JUB_RV rv = JUBR_ERROR;

    std::string appletList;
    JUB_VERIFY_RV(EnumApplet(appletList));

    std::vector<std::string> coinNameList;
    auto vAppList = Split(appletList, " ");
    for (auto appID : vAppList) {
        std::string version;
        rv = GetAppletVersion(appID, version);
        if (JUBR_OK != rv) {
            continue;
        }
        for (auto appInfo : JubiterBLDImpl::g_appInfo) {
            uchar_vector _appID(appInfo.appID);
            if (_appID.getHex() == appID) {
                if (appInfo.minimumAppletVersion <= version) {
                    if (coinNameList.end() == std::find(coinNameList.begin(), coinNameList.end(), appInfo.coinName)) {
                        coinList += appInfo.coinName;
                        coinList += " ";
                        coinNameList.insert(coinNameList.end(), appInfo.coinName);
                    }
                }
            }
        }
    }
    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::GetAppletVersion(const std::string& appID, std::string& version) {

    uchar_vector id(appID);
    if (0 == appID.length()) {
        JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
    }

    JUB_UINT16 ret = 0;
    uchar_vector FidoID(kPKIAID_FIDO, 8);
    if (id == FidoID) {
        //select
        APDU apdu(0x00, 0xA4, 0x04, 0x00, (JUB_ULONG)id.size(), &id[0]);
        JUB_BYTE retData[1024] = {0,};
        JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
        JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
        if (0x9000 != ret) {
            JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
        }

        //get version

        uchar_vector apduData("DFFF028001");
        APDU apduVersion(0x80, 0xE2, 0x80, 0x00, (JUB_ULONG)apduData.size(), &apduData[0], 0x00);
        JUB_BYTE retDataVersion[1024] = {0,};
        JUB_ULONG ulRetVersionLen = sizeof(retDataVersion)/sizeof(JUB_BYTE);
        JUB_VERIFY_RV(_SendApdu(&apduVersion, ret, retDataVersion, &ulRetVersionLen));
        if (0x9000 != ret) {
            JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
        }

        uchar_vector vVersion(&retDataVersion[6], 4);
        version = vVersion.getHex();
        return JUBR_OK;
    }
    else {
        APDU apdu(0x00, 0xA4, 0x04, 0x00, (JUB_ULONG)id.size(), &id[0]);
        JUB_BYTE retData[1024] = {0,};
        JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
        JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
        if (0x9000 != ret) {
            JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
        }

        if (   0x84 == retData[2]
            && 0x04 == retData[3]
            ) {
            uchar_vector vVersion(&retData[4], 4);
            version = vVersion.getHex();
            return JUBR_OK;
        }
    }

    JUB_VERIFY_RV(JUBR_ERROR);

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::GetDeviceCert(std::string& cert) {

    uchar_vector apduData("A60483021518");
    APDU apdu(0x80, 0xca, 0xbf, 0x21, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[1024] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 == ret) {
        uchar_vector vcert(retData + 4, retData + ulRetDataLen);
        cert = vcert.getHex();
        return JUBR_OK;
    }

    JUB_VERIFY_RV(JUBR_ERROR);

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::SendOneApdu(const std::string& apdu, std::string& response) {

    uchar_vector sendApdu(apdu);
    JUB_BYTE retData[FT3KHN_READWRITE_SIZE_ONCE_NEW + 6] = {0,};
    JUB_ULONG ulRetDataLen = FT3KHN_READWRITE_SIZE_ONCE_NEW + 6;
    JUB_RV rv = _device->SendData(sendApdu.data(), (JUB_ULONG)sendApdu.size(), retData, &ulRetDataLen);
    if (JUBR_OK != rv) {
        JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
    }

    uchar_vector vResponse(retData, retData + ulRetDataLen);
    response = vResponse.getHex();

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::QueryBattery(JUB_BYTE &percent) {

    JUB_CHECK_NULL(_device);

    APDU apdu(0x00, 0xD6, 0xFE, 0xED, 0x00, NULL, 0x01);
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[1024] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
    }

    percent = retData[0];

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::ShowVirtualPwd() {

    JUB_CHECK_NULL(_device);
    //SWITCH_TO_BTC_APP

    APDU apdu(0x00, 0x29, 0x00, 0x00, 0x00);
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[1024] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
    }

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::CancelVirtualPwd() {

    JUB_CHECK_NULL(_device);
    //SWITCH_TO_BTC_APP

    APDU apdu(0x00, 0x29, 0x80, 0x00, 0x00);
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[1024] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
    }

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::VerifyPIN(const std::string &pinMix, OUT JUB_ULONG &retry) {

    JUB_CHECK_NULL(_device);

    // select app first
    //SWITCH_TO_BTC_APP

    abcd::DataChunk pinCoord;
    //auto pinData = buildData({ pin });

    std::transform(pinMix.begin(),
                   pinMix.end(),
                   std::back_inserter(pinCoord),
                   [](const char elem) {
                       return (uint8_t)(elem - 0x30);
                   });

    APDU apdu(0x00, 0x20, 0x02, 0x00, (JUB_ULONG)pinCoord.size(), pinCoord.data());
    //APDU apdu(0x00, 0x10, 0x00, 0x00, pinCoord.size(), pinCoord.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[1024] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x6985 == ret) { //locked
        JUB_VERIFY_RV(JUBR_PIN_LOCKED);
    }
    if (0x63C0 == (ret & 0xfff0)) {
        retry = (ret & 0xf);
        JUB_VERIFY_RV(JUBR_DEVICE_PIN_ERROR);
    }
    else if (0x9000 != ret) {
        JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
    }

    return JUBR_OK;
}

bool JubiterBLDImpl::IsInitialize() {

    uchar_vector apduData("DFFF028105");
    APDU apdu(0x80, 0xcb, 0x80, 0x00, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[1024] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    auto rv = _SendApdu(&apdu, ret, retData, &ulRetDataLen);
    if (JUBR_OK == rv) {
        if (0x5a == retData[0]) {
            return true;
        }
    }

    return false;
}

bool JubiterBLDImpl::IsBootLoader() {

    APDU apdu(0x00, 0xa4, 0x04, 0x00, 0x00);
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[1024] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    auto rv = _SendApdu(&apdu, ret, retData, &ulRetDataLen);
    if (  JUBR_OK == rv
        && 0x6e00 == ret
        ) {
        return true;
    }

    return false;
}

JUB_RV JubiterBLDImpl::GetBleVersion(JUB_BYTE bleVersion[4]) {

    uchar_vector apduData("DFFF028100");
    APDU apdu(0x80, 0xcb, 0x80, 0x00, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[1024] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 == ret) {
        uchar_vector version(retData, retData + ulRetDataLen);
        memset(bleVersion, 0x00, 4);
        memcpy(bleVersion, version.getHex().c_str(), 4);

        return JUBR_OK;
    }

    JUB_VERIFY_RV(JUBR_ERROR);

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::GetFwVersion(JUB_BYTE fwVersion[4]) {

    uchar_vector apduData("DFFF028003");
    APDU apdu(0x80, 0xcb, 0x80, 0x00, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[1024] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 == ret) {
        uchar_vector version(retData, retData + ulRetDataLen);
        memset(fwVersion, 0x00, 4);
        memcpy(fwVersion, version.getHex().c_str(), 4);

        return JUBR_OK;
    }

    JUB_VERIFY_RV(JUBR_ERROR);

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::GetSN(JUB_BYTE sn[24]) {

    uchar_vector apduData("DFFF028101");
    APDU apdu(0x80, 0xcb, 0x80, 0x00, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[1024] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 == ret) {
        memset(sn, 0x00, 24);
        memcpy(sn, retData, 24);

        return JUBR_OK;
    }

    JUB_VERIFY_RV(JUBR_ERROR);

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::GetLabel(JUB_BYTE label[32]) {

    uchar_vector apduData("DFFF028104");
    APDU apdu(0x80, 0xcb, 0x80, 0x00, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[1024] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 == ret) {
        memset(label, 0x00, 32);
        memcpy(label, retData, 32);

        return JUBR_OK;
    }

    JUB_VERIFY_RV(JUBR_ERROR);

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::GetPinRetry(JUB_BYTE& retry) {

    uchar_vector apduData("DFFF028102");
    APDU apdu(0x80, 0xcb, 0x80, 0x00, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[1024] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 == ret) {
        retry = retData[0];

        return JUBR_OK;
    }

    JUB_VERIFY_RV(JUBR_ERROR);

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::GetPinMaxRetry(JUB_BYTE& maxRetry) {

    uchar_vector apduData("DFFF028103");
    APDU apdu(0x80, 0xcb, 0x80, 0x00, (JUB_ULONG)apduData.size(), apduData.data());
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[1024] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 == ret) {
        maxRetry = retData[0];

        return JUBR_OK;
    }

    JUB_VERIFY_RV(JUBR_ERROR);

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::_SelectApp(const JUB_BYTE PKIAID[], JUB_BYTE length) {

    APDU apdu(0x00, 0xA4, 0x04, 0x00, length, PKIAID);
    JUB_UINT16 ret = 0;
    JUB_BYTE retData[1024] = {0,};
    JUB_ULONG ulRetDataLen = sizeof(retData)/sizeof(JUB_BYTE);
    JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, &ulRetDataLen));
    if (0x9000 != ret) {
        JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
    }

    uchar_vector vVersion(&retData[4], retData[3]);
    _appletVersion = vVersion.getHex();

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::_SetCoinType(const JUB_BYTE& type) {

    APDU apdu(0x00, 0xf5, type, 0x00, 0x00);
    JUB_UINT16 ret = 0;
    JUB_VERIFY_RV(_SendApdu(&apdu, ret));
    if (   0x9000 == ret
        || 0x6d00 == ret
        ) {
        return JUBR_OK;
    }

    return JUBR_ERROR;
}

JUB_RV JubiterBLDImpl::_TranPack(const abcd::DataSlice &apduData, const JUB_BYTE highMark, const JUB_BYTE sigType, const JUB_ULONG ulSendOnceLen, int finalData/* = false*/, int bOnce/* = false*/) {

    if (apduData.empty()) {
        JUB_VERIFY_RV(JUBR_ERROR);
    }

    JUB_UINT16 ret = 0;
    if (bOnce) {
        // one pack enough
        JUB_BYTE p1 = 0x00;
        p1 |= highMark;
        APDU apdu(0x00, 0xF8, p1, sigType, (JUB_ULONG)apduData.size(), apduData.data());
        JUB_VERIFY_RV(_SendApdu(&apdu, ret));
        if (0x9000 != ret) {
            JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
        }

        return JUBR_OK;
    }

    // else send pack by pack
    auto nextTimes = apduData.size() / ulSendOnceLen;
    auto left = apduData.size() % ulSendOnceLen;

    // split last pack
    if (   0 == left
        && 0 != nextTimes
        ) {
        nextTimes--;
        left = ulSendOnceLen;
    }

    // pack by pack
    JUB_BYTE p1 = 0x02;
    p1 |= highMark;
    APDU apdu(0x00, 0xF8, p1, sigType, 0x00);
    apdu.lc = ulSendOnceLen;
    JUB_UINT32 times = 0;
    for (times = 0; times < nextTimes; times++) {
        apdu.SetData(apduData.data() + times * ulSendOnceLen, apdu.lc);
        JUB_VERIFY_RV(_SendApdu(&apdu, ret));
        if (0x9000 != ret) {
            JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
        }
    }

    // next pack
    apdu.lc = (JUB_ULONG)left;
    if (apdu.lc) {
        if (finalData) {
            apdu.p1 = 0x03;
            apdu.p1 |= highMark;
        }

        apdu.SetData(apduData.data() + times * ulSendOnceLen, apdu.lc);
        JUB_VERIFY_RV(_SendApdu(&apdu, ret));
        if (0x9000 != ret) {
            JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
        }
    }

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::_TranPackApdu(const JUB_ULONG ncla, const JUB_ULONG nins,
                                     const abcd::DataSlice &apduData,
                                     const JUB_BYTE highMark,
                                     const JUB_BYTE sigType,
                                     const JUB_ULONG ulSendOnceLen,
                                     JUB_BYTE *retData/* = nullptr*/, JUB_ULONG *pulRetDataLen/* = nullptr*/,
                                     int finalData/* = false*/, int bOnce/* = false*/) {

    if (apduData.empty()) {
        JUB_VERIFY_RV(JUBR_ERROR);
    }

    JUB_UINT16 ret = 0;
    if (bOnce) {
        // one pack enough
        JUB_BYTE p1 = 0x00;
        p1 |= highMark;
        APDU apdu(ncla, nins, p1, sigType, (JUB_ULONG)apduData.size(), apduData.data());
        JUB_VERIFY_RV(_SendApdu(&apdu, ret));
        if (0x9000 != ret) {
            JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
        }

        return JUBR_OK;
    }

    // else send pack by pack
    auto nextTimes = apduData.size() / ulSendOnceLen;
    auto left = apduData.size() % ulSendOnceLen;

    // split last pack
    if (   0 == left
        && 0 != nextTimes
        ) {
        nextTimes--;
        left = ulSendOnceLen;
    }

    // pack by pack
    JUB_BYTE p1 = 0x02;
    p1 |= highMark;
    APDU apdu(ncla, nins, p1, sigType, 0x00);
    apdu.lc = ulSendOnceLen;
    JUB_UINT32 times = 0;
    for (times = 0; times < nextTimes; times++) {
        apdu.SetData(apduData.data() + times * ulSendOnceLen, apdu.lc);
        JUB_VERIFY_RV(_SendApdu(&apdu, ret));
        if (0x9000 != ret) {
            JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
        }
    }

    // next pack
    apdu.lc = (JUB_ULONG)left;
    if (apdu.lc) {
        if (finalData) {
            apdu.p1 = 0x03;
            apdu.p1 |= highMark;
        }

        apdu.SetData(apduData.data() + times * ulSendOnceLen, apdu.lc);
        JUB_VERIFY_RV(_SendApdu(&apdu, ret, retData, pulRetDataLen));
        if (0x9000 != ret) {
            JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
        }
    }

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::_SendApdu(const APDU *apdu, JUB_UINT16 &wRet, JUB_BYTE *retData /*= nullptr*/,
    JUB_ULONG *pulRetDataLen /*= nullptr*/,
    JUB_ULONG ulMiliSecondTimeout /*= 0*/) {

    JUB_CHECK_NULL(_apduBuiler);
    JUB_CHECK_NULL(_device);

    JUB_BYTE _retData[FT3KHN_READWRITE_SIZE_ONCE_NEW + 6] = {0,};
    JUB_ULONG ulRetDataLen = FT3KHN_READWRITE_SIZE_ONCE_NEW + 6;

    std::vector<JUB_BYTE> vSendApdu;
    if (JUBR_OK == _apduBuiler->BuildApdu(apdu, vSendApdu)) {
        if (JUBR_OK != _device->SendData(vSendApdu.data(), (JUB_ULONG)vSendApdu.size(), _retData, &ulRetDataLen, ulMiliSecondTimeout)) {
            JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);
        }

        if (NULL == pulRetDataLen) {
            wRet = _retData[ulRetDataLen - 2] * 0x100 + _retData[ulRetDataLen - 1];
            return JUBR_OK;
        }

        if (NULL == retData) {
            *pulRetDataLen = ulRetDataLen - 2;
            wRet = (_retData[ulRetDataLen - 2] * 0x100 + _retData[ulRetDataLen - 1]);
            return JUBR_OK;
        }

        if (*pulRetDataLen < (ulRetDataLen - 2)) {
            *pulRetDataLen = ulRetDataLen - 2;
            JUB_VERIFY_RV(JUBR_BUFFER_TOO_SMALL);
        }

        *pulRetDataLen = ulRetDataLen - 2;
        memcpy(retData, _retData, ulRetDataLen - 2);

        wRet = _retData[ulRetDataLen - 2] * 0x100 + _retData[ulRetDataLen - 1];
        return JUBR_OK;
    }

    JUB_VERIFY_RV(JUBR_TRANSMIT_DEVICE_ERROR);

    return JUBR_OK;
}

JUB_RV JubiterBLDImpl::SetTimeout(const JUB_UINT16 timeout) {

    JUB_UINT16 p1 = timeout >> 8;
    JUB_UINT16 p2 = timeout & 0xFF;
    APDU apdu(0x00, 0xfb, p1, p2, 0x00);
    JUB_UINT16 ret = 0;
    JUB_VERIFY_RV(_SendApdu(&apdu, ret));
    if (0x9000 == ret) {
        return JUBR_OK;
    }

    JUB_VERIFY_RV(JUBR_ERROR);

    return JUBR_OK;
}

}  // namespace jub end
