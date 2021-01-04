//
//  bleTransmit.m
//  bleTransmit
//
//  Copyright © 1998-2016, FEITIAN Technologies Co., Ltd. All rights reserved.
//

#include "bleTransmit.h"

#ifdef __ANDROID__
#include <bleTransmit/android/BTManager.h>
#include <logUtils.h>

#elif defined(TARGET_OS_IPHONE)
#include "BLEInterface.h"
#endif

#if   defined(JSSAFE)
#define __PASTE(x,y)                           x##y
#define CK_DECLARE_FUNCTION(returnType, name)  returnType name
#define CK_FUNCTION_INFO(name)                 CK_DECLARE_FUNCTION(unsigned int, __PASTE(JS_, name))
#define ADDPRE(name)                           __PASTE(JS_, name)
#else
#define __PASTE(x,y)                           x##y
#define CK_DECLARE_FUNCTION(returnType, name)  returnType name
#define CK_FUNCTION_INFO(name)                 CK_DECLARE_FUNCTION(unsigned int, __PASTE(FT_, name))
#define ADDPRE(name)                           __PASTE(FT_, name)
#endif // defined(JSSAFE)

#define CK_NEED_ARG_LIST

#ifndef IN
#define IN
#define COMMFRAMEWORKF_IN
#endif

#ifndef OUT
#define OUT
#define COMMFRAMEWORKF_OUT
#endif


CK_FUNCTION_INFO(BLE_Initialize)
#ifdef CK_NEED_ARG_LIST
(BLE_INIT_PARAM param)
#endif
{
    unsigned int ret = IFD_SUCCESS;
#ifdef __ANDROID__
    BT_INIT_PARAM mParam;
    mParam.param = param.param;
    mParam.callBack = param.callBack;
    mParam.discCallBack = param.discCallBack;
    mParam.scanCallBack = param.scanCallBack;

    ret = FT_BTManager::GetInstance()->initialize(mParam);
    if (0 != ret)
    {
        LOG_ERR("BLE initialize failed");
    }
#elif defined(TARGET_OS_IPHONE)
    //whl 20170327
//    [[BLEManager sharedManager] setParam:param];
    BLE_AUX_INIT_PARAM initPara ;
    //memset(&initPara, 0, sizeof(initPara));
    initPara.connFuncCallBack = nullptr;
    initPara.disconnFuncCallBack = (BLE_AUX_DisconnFuncCallBack)param.discCallBack;
    initPara.rcvDataFuncCallBack = (BLE_AUX_RcvDataFuncCallBack)param.callBack;
    initPara.scanFuncCallBack = (BLE_AUX_ScanFuncCallBack)param.scanCallBack;
    ret = ADDPRE(BLEInitialize(&initPara));
    ADDPRE(BLESetIsReConnected(false));
#endif
    return ret;
}

CK_FUNCTION_INFO(BLE_Finalize)
#ifdef CK_NEED_ARG_LIST
(void)
#endif
{
    unsigned int ret = IFD_SUCCESS;
#ifdef __ANDROID__

    ret = FT_BTManager::GetInstance()->finalize();
    if (0 != ret)
    {
        LOG_ERR("BLE Finalize failed");
    }
#elif defined(TARGET_OS_IPHONE)
    //whl 20170327
//    [BLEManager sharedFinal];
    ret = ADDPRE(BLEFinalizedBLE());
#endif

    return ret;
}

CK_FUNCTION_INFO(BLE_Scan)
#ifdef CK_NEED_ARG_LIST
(void)
#endif
{
    unsigned int ret = IFD_SUCCESS;
#ifdef __ANDROID__
    ret = FT_BTManager::GetInstance()->btStartScan(4, 10);
    if (0 != ret)
    {
        LOG_ERR("BLE Scan failed： %08x", ret);
    }
#elif defined(TARGET_OS_IPHONE)
    //whl 20170327
//    [[BLEManager sharedManager] scanDevice];
    ret = ADDPRE(BLEScan());
#endif
    return ret;
}

CK_FUNCTION_INFO(BLE_StopScan)
#ifdef CK_NEED_ARG_LIST
(void)
#endif
{
    unsigned int ret = IFD_SUCCESS;
#ifdef __ANDROID__
    ret = FT_BTManager::GetInstance()->btStopScan();
    if (0 != ret)
    {
        LOG_ERR("BLE Stop Scan Failed");
    }
#elif defined(TARGET_OS_IPHONE)
    //whl 20170327
//    [[BLEManager sharedManager] stopScan];
    ret = ADDPRE(BLEStopScan());;
#endif
    return ret;
}

CK_FUNCTION_INFO(BLE_ConnDev)
#ifdef CK_NEED_ARG_LIST
(
 unsigned char* bBLEUUID,      /**< ble device UUID */
 unsigned int   connectType,   /**< ble device connect type */
 unsigned long* pDevHandle,    /**< output ble device connect handle */
 unsigned int   timeout
 )
#endif
{
    unsigned int ret = IFD_SUCCESS;
#ifdef __ANDROID__
    LOG_ERR("===>>> BLE_ConnDev - connectType: %d", connectType);
    ret = FT_BTManager::GetInstance()->btConnect(bBLEUUID, pDevHandle, timeout);
    if (0 != ret)
    {
        LOG_ERR("BLE Connect failed: %08X", ret);
    }
    else
    {
        LOG_ERR("句柄111： %ld", *pDevHandle);
    }
#elif defined(TARGET_OS_IPHONE)
    ADDPRE(BLESetIsReConnected(false));
    //whl 20170327
    ret = ADDPRE(BLEConnectDev((const char*)bBLEUUID, pDevHandle, timeout));
//    NSString* name = [NSString stringWithUTF8String:(const char*)bBLEUUID];
//    if (YES == [[BLEManager sharedManager] connectDeviceByname:name withTimeOut:timeout]) {
//        *pDevHandle = (unsigned long)[[BLEManager sharedManager] currentPer];
//        
//        return IFD_SUCCESS;
//    }
    
//    return IFD_TIMEOUT;

#endif
    return ret;
}

CK_FUNCTION_INFO(BLE_CancelConnDev)
#ifdef CK_NEED_ARG_LIST
(
 unsigned char* bBLEUUID      /**< ble device UUID */
 )
#endif
{
    unsigned int ret = IFD_SUCCESS;
#ifdef __ANDROID__
    ret = FT_BTManager::GetInstance()->btCancelConnect(bBLEUUID);
    if (0 != ret)
    {
        LOG_ERR("BLE btCancelConnect Failed: %08X", ret);
    }
#elif defined(TARGET_OS_IPHONE)
    //whl 20170327
    ret = ADDPRE(BLECancelConnect((const char*)bBLEUUID));

#endif
    return ret;
}

CK_FUNCTION_INFO(BLE_DisConn)
#ifdef CK_NEED_ARG_LIST
(
 unsigned long devHandle               /**< output ble device connect handle */
 )
#endif
{
    unsigned int ret = IFD_SUCCESS;
#ifdef __ANDROID__
    ret = FT_BTManager::GetInstance()->btDisconnect(devHandle);
    if (0 != ret)
    {
        LOG_ERR("BLE Disconnect Failed: %08X", ret);
    }
#elif defined(TARGET_OS_IPHONE)
    //iOS 代码后续需补全
//    [[BLEManager sharedManager] disconnectDevice];
//    ret = IFD_SUCCESS;
    ADDPRE(BLESetIsReConnected(false));
    ret = ADDPRE(BLEDisconnect(devHandle));
#endif
    return ret;
}

/**
 * add by fs - 20170602
 * 该接口按 bool 值处理。 0 - false; 非 0 - true;
 */
CK_FUNCTION_INFO(BLE_IsConn)
#ifdef CK_NEED_ARG_LIST
(
 unsigned long devHandle       /**< output ble device connect handle */
 )
#endif
{
    unsigned int ret = IFD_SUCCESS;
#ifdef __ANDROID__
    // 注:这里返回的状态注意区分
    ret = FT_BTManager::GetInstance()->btIsConnected(devHandle);
    if (0 == ret) {
        LOG_ERR("btIsConnected: %08x", ret);
    }
#elif defined(TARGET_OS_IPHONE)
    ret = ADDPRE(BLEIsConnected(devHandle));
#else
#endif
    return ret;
}

/**
 * add by Pan Min - 20190719
 * 该接口按 bool 值处理。 0 - false; 非 0 - true;
 */
CK_FUNCTION_INFO(BLE__SetIsReConnected)
#ifdef CK_NEED_ARG_LIST
(
 bool on       /**<reconnect or not  */
 )
#endif
{
#ifdef __ANDROID__

#elif defined(TARGET_OS_IPHONE)
    ADDPRE(BLESetIsReConnected(on));
#else
#endif
    return IFD_SUCCESS;
}

CK_FUNCTION_INFO(BLE_SendAPDU)
#ifdef CK_NEED_ARG_LIST
(
 unsigned long  devHandle,
 unsigned char* apdu,
 unsigned int   apduLen
 )
#endif
{
    unsigned int ret = IFD_SUCCESS;
#ifdef __ANDROID__
    BYTE recv[2048] = {0,};
    ULONG recvLen = 2048;
    ret = FT_BTManager::GetInstance()->btAsyncTransmit(devHandle, apdu, apduLen);
    if (0 != ret)
    {
        LOG_ERR("btSyncTransmit failed: %08X", ret);
    }
#elif defined(TARGET_OS_IPHONE)
    //whl 20170327
//    ret = [[BLEManager sharedManager] writeData:apdu withLen:apduLen];
    ret = ADDPRE(BLESendData(devHandle,apdu, apduLen));
#endif
    return ret;
}

#undef CK_NEED_ARG_LIST

#ifdef COMMFRAMEWORKF_IN
#undef IN
#undef COMMFRAMEWORKF_IN
#endif

#ifdef COMMFRAMEWORKF_OUT
#undef OUT
#undef COMMFRAMEWORKF_OUT
#endif

#if    defined(FTSAFE) || defined(JSSAFE)
#undef ADDPRE
#undef CK_CALLBACK_FUNC
#undef CK_FUNCTION_INFO
#undef CK_DECLARE_FUNCTION
#undef __PASTE
#endif

