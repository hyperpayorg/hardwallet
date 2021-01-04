//
//  JubSDKCore.h
//  JubSDKCore
//
//  Created by Pan Min on 2019/6/22.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import <Foundation/Foundation.h>

#define JUBR_OK                     0x00000000UL

#define JUBR_ERROR                  0x00000001UL
#define JUBR_HOST_MEMORY            0x00000002UL
#define JUBR_ARGUMENTS_BAD          0x00000003UL
#define JUBR_IMPL_NOT_SUPPORT       0x00000004UL
#define JUBR_MEMORY_NULL_PTR        0x00000005UL

#define JUBR_INVALID_MEMORY_PTR     0x00000008UL
#define JUBR_REPEAT_MEMORY_PTR      0x00000009UL
#define JUBR_BUFFER_TOO_SMALL       0x0000000AUL

#define JUBR_MULTISIG_OK            0x0000000BUL

//typedef enum {
//    BOOL_FALSE = 0,
//    BOOL_TRUE,
//    BOOL_NR_ITEMS
//} JUB_ENUM_BOOL;
typedef NS_ENUM(NSInteger, JUB_NS_ENUM_BOOL) {
    BOOL_NS_FALSE = 0,
    BOOL_NS_TRUE,
    BOOL_NS_NR_ITEMS
};

//typedef enum {
//    HEX  = 0x00,
//    XPUB = 0x01
//} JUB_ENUM_PUB_FORMAT;
typedef NS_ENUM(NSInteger, JUB_NS_ENUM_PUB_FORMAT) {
    NS_HEX  = 0x00,
    NS_XPUB = 0x01
};

//typedef struct {
//    JUB_ENUM_BOOL change;
//    JUB_UINT64    addressIndex;
//} BIP32_Path;
@interface BIP32Path : NSObject
@property (atomic, assign) JUB_NS_ENUM_BOOL change;
@property (atomic, assign) NSInteger addressIndex;
@end

//typedef struct stContextCfg {
//    JUB_CHAR_PTR            main_path;
//} CONTEXT_CONFIG;
@interface ContextConfig : NSObject
@property (atomic, copy  ) NSString* mainPath;
@end

@interface JubSDKCore : NSObject
@property (atomic, assign) NSUInteger lastError;

- (NSUInteger)JUB_LastError;

//JUB_RV JUB_ClearContext(IN JUB_UINT16 contextID);
- (void)JUB_ClearContext:(NSUInteger)contextID;

//JUB_RV JUB_ShowVirtualPwd(IN JUB_UINT16 contextID);
- (void)JUB_ShowVirtualPwd:(NSUInteger)contextID;

//JUB_RV JUB_CancelVirtualPwd(IN JUB_UINT16 contextID);
- (void)JUB_CancelVirtualPwd:(NSUInteger)contextID;

//JUB_RV JUB_VerifyPIN(IN JUB_UINT16 contextID,
//                     IN JUB_CHAR_PTR pinMix,
//                     OUT JUB_ULONG_PTR retry);
- (NSUInteger)JUB_VerifyPIN:(NSUInteger)contextID
                     pinMix:(NSString*)pinMix;

//JUB_CHAR_PTR JUB_GetVersion(void);
- (NSString*)JUB_GetVersion;

@end
