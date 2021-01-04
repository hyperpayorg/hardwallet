//
//  JubSDKCore.m
//  JubSDKCore
//
//  Created by Pan Min on 2019/6/22.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore.h"
#import "JUB_SDK.h"

//typedef struct {
//    JUB_ENUM_BOOL change;
//    JUB_UINT64    addressIndex;
//} BIP32_Path;
@implementation BIP32Path
@synthesize change;
@synthesize addressIndex;
@end

//typedef struct stContextCfg {
//    JUB_CHAR_PTR            main_path;
//} CONTEXT_CONFIG;
@implementation ContextConfig
@synthesize mainPath;
@end

bool (^inlinebool)(JUB_NS_ENUM_BOOL) = ^(JUB_NS_ENUM_BOOL argument) {
    bool b;
    switch (argument) {
        case BOOL_NS_FALSE:
            b = false;
            break;
        case BOOL_NS_TRUE:
            b = true;
            break;
        case BOOL_NS_NR_ITEMS:
            b = false;
            break;
    }
    return b;
};

JUB_NS_ENUM_BOOL (^inlineNSbool)(bool) = ^(bool argument) {
    JUB_NS_ENUM_BOOL b;
    if (argument) {
        b = BOOL_NS_TRUE;
    }
    else {
        b = BOOL_NS_FALSE;
    }
    return b;
};

JUB_ENUM_BOOL (^inlineBool)(JUB_NS_ENUM_BOOL) = ^(JUB_NS_ENUM_BOOL argument) {
    JUB_ENUM_BOOL b;
    switch (argument) {
        case BOOL_NS_FALSE:
            b = BOOL_FALSE;
            break;
        case BOOL_NS_TRUE:
            b = BOOL_TRUE;
            break;
        case BOOL_NS_NR_ITEMS:
            b = BOOL_NR_ITEMS;
            break;
    }
    return b;
};

JUB_NS_ENUM_BOOL (^inlineNSBool)(JUB_ENUM_BOOL) = ^(JUB_ENUM_BOOL argument) {
    JUB_NS_ENUM_BOOL b;
    switch (argument) {
        case BOOL_FALSE:
            b = BOOL_NS_FALSE;
            break;
        case BOOL_TRUE:
            b = BOOL_NS_TRUE;
            break;
        case BOOL_NR_ITEMS:
            b = BOOL_NS_NR_ITEMS;
            break;
    }
    return b;
};

JUB_ENUM_PUB_FORMAT (^inlinePubFormat)(JUB_NS_ENUM_PUB_FORMAT) = ^(JUB_NS_ENUM_PUB_FORMAT argument) {
    JUB_ENUM_PUB_FORMAT fmt;
    switch (argument) {
        case NS_HEX:
            fmt = HEX;
            break;
        case NS_XPUB:
            fmt = XPUB;
            break;
        default:
            fmt = PUB_FORMAT_NS_ITEM;
            break;
    }
    
    return fmt;
};

@implementation JubSDKCore
- (NSUInteger)JUB_LastError
{
    return _lastError;
}

//JUB_RV JUB_ClearContext(IN JUB_UINT16 contextID);
- (void)JUB_ClearContext:(NSUInteger)contextID
{
    _lastError = JUB_ClearContext(contextID);
}

//JUB_RV JUB_ShowVirtualPwd(IN JUB_UINT16 contextID);
- (void)JUB_ShowVirtualPwd:(NSUInteger)contextID
{
    _lastError = JUB_ShowVirtualPwd(contextID);
}

//JUB_RV JUB_CancelVirtualPwd(IN JUB_UINT16 contextID);
- (void)JUB_CancelVirtualPwd:(NSUInteger)contextID
{
    _lastError = JUB_CancelVirtualPwd(contextID);
}

//JUB_RV JUB_VerifyPIN(IN JUB_UINT16 contextID,
//                     IN JUB_CHAR_PTR pinMix,
//                     OUT JUB_ULONG_PTR retry);
- (NSUInteger)JUB_VerifyPIN:(NSUInteger)contextID
                     pinMix:(NSString*)pinMix
{
    _lastError = JUBR_OK;
    
    JUB_CHAR_PTR pPinMix = nullptr;
    if (nil != pinMix) {
        pPinMix = (JUB_CHAR_PTR)[pinMix UTF8String];
    }
    
    JUB_ULONG retry = 0;
    JUB_RV rv = JUB_VerifyPIN(contextID,
                              pPinMix,
                              &retry);
    if (JUBR_OK != rv) {
        _lastError = rv;
        return retry;
    }
    
    return retry;
}

////JUB_RV JUB_FreeMemory(IN JUB_CHAR_CPTR memPtr);
//- (void) JUB_FreeMemory :(NSString*)memPtr;

//JUB_CHAR_PTR JUB_GetVersion(void);
- (NSString*)JUB_GetVersion
{
    _lastError = JUBR_OK;
    
    return [NSString stringWithCString:JUB_GetVersion()
                              encoding:NSUTF8StringEncoding];
}

@end
