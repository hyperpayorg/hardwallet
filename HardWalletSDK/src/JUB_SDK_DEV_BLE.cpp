//
//  JUB_SDK_DEV_BLE.cpp
//  JubSDK
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#include "JUB_SDK_DEV_BLE.h"

#include "utility/util.h"

#include "device/JubiterBLEDevice.hpp"
#include "token/JubiterBLDImpl.h"
#ifdef __ANDROID__
#include "utils/logUtils.h"
#endif

/// ble device APIs //////////////////////////////////////////
// only works in ble (android and ios)

//#if defined(ANDROID) || defined(TARGET_OS_IPHONE)
stDeviceInitParam::stDeviceInitParam() {
     callBack = nullptr;
     scanCallBack = nullptr;
     discCallBack = nullptr;
}

using BLE_device_map = Singleton<xManager<JUB_ULONG>>;

JUB_RV JUB_initDevice(IN DEVICE_INIT_PARAM param) {

#ifdef BLE_MODE
    auto bleDevice = Singleton<jub::JubiterBLEDevice>::GetInstance();
    if (!bleDevice) {
        return JUBR_ERROR;
    }

    JUB_VERIFY_RV(bleDevice->Initialize({param.param,
                                         param.callBack,
                                         param.scanCallBack,
                                         param.discCallBack})
    );

    return JUBR_OK;
#else
    return JUBR_IMPL_NOT_SUPPORT;
#endif // #ifdef BLE_MODE
}

JUB_RV JUB_enumDevices(void) {

#ifdef BLE_MODE
    auto bleDevice = Singleton<jub::JubiterBLEDevice>::GetInstance();
    if (!bleDevice) {
        return JUBR_ERROR;
    }

    JUB_VERIFY_RV(bleDevice->Scan());

    return JUBR_OK;
#else
    return JUBR_IMPL_NOT_SUPPORT;
#endif // #ifdef BLE_MODE
}

JUB_RV JUB_stopEnumDevices(void) {

#ifdef BLE_MODE
    auto bleDevice = Singleton<jub::JubiterBLEDevice>::GetInstance();
    if (!bleDevice) {
        return JUBR_ERROR;
    }

    JUB_VERIFY_RV(bleDevice->StopScan());

    return JUBR_OK;
#else
    return JUBR_IMPL_NOT_SUPPORT;
#endif // #ifdef BLE_MODE
}

JUB_RV JUB_connectDevice(JUB_BYTE_PTR bBLEUUID,
                         JUB_UINT32 connectType,
                         JUB_UINT16* pDeviceID,
                         JUB_UINT32 timeout) {

#ifdef BLE_MODE
    auto bleDevice = Singleton<jub::JubiterBLEDevice>::GetInstance();
    if (!bleDevice) {
        return JUBR_ERROR;
    }

    JUB_ULONG * pdevHandle = new JUB_ULONG;
    JUB_RV rv = bleDevice->Connect(bBLEUUID, connectType, pdevHandle, timeout);
//    LOG_INF("JUB_connectDevice rv: %lu", *pdevHandle);
    JUB_VERIFY_RV(rv);

    *pDeviceID = BLE_device_map::GetInstance()->AddOne(pdevHandle);
//    LOG_INF("JUB_connectDevice rv: %hu", *pDeviceID);
    jub::JubiterBLDImpl* token = new jub::JubiterBLDImpl(bleDevice);
    jub::TokenManager::GetInstance()->AddOne(*pDeviceID, token);

    return rv;
#else
    return JUBR_IMPL_NOT_SUPPORT;
#endif // #ifdef BLE_MODE
}

JUB_RV JUB_cancelConnect(JUB_BYTE_PTR bBLEUUID) {

#ifdef BLE_MODE
    auto bleDevice = Singleton<jub::JubiterBLEDevice>::GetInstance();
    if (!bleDevice) {
        return JUBR_ERROR;
    }

    JUB_VERIFY_RV(bleDevice->CancelConnect(bBLEUUID));

    return JUBR_OK;
#else
    return JUBR_IMPL_NOT_SUPPORT;
#endif // #ifdef BLE_MODE
}

JUB_RV JUB_disconnectDevice(JUB_UINT16 deviceID) {

#ifdef BLE_MODE
    auto bleDevice = Singleton<jub::JubiterBLEDevice>::GetInstance();
    if (!bleDevice) {
        return JUBR_ERROR;
    }

    JUB_ULONG *devHandle = BLE_device_map::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(devHandle);
    JUB_VERIFY_RV(bleDevice->Disconnect(*devHandle));

    return JUBR_OK;
#else
    return JUBR_IMPL_NOT_SUPPORT;
#endif // #ifdef BLE_MODE
}

JUB_RV JUB_isDeviceConnect(JUB_UINT16 deviceID) {

#ifdef BLE_MODE
    auto bleDevice = Singleton<jub::JubiterBLEDevice>::GetInstance();
    if (!bleDevice) {
        return JUBR_ERROR;
    }
    JUB_ULONG *devHandle = BLE_device_map::GetInstance()->GetOne(deviceID);
    if (NULL == devHandle) {
        return JUBR_CONNECT_DEVICE_ERROR;
    }

    JUB_VERIFY_RV(bleDevice->IsConnect(*devHandle));

    return JUBR_OK;
#else
    return JUBR_IMPL_NOT_SUPPORT;
#endif // #ifdef BLE_MODE
}//#endif // #if defined(ANDROID) || defined(TARGET_OS_IPHONE)

/*****************************************************************************
 * @function name : JUB_QueryBattery
 * @in  param : deviceID - device ID
 * @out param : percent
 * @last change :
 *****************************************************************************/
JUB_RV JUB_QueryBattery(IN JUB_UINT16 deviceID,
                        OUT JUB_BYTE_PTR percent) {

#ifdef BLE_MODE
    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->QueryBattery(*percent));

    return JUBR_OK;
#else
    return JUBR_IMPL_NOT_SUPPORT;
#endif
}
