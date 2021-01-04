//
//  JubSDKCore+DEV.h
//  JubSDKCore
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore.h"

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

NS_ASSUME_NONNULL_BEGIN

//typedef struct {
//    JUB_CHAR label[32];
//    JUB_CHAR sn[24];
//    JUB_UINT16 pinRetry;
//    JUB_UINT16 pinMaxRetry;
//    JUB_CHAR bleVersion[4];
//    JUB_CHAR firmwareVersion[4];
//} JUB_DEVICE_INFO;
@interface DeviceInfo : NSObject
@property (atomic, copy  ) NSString* label;
@property (atomic, copy  ) NSString* sn;
@property (atomic, assign) NSInteger pinRetry;
@property (atomic, assign) NSInteger pinMaxRetry;
@property (atomic, copy  ) NSString* bleVersion;
@property (atomic, copy  ) NSString* firmwareVersion;
@end

@interface JubSDKCore (DEV)

//JUB_RV JUB_GetDeviceInfo(IN JUB_UINT16 deviceID,
//                         OUT JUB_DEVICE_INFO_PTR info);
- (DeviceInfo*)JUB_GetDeviceInfo:(NSUInteger)deviceID;

//JUB_RV JUB_GetDeviceCert(IN JUB_UINT16 deviceID,
//                         OUT JUB_CHAR_PTR_PTR cert);
- (NSString*)JUB_GetDeviceCert:(NSUInteger)deviceID;

//JUB_RV JUB_SendOneApdu(IN JUB_UINT16 deviceID,
//                       IN JUB_CHAR_PTR apdu ,
//                       OUT JUB_CHAR_PTR_PTR response);
- (NSString*)JUB_SendOneApdu:(NSUInteger)deviceID
                        apdu:(NSString*)apdu;

//JUB_ENUM_BOOL JUB_IsInitialize(IN JUB_UINT16 deviceID);
- (JUB_NS_ENUM_BOOL)JUB_IsInitialize:(NSUInteger)deviceID;

//JUB_ENUM_BOOL JUB_IsBootLoader(IN JUB_UINT16 deviceID);
- (JUB_NS_ENUM_BOOL)JUB_IsBootLoader:(NSUInteger)deviceID;

//JUB_RV JUB_SetTimeOut(IN JUB_UINT16 contextID, IN JUB_UINT16 timeout);
- (void)JUB_SetTimeOut:(NSUInteger)contextID
               timeout:(NSUInteger)timeout;

//JUB_RV JUB_FreeMemory(IN JUB_CHAR_CPTR memPtr);
//- (void) JUB_FreeMemory :(NSString*)memPtr;

//JUB_RV JUB_EnumApplets(IN JUB_UINT16 deviceID,
//                       OUT JUB_CHAR_PTR_PTR appList);
- (NSString*)JUB_EnumApplets:(NSUInteger)deviceID;

//JUB_RV Jub_EnumSupportCoins(IN JUB_UINT16 deviceID,
//                            OUT JUB_CHAR_PTR_PTR coinList);
- (NSString*)Jub_EnumSupportCoins:(NSUInteger)deviceID;

//JUB_RV JUB_GetAppletVersion(IN JUB_UINT16 deviceID,
//                            IN JUB_CHAR_PTR appID,
//                            OUT JUB_CHAR_PTR_PTR version);
- (NSString*)JUB_GetAppletVersion:(NSUInteger)deviceID
                            appID:(NSString*)appID;

@end

NS_ASSUME_NONNULL_END
