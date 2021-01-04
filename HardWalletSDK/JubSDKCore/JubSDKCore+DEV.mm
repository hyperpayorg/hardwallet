//
//  JubSDKCore+DEV.mm
//  JubSDKCore
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore+DEV.h"
#import "JUB_SDK_DEV.h"

//typedef struct {
//    JUB_CHAR label[32];
//    JUB_CHAR sn[24];
//    JUB_UINT16 pinRetry;
//    JUB_UINT16 pinMaxRetry;
//    JUB_CHAR bleVersion[4];
//    JUB_CHAR firmwareVersion[4];
//} JUB_DEVICE_INFO;
@implementation DeviceInfo
@synthesize label;
@synthesize sn;
@synthesize pinRetry;
@synthesize pinMaxRetry;
@synthesize bleVersion;
@synthesize firmwareVersion;
@end

extern JUB_ENUM_BOOL    (^inlineBool)(JUB_NS_ENUM_BOOL);
extern JUB_NS_ENUM_BOOL (^inlineNSBool)(JUB_ENUM_BOOL);

@implementation JubSDKCore (DEV)

//JUB_RV JUB_GetDeviceInfo(IN JUB_UINT16 deviceID,
//                         OUT JUB_DEVICE_INFO_PTR info);
- (DeviceInfo*)JUB_GetDeviceInfo:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    JUB_DEVICE_INFO deviceInfo;
    JUB_RV rv = JUB_GetDeviceInfo(deviceID,
                                  &deviceInfo);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return (DeviceInfo*)[NSNull null];
    }
    
    DeviceInfo* info = [[DeviceInfo alloc] init];
    info.label = [NSString stringWithCString:deviceInfo.label
                                    encoding:NSUTF8StringEncoding];
    info.sn    = [NSString stringWithCString:deviceInfo.sn
                                    encoding:NSUTF8StringEncoding];
    info.pinRetry    = deviceInfo.pinRetry;
    info.pinMaxRetry = deviceInfo.pinMaxRetry;
    info.bleVersion  = [NSString stringWithCString:deviceInfo.bleVersion
                                          encoding:NSUTF8StringEncoding];
    info.firmwareVersion = [NSString stringWithCString:deviceInfo.firmwareVersion
                                              encoding:NSUTF8StringEncoding];
    
    return info;
}

//JUB_RV JUB_GetDeviceCert(IN JUB_UINT16 deviceID,
//                         OUT JUB_CHAR_PTR_PTR cert);
- (NSString*)JUB_GetDeviceCert:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    char *cert = nullptr;
    JUB_RV rv = JUB_GetDeviceCert(deviceID,
                                  &cert);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString *strCert = [NSString stringWithCString:cert
                                           encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(cert);
    
    return strCert;
}

//JUB_RV JUB_SendOneApdu(IN JUB_UINT16 deviceID,
//                       IN JUB_CHAR_PTR apdu ,
//                       OUT JUB_CHAR_PTR_PTR response);
- (NSString*)JUB_SendOneApdu:(NSUInteger)deviceID
                                  apdu:(NSString*)apdu
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR pApdu = nullptr;
    if (nil != apdu) {
        pApdu = (JUB_CHAR_PTR)[apdu UTF8String];
    }
    
    JUB_CHAR_PTR response = nullptr;
    JUB_RV rv = JUB_SendOneApdu(deviceID,
                                pApdu,
                                &response);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString *strResp = [NSString stringWithCString:response
                                           encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(response);
    
    return strResp;
}

//JUB_ENUM_BOOL JUB_IsInitialize(IN JUB_UINT16 deviceID);
- (JUB_NS_ENUM_BOOL)JUB_IsInitialize:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    JUB_ENUM_BOOL bRv = JUB_IsInitialize(deviceID);
    
    return inlineNSBool(bRv);
}

//JUB_ENUM_BOOL JUB_IsBootLoader(IN JUB_UINT16 deviceID);
- (JUB_NS_ENUM_BOOL)JUB_IsBootLoader:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    JUB_ENUM_BOOL bRv = JUB_IsBootLoader(deviceID);
    
    return inlineNSBool(bRv);
}

//JUB_RV JUB_SetTimeOut(IN JUB_UINT16 contextID, IN JUB_UINT16 timeout);
- (void)JUB_SetTimeOut:(NSUInteger)contextID
               timeout:(NSUInteger)timeout
{
    self.lastError = JUB_SetTimeOut(contextID,
                                    timeout);
}

////JUB_RV JUB_FreeMemory(IN JUB_CHAR_CPTR memPtr);
//- (void) JUB_FreeMemory :(NSString*)memPtr;

//JUB_RV JUB_EnumApplets(IN JUB_UINT16 deviceID,
//                       OUT JUB_CHAR_PTR_PTR appList);
- (NSString*)JUB_EnumApplets:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR appList = nullptr;
    JUB_RV rv = JUB_EnumApplets(deviceID,
                                &appList);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strAppList = [NSString stringWithCString:appList
                                              encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(appList);
    
    return strAppList;
}

//JUB_RV Jub_EnumSupportCoins(IN JUB_UINT16 deviceID,
//                            OUT JUB_CHAR_PTR_PTR coinList);
- (NSString*)Jub_EnumSupportCoins:(NSUInteger)deviceID
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR coinList = nullptr;
    JUB_RV rv = Jub_EnumSupportCoins(deviceID,
                                     &coinList);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strCoinList = [NSString stringWithCString:coinList
                                               encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(coinList);
    
    return strCoinList;
}

//JUB_RV JUB_GetAppletVersion(IN JUB_UINT16 deviceID,
//                            IN JUB_CHAR_PTR appID,
//                            OUT JUB_CHAR_PTR_PTR version);
- (NSString*)JUB_GetAppletVersion:(NSUInteger)deviceID
                            appID:(NSString*)appID
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR pAppID = nullptr;
    if (nil != appID) {
        pAppID = (JUB_CHAR_PTR)[appID UTF8String];
    }
    
    JUB_CHAR_PTR version = nullptr;
    JUB_RV rv = JUB_GetAppletVersion(deviceID,
                                     pAppID,
                                     &version);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strVersion = [NSString stringWithCString:version
                                              encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(version);
    
    return strVersion;
}

@end
