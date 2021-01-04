//
//  JUB_SDK_DEV.h
//  JubSDK
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#ifndef JUB_SDK_DEV_h
#define JUB_SDK_DEV_h

#include "JUB_SDK.h"

#define JUBR_INIT_DEVICE_LIB_ERROR  0x00000060UL
#define JUBR_CONNECT_DEVICE_ERROR   0x00000061UL
#define JUBR_TRANSMIT_DEVICE_ERROR  0x00000062UL
#define JUBR_NOT_CONNECT_DEVICE     0x00000063UL
#define JUBR_DEVICE_PIN_ERROR       0x00000064UL
#define JUBR_USER_CANCEL            0x00000065UL
#define JUBR_ERROR_ARGS             0x00000066UL
#define JUBR_PIN_LOCKED             0x00000067UL

#define JUBR_DEVICE_BUSY            0x00001001
#define JUBR_TRANSACT_TIMEOUT       0x00001002
#define JUBR_OTHER_ERROR            0x00001003
#define JUBR_CMD_ERROR              0x00001004
#define JUBR_BT_BOND_FAILED         0x00001005

#define JUBR_CUSTOM_DEFINED         0x80000000UL
#define JUBR_EOS_APP_INDEP_OK       JUBR_CUSTOM_DEFINED + 1

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

typedef struct stDevicdInfo {
    JUB_CHAR label[32];
    JUB_CHAR sn[24];
    JUB_UINT16 pinRetry;
    JUB_UINT16 pinMaxRetry;
    JUB_CHAR bleVersion[4];
    JUB_CHAR firmwareVersion[4];

     stDevicdInfo();
    ~stDevicdInfo() = default;
} JUB_DEVICE_INFO;
typedef JUB_DEVICE_INFO* JUB_DEVICE_INFO_PTR;

/*****************************************************************************
 * @function name : JUB_GetDeviceInfo
 * @in  param : deviceID - device ID
 * @out param : info - device info
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_GetDeviceInfo(IN JUB_UINT16 deviceID,
                         OUT JUB_DEVICE_INFO_PTR info);

/*****************************************************************************
 * @function name : JUB_GetDeviceCert
 * @in  param : deviceID - device ID
 * @out param : cert - device certificate
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_GetDeviceCert(IN JUB_UINT16 deviceID,
                         OUT JUB_CHAR_PTR_PTR cert);

/*****************************************************************************
 * @function name : JUB_SendOneApdu
 * @in  param : deviceID - device ID
 *            : apdu - one apdu
 * @out param : response
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SendOneApdu(IN JUB_UINT16 deviceID,
                       IN JUB_CHAR_PTR apdu ,
                       OUT JUB_CHAR_PTR_PTR response);

/*****************************************************************************
 * @function name : JUB_IsInitialize
 * @in  param : deviceID - device ID
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_ENUM_BOOL JUB_IsInitialize(IN JUB_UINT16 deviceID);

/*****************************************************************************
 * @function name : JUB_IsBootLoader
 * @in  param : deviceID - device ID
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_ENUM_BOOL JUB_IsBootLoader(IN JUB_UINT16 deviceID);

/*****************************************************************************
 * @function name : JUB_SetTimeOut
 * @in  param : contextID - context ID
 *            : timeout - how many s
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SetTimeOut(IN JUB_UINT16 contextID, IN JUB_UINT16 timeout);

/*****************************************************************************
 * @function name : JUB_EnumApplets
 * @in  param : deviceID - device ID
 * @out param : appList - applet list
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_EnumApplets(IN JUB_UINT16 deviceID,
                       OUT JUB_CHAR_PTR_PTR appList);

/*****************************************************************************
 * @function name : Jub_EnumSupportCoins
 * @in  param : deviceID - device ID
 * @out param : coinsList - coin list
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV Jub_EnumSupportCoins(IN JUB_UINT16 deviceID,
                            OUT JUB_CHAR_PTR_PTR coinsList);

/*****************************************************************************
 * @function name : JUB_GetAppletVersion
 * @in  param : deviceID - device ID
 *            : appID - applet ID
 * @out param : version - applet version
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_GetAppletVersion(IN JUB_UINT16 deviceID,
                            IN JUB_CHAR_PTR appID,
                            OUT JUB_CHAR_PTR_PTR version);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif /* JUB_SDK_DEV_h */
