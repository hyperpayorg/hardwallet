//
//  JUB_SDK_DEV_BLE.h
//  JubSDK
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#ifndef JUB_SDK_DEV_BLE_h
#define JUB_SDK_DEV_BLE_h

#include "JUB_SDK_DEV.h"

/// ble device APIs //////////////////////////////////////////
#define DEV_SUCCESS 0               /**< no error */
#define DEV_TIMEOUT 1               /**< conn time out */
#define DEV_COMMUNICATION_ERROR 612 /**< generic error */
#define DEV_RESPONSE_TIMEOUT    613 /**< timeout */
#define DEV_NOT_SUPPORTED       614 /**< request is not supported */
#define DEV_NO_DEVICE           615 /**< no device>*/

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

typedef int(*DEV_ReadCallBack)(JUB_ULONG devHandle, JUB_BYTE_PTR data, JUB_UINT32 dataLen);

typedef void(*DEV_ScanCallBack)(JUB_BYTE_PTR devName, JUB_BYTE_PTR uuid, JUB_UINT32 type);

typedef void(*DEV_DiscCallBack)(JUB_BYTE_PTR uuid);

typedef struct stDeviceInitParam {
    void* param;
    DEV_ReadCallBack callBack;
    DEV_ScanCallBack scanCallBack;
    DEV_DiscCallBack discCallBack;

     stDeviceInitParam();
    ~stDeviceInitParam() = default;
} DEVICE_INIT_PARAM;

JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_initDevice(IN DEVICE_INIT_PARAM param);

JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_enumDevices(void);

JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_stopEnumDevices(void);

JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_connectDevice(JUB_BYTE_PTR bBLEUUID,  /**< ble device UUID */
                         JUB_UINT32 connectType, /**< ble device connect type */
                         JUB_UINT16* pDeviceID,  /**< output ble device connect handle */
                         JUB_UINT32 timeout);

JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_cancelConnect(JUB_BYTE_PTR bBLEUUID);

JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_disconnectDevice(JUB_UINT16 deviceID);

JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_isDeviceConnect(JUB_UINT16 deviceID);

/*****************************************************************************
 * @function name : JUB_QueryBattery
 * @in  param : deviceID - device ID
 * @out param : percent
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_QueryBattery(IN JUB_UINT16 deviceID,
                        OUT JUB_BYTE_PTR percent);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif /* JUB_SDK_DEV_BLE_h */
