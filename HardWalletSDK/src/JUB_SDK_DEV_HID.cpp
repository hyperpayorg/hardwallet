//
//  JUB_SDK_DEV_HID.cpp
//  JubSDK
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#include "JUB_SDK_DEV_HID.h"

#include "utility/util.h"

#include "device/JubiterHidDevice.hpp"
#include "token/JubiterBLDImpl.h"
/*****************************************************************************
 * @function name : JUB_ListDeviceHid
 * @in  param :
 * @out param : deviceIDs - device ID list
 * @last change :
 *****************************************************************************/
JUB_RV JUB_ListDeviceHid(OUT JUB_UINT16 deviceIDs[MAX_DEVICE]) {

#ifdef HID_MODE
    auto path_list = jub::JubiterHidDevice::EnumDevice();
    //std::cout <<"** "<< path_list.size() << std::endl;

    //deal removed key
    auto vDeviceIDs = jub::TokenManager::GetInstance()->GetHandleList();
    for (JUB_UINT16 i = 0; i < vDeviceIDs.size(); i++) {
        auto token = (jub::JubiterBLDImpl*)jub::TokenManager::GetInstance()->GetOne(vDeviceIDs[i]);
        if (std::end(path_list) == std::find(std::begin(path_list), std::end(path_list), token->getPath())) {
            //removed key
            jub::TokenManager::GetInstance()->ClearOne(vDeviceIDs[i]);
        }
    }

    //deal inserted key
    auto isInManager = [](std::string path)-> bool {
        auto vDeviceIDs = jub::TokenManager::GetInstance()->GetHandleList();
        for (JUB_UINT16 i = 0; i < vDeviceIDs.size(); i++) {
            auto token = (jub::JubiterBLDImpl*)jub::TokenManager::GetInstance()->GetOne(vDeviceIDs[i]);
            if (   token
                && path == token->getPath()
                ) {
                return true;
            }
        }

        return false;
    };

    for (auto path : path_list) {
        if (!isInManager(path)) {
            //new inserted key
            jub::JubiterBLDImpl* token = new jub::JubiterBLDImpl(path);
            jub::TokenManager::GetInstance()->AddOne(token);
        }
    }

    auto _vDeviceIDs = jub::TokenManager::GetInstance()->GetHandleList();
    for (JUB_UINT16 i = 0 ; i < std::min((size_t)MAX_DEVICE, _vDeviceIDs.size()); i++) {
        deviceIDs[i] = _vDeviceIDs[i];
    }

    return JUBR_OK;
#else
    return JUBR_IMPL_NOT_SUPPORT;
#endif // #ifdef HID_MODE
}

/*****************************************************************************
 * @function name : JUB_ConnetDeviceHid
 * @in  param : deviceID - device ID
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_RV JUB_ConnetDeviceHid(IN JUB_UINT16 deviceID) {

#ifdef HID_MODE
    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->ConnectToken());

    return JUBR_OK;
#else
    return JUBR_IMPL_NOT_SUPPORT;
#endif // #ifdef HID_MODE
}

/*****************************************************************************
 * @function name : JUB_DisconnetDeviceHid
 * @in  param : deviceID - device ID
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_RV JUB_DisconnetDeviceHid(IN JUB_UINT16 deviceID) {

#ifdef HID_MODE
    auto token = jub::TokenManager::GetInstance()->GetOne(deviceID);
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->DisconnectToken());

    return JUBR_OK;
#else
    return JUBR_IMPL_NOT_SUPPORT;
#endif // #ifdef HID_MODE
}
