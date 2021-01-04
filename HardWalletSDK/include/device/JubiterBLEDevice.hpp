#ifndef __JubiterBLEDevice__
#define __JubiterBLEDevice__

#include "device/DeviceTypeBase.hpp"
#include "JUB_SDK_DEV.h"

#ifdef BLE_MODE
#include <memory>

#include "bleTransmit/bleTransmit.h"

namespace jub {

class JubiterBLEDevice : public DeviceTypeBase {

public:
    JubiterBLEDevice();
    ~JubiterBLEDevice();

public:
    // for common device
    virtual JUB_RV Connect(const std::string path = "");
    virtual JUB_RV Disconnect();

    virtual JUB_RV SendData(IN JUB_BYTE_CPTR sendData, IN JUB_ULONG ulSendLen,
                            OUT JUB_BYTE_PTR retData, INOUT JUB_ULONG_PTR pulRetDataLen,
                            IN JUB_ULONG ulMiliSecondTimeout = 1200000);

public:
    // for ble device
    virtual unsigned int Initialize(const BLE_INIT_PARAM& params);
    virtual unsigned int Finalize();

    virtual unsigned int Scan();
    virtual unsigned int StopScan();

    virtual unsigned int Connect(unsigned char* bBLEUUID,   /**< ble device UUID */
                                 unsigned int connectType,  /**< ble device connect type */
                                 unsigned long* pdevHandle, /**< output ble device connect handle */
                                 unsigned int timeout);

    virtual unsigned int CancelConnect(unsigned char* bBLEUUID);

    virtual unsigned int Disconnect(unsigned long handle);
    virtual unsigned int IsConnect(unsigned long handle);

    virtual unsigned long GetHandle();
    virtual void SetHandle(unsigned long handle);

    virtual void SetConnectStatuteFalse();

    BLE_INIT_PARAM outerParams;

protected:
    static int BLE_ReadCallBack(unsigned long devHandle,
                                unsigned char* data, unsigned int uiDataLen);

    static void BLE_ScanCallBack(unsigned char* devName,
                                 unsigned char* uuid,
                                 unsigned int type);

    static void BLE_DiscCallBack(unsigned char* uuid);

    JUB_RV MatchErrorCode(int error);

    //static std::shared_ptr<jub::JubiterBLEDevice> getThis();

    // check ble version, and set ble library reconnect flag
    void ExtraSetting();

protected:
    /* data */
    BLE_INIT_PARAM _param;
    unsigned long _handle;
    bool _bConnected;
}; // class JubiterBLEDevice end

}  // namespace jub end

#endif  // USE_BLE_DEVICE
#endif  // __JubiterBLEDevice__
