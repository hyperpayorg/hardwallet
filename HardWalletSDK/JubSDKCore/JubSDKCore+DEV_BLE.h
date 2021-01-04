//
//  JubSDKCore+DEV_BLE.h
//  JubSDKCore
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore.h"

NS_ASSUME_NONNULL_BEGIN

//typedef int(*DEV_ReadCallBack)(JUB_ULONG devHandle, JUB_BYTE_PTR data, JUB_UINT32 dataLen);
//typedef void(*DEV_ScanCallBack)(JUB_BYTE_PTR devName, JUB_BYTE_PTR uuid, JUB_UINT32 type);
//typedef void(*DEV_DiscCallBack)(JUB_BYTE_PTR uuid);
//typedef struct _DEVICE_INIT_PARAM_ {
//    void* param;
//    DEV_ReadCallBack callBack;
//    DEV_ScanCallBack scanCallBack;
//    DEV_DiscCallBack discCallBack;
//} DEVICE_INIT_PARAM;
typedef  int(*ReadCallBack)(unsigned long int devHandle, unsigned char* data, unsigned int uiDataLen);
typedef void(*ScanCallBack)(unsigned char* devName, unsigned char* uuid, unsigned int type);
typedef void(*DiscCallBack)(unsigned char* uuid);
@interface DeviceInitParam : NSObject {
    void* param;
    ReadCallBack readCallBack;
    ScanCallBack scanCallBack;
    DiscCallBack discCallBack;
}
@property (atomic, assign) void* param;
@property (atomic, assign) ReadCallBack readCallBack;
@property (atomic, assign) ScanCallBack scanCallBack;
@property (atomic, assign) DiscCallBack discCallBack;
@end

@interface JubSDKCore (DEV_BLE)

//JUB_RV JUB_initDevice(IN DEVICE_INIT_PARAM param);
- (void) JUB_initDevice :(DeviceInitParam*)param;

//JUB_RV JUB_enumDevices(void);
- (void)JUB_enumDevices;

//JUB_RV JUB_stopEnumDevices(void);
- (void)JUB_stopEnumDevices;

//JUB_RV JUB_connectDevice(JUB_BYTE_PTR bBLEUUID,  /**< ble device UUID */
//                         JUB_UINT32 connectType, /**< ble device connect type */
//                         JUB_UINT16* pDeviceID,  /**< output ble device connect handle */
//                         JUB_UINT32 timeout);
- (NSUInteger)JUB_connectDevice:(NSString*)deviceName
                    connectType:(NSUInteger)connectType
                        timeout:(NSUInteger)timeout;

//JUB_RV JUB_cancelConnect(JUB_BYTE_PTR bBLEUUID);
- (void)JUB_cancelConnect:(NSString*)deviceName;

//JUB_RV JUB_disconnectDevice(JUB_UINT16 deviceID);
- (void)JUB_disconnectDevice:(NSUInteger)deviceID;

//JUB_RV JUB_isDeviceConnect(JUB_UINT16 deviceID);
- (void)JUB_isDeviceConnect:(NSUInteger)deviceID;

//JUB_RV JUB_QueryBattery(IN JUB_UINT16 deviceID,
//                        OUT JUB_BYTE_PTR percent);
- (NSString*)JUB_QueryBattery:(NSUInteger)deviceID;

@end

NS_ASSUME_NONNULL_END
