#include "device/DeviceIOLogHelper.hpp"

#ifdef BLE_MODE
#include <unistd.h>
#include <sys/time.h>

#include "utility/util.h"
#include "utility/Singleton.h"
#include "bleTransmit/bleTransmit.h"
#include "device/JubiterBLEDevice.hpp"
#include "device/Fido.h"

#ifdef __ANDROID__
#include "bleTransmit/android/BTManager.h"
#else
#include "ErrorCodesAndMacros.h"
#include "BLEInterface.h"
#endif

namespace jub {

JubiterBLEDevice::JubiterBLEDevice():
    _param{nullptr, BLE_ReadCallBack, BLE_ScanCallBack, BLE_DiscCallBack},
    _handle(0),
    _bConnected(false) {

}

JubiterBLEDevice::~JubiterBLEDevice() {
    Disconnect();
}

JUB_RV JubiterBLEDevice::MatchErrorCode(int error) {

#ifdef __ANDROID__
switch (error) {
    case BT_SUCCESS:
        return JUBR_OK;
    case BT_ERR_CONNECT_FAIL:
        return JUBR_CONNECT_DEVICE_ERROR;
    case BT_ERR_BOND_FAIL:
        return JUBR_BT_BOND_FAILED;
    case BT_ERR_NOT_ENABLE:
    case BT_ERR_NOT_SUPPORT:
    case BT_ERR_NO_LOCATION_PERMISSION:
    case BT_ERR_TRANSMIT_ERROR:
    default:
        return JUBR_TRANSMIT_DEVICE_ERROR;
} // switch (error) end
#else
switch ((unsigned long)error) {
    case CKR_OK:
        return JUBR_OK;
    case CKR_BLE_BOND_FAIL:
        return JUBR_BT_BOND_FAILED;
    case CKR_BLE_CONNECT_FAIL:
        return JUBR_CONNECT_DEVICE_ERROR;
    default:
        return JUBR_TRANSMIT_DEVICE_ERROR;
} // switch ((unsigned long)error) end
#endif
}

JUB_RV JubiterBLEDevice::Connect(const std::string params /* = ""*/) {

    // parse parms
//     unsigned char guid[] = "0000-0000-0000";
//
//     FT_BLE_ConnDev(guid, 0, &_handle, 12000);

    return JUBR_OK;
}

JUB_RV JubiterBLEDevice::Disconnect() {

    if (             0 != _handle
        && IFD_SUCCESS == FT_BLE_DisConn(_handle)
        ) {

        _handle = 0;
    }

    return JUBR_OK;
}

JUB_RV JubiterBLEDevice::SendData(IN JUB_BYTE_CPTR sendData, IN JUB_ULONG ulSendLen,
                                  OUT JUB_BYTE_PTR retData, INOUT JUB_ULONG_PTR pulRetDataLen,
                                  IN JUB_ULONG ulMiliSecondTimeout) {

    if (    0 == _handle
        || !FT_BLE_IsConn(_handle)
        ) {
        return JUBR_NOT_CONNECT_DEVICE;
    }

#ifdef _DEBUG
	DeviceIOLogHelper handler(sendData, ulSendLen, retData, pulRetDataLen);
#endif

    auto& fido = Fido::instance();
    fido.clear();

    unsigned char sendMsg[2048] = {0,};
    unsigned int uiSendLen = (unsigned int)sizeof(sendMsg)/sizeof(unsigned char);
    // 封装fido协议
    fido.wrapFidoApdu(CMD_MSG, const_cast<unsigned char *>(sendData), (unsigned int)ulSendLen, sendMsg, &uiSendLen);

    int ret = FT_BLE_SendAPDU(_handle, const_cast<unsigned char *>(sendMsg), uiSendLen);
    if (0 != ret) {
        return MatchErrorCode(ret);
    }

    auto status = fido.waitForReceive(ulMiliSecondTimeout);
    if (!_bConnected) {
        return JUBR_NOT_CONNECT_DEVICE;
    }
    if (std::cv_status::timeout == status) {
        return JUBR_TRANSACT_TIMEOUT;
    }

    auto respStatus = fido.checkResponse();
    switch (respStatus) {
        case Fido::ResponseStatus::empty:
            return JUBR_TRANSMIT_DEVICE_ERROR;
        case Fido::ResponseStatus::invalid:
            return JUBR_TRANSMIT_DEVICE_ERROR;
        case Fido::ResponseStatus::receiving:
            return JUBR_TRANSACT_TIMEOUT;
        case Fido::ResponseStatus::done:
            break;
    } // switch (respStatus) end

    auto resp = fido.response();
    if (   NULL == retData
        || NULL == pulRetDataLen
        ) {
        return JUBR_OK;
    }

    ret = fido.parseFidoApdu(resp.data(), (unsigned int)resp.size(), retData, (unsigned int *) pulRetDataLen);

    return MatchErrorCode(ret);
}

unsigned int JubiterBLEDevice::Initialize(const BLE_INIT_PARAM& params) {

	// init with inner _param
	_param.param = params.param;
	unsigned int ret = FT_BLE_Initialize(_param);
	if (IFD_SUCCESS == ret) {
		outerParams = params;
	}

    return (unsigned int)MatchErrorCode(ret);
}

unsigned int JubiterBLEDevice::Finalize() {

	outerParams = {0, 0, 0, 0};
	return FT_BLE_Finalize(); 
}

unsigned int JubiterBLEDevice::Scan() {

	return (unsigned int)MatchErrorCode(FT_BLE_Scan());
}

unsigned int JubiterBLEDevice::StopScan() {

	return (unsigned int)MatchErrorCode(FT_BLE_StopScan());
}

unsigned int JubiterBLEDevice::Connect(unsigned char* bBLEUUID,
                                       unsigned int connectType,
                                       unsigned long* pdevHandle,
                                       unsigned int timeout) {

    unsigned int ret = FT_BLE_ConnDev(bBLEUUID, connectType, pdevHandle, timeout);
    if (IFD_SUCCESS == ret) {
        _handle = *pdevHandle;
        _bConnected = true;
    }
    ExtraSetting();

    return (unsigned int)MatchErrorCode(ret);
}

unsigned int JubiterBLEDevice::CancelConnect(unsigned char* bBLEUUID) {

	unsigned int ret = FT_BLE_CancelConnDev(bBLEUUID);
	_handle = 0;
    _bConnected = false;

	return (unsigned int)MatchErrorCode(ret);
}

unsigned int JubiterBLEDevice::Disconnect(unsigned long handle) {

	unsigned int ret = FT_BLE_DisConn(handle);
	if (IFD_SUCCESS == ret) {
		_handle = 0;
        _bConnected = false;
	}

	return (unsigned int)MatchErrorCode(ret);
}

unsigned int JubiterBLEDevice::IsConnect(unsigned long handle) {

    // FT_BLE_IsConn: Non-zero means success, and zero means failure
    if (0 != FT_BLE_IsConn(handle)) {
        return JUBR_OK;
    }
    else {
        return JUBR_CONNECT_DEVICE_ERROR;
    }
}

unsigned long JubiterBLEDevice::GetHandle() {

	return _handle;
}

void JubiterBLEDevice::SetHandle(unsigned long handle) {

	_handle = handle;
}

void JubiterBLEDevice::SetConnectStatuteFalse() {

    _bConnected = false;
}

int JubiterBLEDevice::BLE_ReadCallBack(unsigned long devHandle,
                                       unsigned char* data, unsigned int uiDataLen) {
//    if (!(IS_BLE_MODE)) {
//        return IFD_NOT_SUPPORTED;
//    }

    // analyse data here...

    auto bleDevice = Singleton<jub::JubiterBLEDevice>::GetInstance();
    if (bleDevice) {
        Fido::RecvCallBack(devHandle, data, uiDataLen);
    }

    return IFD_SUCCESS;
}

void JubiterBLEDevice::BLE_ScanCallBack(unsigned char* devName,
                                        unsigned char* uuid,
                                        unsigned int type) {
//    if (!(IS_BLE_MODE)) {
//        return;
//    }

    auto bleDevice = Singleton<jub::JubiterBLEDevice>::GetInstance();
    if (bleDevice) {
        if (bleDevice->outerParams.scanCallBack) {
            bleDevice->outerParams.scanCallBack(devName, uuid, type);
        }
    }

    return;
}

void JubiterBLEDevice::BLE_DiscCallBack(unsigned char* uuid) {
//    if (!(IS_BLE_MODE)) {
//        return;
//    }

    auto bleDevice = Singleton<jub::JubiterBLEDevice>::GetInstance();
    if (bleDevice) {
		bleDevice->SetHandle(0);
        bleDevice->SetConnectStatuteFalse();
        Fido::instance().stopReceiving();
        if (bleDevice->outerParams.discCallBack) {
            bleDevice->outerParams.discCallBack(uuid);
        }
	}

    return;
}

void JubiterBLEDevice::ExtraSetting() {
#if defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_OS_IOS // ios
    FT_BLE__SetIsReConnected(false);
    // cmd: 00A40400
    uchar_vector cmd("00A40400");
    unsigned char resp[0x100] = {0,};
    JUB_ULONG ulRespLen = sizeof(resp)/sizeof(unsigned char);
    auto rv = SendData(cmd.data(), cmd.size(), resp, &ulRespLen);
    if (JUBR_OK != rv) {
        return;
    }

    if (2 > ulRespLen) {
        return;
    }

    JUB_RV ret = ret = resp[ulRespLen - 2] * 0x100 + resp[ulRespLen - 1];
    if (0x9000 != ret) {
        return;
    }

    // cmd:  80CB800005DFFF028100
    // resp: 2003D86A571205504A7542697465722D71686A769000
    cmd.setHex("80CB800005DFFF028100");
    ulRespLen = sizeof(resp)/sizeof(unsigned char);
    rv = SendData(cmd.data(), cmd.size(), resp, &ulRespLen);
    if (JUBR_OK != rv) {
        return;
    }
    if (4 > ulRespLen) {
        return;
    }
    ret = resp[ulRespLen - 2] * 0x100;
    ret += resp[ulRespLen - 1];
    if (0x9000 != ret) {
        return;
    }
    if (   0x20 != resp[0]
        || 0x03 != resp[1]
        ) {
        return;
    }

    FT_BLE__SetIsReConnected(true);
#endif // #if TARGET_OS_IOS
#endif // #if defined(__APPLE__)
}

}  // namespace jub end

#endif // USE_BLE_DEVICE
