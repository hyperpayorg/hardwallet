//
//  JUB_SDK_DEV.cpp
//  JubSDK
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#include "JUB_SDK_DEV.h"

#include <string.h>

#include "utility/util.h"
#include "utility/Singleton.h"

#include "context/Context.h"
#include "context/ContextBTC.h"
#include "token/interface/TokenInterface.hpp"

JUB_RV _allocMem(JUB_CHAR_PTR_PTR memPtr, const std::string &strBuf);

stDevicdInfo::stDevicdInfo() {
     memset(label, 0x00, sizeof(label)/sizeof(JUB_CHAR));
     memset(sn, 0x00, sizeof(sn)/sizeof(JUB_CHAR));
     pinRetry = 0;
     pinMaxRetry = 0;
     memset(bleVersion, 0x00, sizeof(bleVersion)/sizeof(JUB_CHAR));
     memset(firmwareVersion, 0x00, sizeof(firmwareVersion)/sizeof(JUB_CHAR));
}

/*****************************************************************************
 * @function name : JUB_GetDeviceInfo
 * @in  param : deviceID - device ID
 * @out param : info - device info
 * @last change :
 *****************************************************************************/
JUB_RV JUB_GetDeviceInfo(IN JUB_UINT16 deviceID,
                         OUT JUB_DEVICE_INFO_PTR info) {

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);

    /*
     JUB_VERIFY_RV(token->getPinRetry(info.pinRetry));
     JUB_VERIFY_RV(token->getPinMaxRetry(info.pinMaxRetry));
     JUB_VERIFY_RV(token->getSN(sn));
     JUB_VERIFY_RV(token->getLabel(label));*/

    // Let's go to the main security domain,
    // instead of judging the return value,
    // to get the data back
    JUB_IsBootLoader(deviceID);

    JUB_BYTE sn[24] = {0,};
    JUB_BYTE label[32] = {0,};
    JUB_BYTE retry = 0;
    JUB_BYTE maxRetry = 0;
    JUB_BYTE bleVersion[4] = {0,};
    JUB_BYTE fwVersion[4] = {0,};

    JUB_VERIFY_RV(token->GetPinRetry(retry));
    JUB_VERIFY_RV(token->GetPinMaxRetry(maxRetry));
    JUB_VERIFY_RV(token->GetSN(sn));
    JUB_VERIFY_RV(token->GetLabel(label));
    JUB_VERIFY_RV(token->GetBleVersion(bleVersion));
    JUB_VERIFY_RV(token->GetFwVersion(fwVersion));

    memcpy(info->sn, sn, sizeof(sn)/sizeof(JUB_BYTE));
    memcpy(info->label, label, sizeof(label)/sizeof(JUB_BYTE));
    info->pinRetry = retry;
    info->pinMaxRetry = maxRetry;
    memcpy(info->bleVersion, bleVersion, sizeof(bleVersion)/sizeof(JUB_BYTE));
    memcpy(info->firmwareVersion, fwVersion, sizeof(fwVersion)/sizeof(JUB_BYTE));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_IsInitialize
 * @in  param : deviceID - device ID
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_ENUM_BOOL JUB_IsInitialize(IN JUB_UINT16 deviceID) {

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    if (nullptr == token) {
        return BOOL_FALSE;
    }

    return (JUB_ENUM_BOOL)token->IsInitialize();
}

/*****************************************************************************
 * @function name : JUB_IsBootLoader
 * @in  param : deviceID - device ID
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_ENUM_BOOL JUB_IsBootLoader(IN JUB_UINT16 deviceID) {

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    if (nullptr == token) {
        return BOOL_FALSE;
    }

    jub::ContextManager::GetInstance()->ClearLast();

    return (JUB_ENUM_BOOL)token->IsBootLoader();
}

/*****************************************************************************
 * @function name : JUB_EnumApplets
 * @in  param : deviceID - device ID
 * @out param : appList - applet list
 * @last change :
 *****************************************************************************/
JUB_RV JUB_EnumApplets(IN JUB_UINT16 deviceID,
                       OUT JUB_CHAR_PTR_PTR appList) {

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);

    std::string appletList;
    JUB_VERIFY_RV(token->EnumApplet(appletList));
    JUB_VERIFY_RV(_allocMem(appList, appletList));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : Jub_EnumSupportCoins
 * @in  param : deviceID - device ID
 * @out param : coinsList - coin list
 * @last change :
 *****************************************************************************/
JUB_RV Jub_EnumSupportCoins(IN JUB_UINT16 deviceID,
                            OUT JUB_CHAR_PTR_PTR coinsList) {

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    if (nullptr == token) {
        JUB_VERIFY_RV(JUBR_ERROR);
    }

    std::string str_coinsList;
    JUB_VERIFY_RV(token->EnumSupportCoins(str_coinsList));
    JUB_VERIFY_RV(_allocMem(coinsList, str_coinsList));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_GetAppletVersion
 * @in  param : deviceID - device ID
 *            : appID - applet ID
 * @out param : version - applet version
 * @last change :
 *****************************************************************************/
JUB_RV JUB_GetAppletVersion(IN JUB_UINT16 deviceID,
                            IN JUB_CHAR_PTR appID,
                            OUT JUB_CHAR_PTR_PTR version) {

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);

    std::string str_version;
    JUB_VERIFY_RV(token->GetAppletVersion(appID,str_version));
    JUB_VERIFY_RV(_allocMem(version, str_version));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_SetTimeOut
 * @in  param : contextID - context ID
 *            : timeout - how many s
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_RV JUB_SetTimeOut(IN JUB_UINT16 contextID,
                      IN JUB_UINT16 timeout) {

    auto context = (jub::ContextBTC*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    if (600 < timeout) {
        JUB_VERIFY_RV(JUBR_ERROR_ARGS);
    }

    JUB_VERIFY_RV(context->SetTimeout(timeout * 2));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_GetDeviceCert
 * @in  param : deviceID - device ID
 * @out param : cert - device certificate
 * @last change :
 *****************************************************************************/
JUB_RV JUB_GetDeviceCert(IN JUB_UINT16 deviceID,
                         OUT JUB_CHAR_PTR_PTR cert) {

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);

    // Let's go to the main security domain,
    // instead of judging the return value,
    // to get the data back
    JUB_IsBootLoader(deviceID);

    std::string str_cert;
    JUB_VERIFY_RV(token->GetDeviceCert(str_cert));
    JUB_VERIFY_RV(_allocMem(cert, str_cert));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_SendOneApdu
 * @in  param : deviceID - device ID
 *            : apdu - one apdu
 * @out param : response
 * @last change :
 *****************************************************************************/
JUB_RV JUB_SendOneApdu(IN JUB_UINT16 deviceID,
                       IN JUB_CHAR_PTR apdu,
                       OUT JUB_CHAR_PTR_PTR response) {

    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);

    std::string str_response;
    JUB_VERIFY_RV(token->SendOneApdu(apdu, str_response));
    JUB_VERIFY_RV(_allocMem(response, str_response));

    return JUBR_OK;
}
