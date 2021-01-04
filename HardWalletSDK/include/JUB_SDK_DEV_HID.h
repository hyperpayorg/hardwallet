//
//  JUB_SDK_DEV_HID.h
//  JubSDK
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#ifndef JUB_SDK_DEV_HID_h
#define JUB_SDK_DEV_HID_h

#include "JUB_SDK.h"

#define MAX_DEVICE                  8

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*****************************************************************************
 * @function name : JUB_ListDeviceHid
 * @in  param :
 * @out param : deviceIDs - device ID list
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_ListDeviceHid(OUT JUB_UINT16 deviceIDs[MAX_DEVICE]);

/*****************************************************************************
 * @function name : JUB_ConnetDeviceHid
 * @in  param : deviceID - device ID
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_ConnetDeviceHid(IN JUB_UINT16 deviceID);

/*****************************************************************************
 * @function name : JUB_DisconnetDeviceHid
 * @in  param : deviceID - device ID
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_DisconnetDeviceHid(IN JUB_UINT16 deviceID);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif /* JUB_SDK_DEV_HID_h */
