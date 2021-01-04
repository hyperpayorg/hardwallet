//
//  JubSDKCore+DEV_BLE.mm
//  JubSDKCore
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore+DEV_BLE.h"
#import "JUB_SDK_DEV_BLE.h"

//typedef int(*DEV_ReadCallBack)(JUB_ULONG devHandle, JUB_BYTE_PTR data, JUB_UINT32 dataLen);
//typedef void(*DEV_ScanCallBack)(JUB_BYTE_PTR devName, JUB_BYTE_PTR uuid, JUB_UINT32 type);
//typedef void(*DEV_DiscCallBack)(JUB_BYTE_PTR uuid);
//typedef struct _DEVICE_INIT_PARAM_ {
//    void* param;
//    DEV_ReadCallBack callBack;
//    DEV_ScanCallBack scanCallBack;
//    DEV_DiscCallBack discCallBack;
//} DEVICE_INIT_PARAM;
@implementation DeviceInitParam
@synthesize param;
@synthesize readCallBack;
@synthesize scanCallBack;
@synthesize discCallBack;
@end

@implementation JubSDKCore (DEV_BLE)

//JUB_RV JUB_initDevice(IN DEVICE_INIT_PARAM param);
- (void) JUB_initDevice :(DeviceInitParam*)param
{
    DEVICE_INIT_PARAM parameter;
    parameter.param = param.param;
    parameter.callBack     = param.readCallBack;
    parameter.scanCallBack = param.scanCallBack;
    parameter.discCallBack = param.discCallBack;
    
    self.lastError = JUB_initDevice(parameter);
}

//JUB_RV JUB_enumDevices(void);
- (void)JUB_enumDevices
{
    self.lastError = JUB_enumDevices();
}

//JUB_RV JUB_stopEnumDevices(void);
- (void)JUB_stopEnumDevices
{
    self.lastError = JUB_stopEnumDevices();
}

//JUB_RV JUB_connectDevice(JUB_BYTE_PTR bBLEUUID,  /**< ble device UUID */
//                         JUB_UINT32 connectType, /**< ble device connect type */
//                         JUB_UINT16* pDeviceID,  /**< output ble device connect handle */
//                         JUB_UINT32 timeout);
- (NSUInteger)JUB_connectDevice:(NSString*)deviceName
                    connectType:(NSUInteger)connectType
                        timeout:(NSUInteger)timeout
{
    self.lastError = JUBR_OK;
    
    JUB_BYTE_PTR pDeviceName = nullptr;
    if (nil != deviceName) {
        pDeviceName = (JUB_BYTE_PTR)[deviceName UTF8String];
    }
    
    JUB_UINT16 deviceID = 0;
    JUB_RV rv = JUB_connectDevice(pDeviceName,
                                  (JUB_UINT32)connectType,
                                  &deviceID,
                                  (JUB_UINT32)timeout);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return deviceID;
    }
    
    return NSUInteger(deviceID);
}

//JUB_RV JUB_cancelConnect(JUB_BYTE_PTR bBLEUUID);
- (void)JUB_cancelConnect:(NSString*)deviceName
{
    JUB_BYTE_PTR pDeviceName = nullptr;
    if (nil != deviceName) {
        pDeviceName = (JUB_BYTE_PTR)[deviceName UTF8String];
    }
    
    self.lastError = JUB_cancelConnect(pDeviceName);
}

//JUB_RV JUB_disconnectDevice(JUB_UINT16 deviceID);
- (void)JUB_disconnectDevice:(NSUInteger)deviceID
{
    self.lastError = JUB_disconnectDevice(deviceID);
}

//JUB_RV JUB_isDeviceConnect(JUB_UINT16 deviceID);
- (void)JUB_isDeviceConnect:(NSUInteger)deviceID
{
    self.lastError = JUB_isDeviceConnect(deviceID);
}

//JUB_RV JUB_QueryBattery(IN JUB_UINT16 deviceID,
//                        OUT JUB_BYTE_PTR percent);
- (NSString*)JUB_QueryBattery:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    JUB_BYTE percent = 0x00;
    JUB_RV rv = JUB_QueryBattery(deviceID,
                                 &percent);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    return [NSString stringWithFormat:@"%c", percent];
}

@end
