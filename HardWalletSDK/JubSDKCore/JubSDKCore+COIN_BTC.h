//
//  JubSDKCore+COIN_BTC.h
//  JubSDKCore
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore.h"

NS_ASSUME_NONNULL_BEGIN

//typedef enum {
//    COINBTC,
//    COINBCH,
//    COINLTC,
//    COINUSDT,
//    COINQTUM,
//    Default = COINBTC
//} JUB_ENUM_COINTYPE_BTC;
typedef NS_ENUM(NSInteger, JUB_NS_ENUM_COINTYPE_BTC) {
    NS_COINBTC,
    NS_COINBCH,
    NS_COINLTC,
    NS_COINUSDT,
    NS_COINQTUM,
    NS_Default = NS_COINBTC
};

//typedef enum {
//    p2pkh = 0,
//    //p2pwpkh,
//    p2sh_p2wpkh
//    p2sh_multisig,
//    p2wsh_multisig
//} JUB_BTC_TRANS_TYPE;
typedef NS_ENUM(NSInteger, JUB_NS_BTC_TRANS_TYPE) {
    ns_p2pkh = 0,
    ns_p2sh_p2wpkh,
    ns_p2sh_multisig,
    ns_p2wsh_multisig
};

//typedef enum {
//    P2PKH   = 0x00,
//    RETURN0 = 0x01,
//    P2SH_MULTISIG = 0x02,
//    NS_QRC20 = 0x03,
//} SCRIPT_BTC_TYPE;
typedef NS_ENUM(NSInteger, JUB_NS_SCRIPT_BTC_TYPE) {
    NS_P2PKH = 0x00,
    NS_RETURN0 = 0x01,
    NS_P2SH_MULTISIG = 0x02,
    NS_QRC20 = 0x03,
    NS_SCRIPT_BTC_TYPE_NS
};

//typedef enum {
//    BTC = 0x00,
//    cBTC,
//    mBTC,
//    uBTC,
//    Satoshi
//} JUB_BTC_UNIT_TYPE;
typedef NS_ENUM(NSInteger, JUB_NS_BTC_UNIT_TYPE) {
    NS_BTC = 0x00,
    NS_cBTC,
    NS_mBTC,
    NS_uBTC,
    NS_Satoshi,
    NS_BTC_UNIT_TYPE_NS
};

//typedef enum {
//    OWN = 0X00,
//    LEGACY,
//    DFLT = OWN
//} JUB_ENUM_BTC_ADDRESS_FORMAT;
typedef NS_ENUM(NSInteger, JUB_NS_ENUM_BTC_ADDRESS_FORMAT) {
    NS_OWN = 0x00,
    NS_LEGACY,
    NS_DFLT = NS_OWN
};

//typedef struct stInput {
//    SCRIPT_BTC_TYPE type;
//    JUB_CHAR_PTR    preHash;
//    JUB_UINT16      preIndex;
//    JUB_UINT32      nSequence;
//    JUB_UINT64      amount;
//    BIP32_Path      path;
//} INPUT_BTC;
@interface InputBTC : NSObject
@property (atomic, assign) JUB_NS_SCRIPT_BTC_TYPE type;
@property (atomic, copy  ) NSString* _Nonnull preHash;
@property (atomic, assign) NSInteger preIndex;
@property (atomic, assign) NSUInteger nSequence;
@property (atomic, assign) NSInteger amount;
@property (atomic, strong) BIP32Path* path;
@end

//typedef struct stOutput {
//    JUB_CHAR_PTR    address;
//    JUB_UINT64      amount;
//    JUB_ENUM_BOOL   change_address;
//    BIP32_Path      path;
//} OUTPUT;
@interface StdOutput : NSObject
@property (atomic, copy  ) NSString* _Nonnull address;
@property (atomic, assign) NSInteger amount;
@property (atomic, assign) JUB_NS_ENUM_BOOL isChangeAddress;
@property (atomic, strong) BIP32Path* path;
@end

//typedef struct {
//    JUB_UINT64      amount;
//    JUB_UINT16      data_len;
//    JUB_BYTE        data[40];
//} OUTPUT_RETURN0;
@interface OReturn0 : NSObject
@property (atomic, assign) NSInteger amount;
@property (atomic, copy  ) NSString* _Nonnull data;
@end

//typedef struct stOutputQRC20 {
//    JUB_UINT16      dataLen;
//    JUB_BYTE        data[200];
//} OUTPUT_QRC20;
@interface Qrc20 : NSObject
@property (atomic, copy  ) NSString* _Nonnull data;
@end

//typedef struct {
//    SCRIPT_BTC_TYPE type;
//    union {
//        OUTPUT stdOutput;
//        OUTPUT_RETURN0 return0;
//        OUTPUT_QRC20 qrc20;
//    };
//} OUTPUT_BTC;
//@interface StdOutput : NSObject
//@property (nonatomic, assign) JUB_NS_SCRIPT_BTC_TYPE type;
//@property (nonatomic, assign) StdOutput* stdOutput;
//@end
//
//@interface OutputReturn0 : NSObject
//@property (nonatomic, assign) JUB_NS_SCRIPT_BTC_TYPE type;
//@property (nonatomic, assign) OReturn0* output;
//@end
//@interface OutputQrc20 : NSObject
//@property (nonatomic, assign) JUB_NS_SCRIPT_BTC_TYPE type;
//@property (nonatomic, assign) Qrc20* qrc20;
//@end
@interface OutputBTC : NSObject
@property (atomic, assign) JUB_NS_SCRIPT_BTC_TYPE type;
@property (atomic, strong) StdOutput* stdOutput;
@property (atomic, strong) OReturn0* return0;
@property (atomic, strong) Qrc20* qrc20;
@end

//typedef struct stContextCfgBTC : stContextCfg {
//    JUB_ENUM_COINTYPE_BTC   coinType;// = { JUB_ENUM_COINTYPE_BTC::COINBTC };
//    JUB_BTC_TRANS_TYPE      transType;
//} CONTEXT_CONFIG_BTC;
@interface ContextConfigBTC : ContextConfig
@property (atomic, assign) JUB_NS_ENUM_COINTYPE_BTC coinType;
@property (atomic, assign) JUB_NS_BTC_TRANS_TYPE transType;
@end

//typedef struct stContextCfgMultisigBTC : stContextCfgBTC {
//    unsigned long m;
//    unsigned long n;
//    std::vector<std::string> vCosignerMainXpub;
//} CONTEXT_CONFIG_MULTISIG_BTC;
@interface ContextCfgMultisigBTC : ContextConfigBTC
@property (atomic, assign) NSUInteger m;
@property (atomic, assign) NSUInteger n;
@property (atomic, strong) NSMutableArray* vCosignerMainXpub;
@end

@interface TxBTC : NSObject
@property (atomic, strong) NSMutableArray* vInputs;
@property (atomic, strong) NSMutableArray* vOutputs;
@property (atomic, assign) NSUInteger locktime;
@end

@interface JubSDKCore (COIN_BTC)

//JUB_RV JUB_CreateContextBTC(IN CONTEXT_CONFIG_BTC cfg,
//                            IN JUB_UINT16 deviceID,
//                            OUT JUB_UINT16* contextID);
- (NSUInteger)JUB_CreateContextBTC:(NSUInteger)deviceID
                               cfg:(ContextConfigBTC*)cfg;

//JUB_RV JUB_GetHDNodeBTC(IN JUB_UINT16 contextID,
//                        IN BIP32_Path path,
//                        OUT JUB_CHAR_PTR_PTR xpub);
- (NSString*)JUB_GetHDNodeBTC:(NSUInteger)contextID
                         path:(BIP32Path*)path;

//JUB_RV JUB_GetMainHDNodeBTC(IN JUB_UINT16 contextID,
//                            OUT JUB_CHAR_PTR_PTR xpub);
- (NSString*)JUB_GetMainHDNodeBTC:(NSUInteger)contextID;


//JUB_RV JUB_GetAddressBTC(IN JUB_UINT16 contextID,
//                         IN JUB_ENUM_BTC_ADDRESS_FORMAT addrFmt,
//                         IN BIP32_Path path,
//                         IN JUB_ENUM_BOOL bShow,
//                         OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_GetAddressBTC:(NSUInteger)contextID
                       addrFmt:(JUB_NS_ENUM_BTC_ADDRESS_FORMAT)addrFmt
                          path:(BIP32Path*)path
                         bShow:(JUB_NS_ENUM_BOOL)bShow;
- (NSString*)JUB_GetAddressBTC:(NSUInteger)contextID
                          path:(BIP32Path*)path
                         bShow:(JUB_NS_ENUM_BOOL)bShow;

////JUB_RV JUB_GetP2SHMultisigAddressBTC(IN JUB_UINT16 contextID,
////                                     IN BIP32_Path path,
////                                     IN JUB_ENUM_BOOL bShow,
////                                     OUT JUB_CHAR_PTR_PTR address);
//- (NSString*)JUB_GetP2SHMultisigAddressBTC:(NSUInteger)contextID
//                                      path:(BIP32Path*)path
//                                     bShow:(JUB_NS_ENUM_BOOL)bShow;

//JUB_RV JUB_SetMyAddressBTC(IN JUB_UINT16 contextID,
//                           IN JUB_ENUM_BTC_ADDRESS_FORMAT addrFmt,
//                           IN BIP32_Path path,
//                           OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_SetMyAddressBTC:(NSUInteger)contextID
                         addrFmt:(JUB_NS_ENUM_BTC_ADDRESS_FORMAT)addrFmt
                            path:(BIP32Path*)path;
- (NSString*)JUB_SetMyAddressBTC:(NSUInteger)contextID
                            path:(BIP32Path*)path;

//JUB_RV JUB_SignTransactionBTC(IN JUB_UINT16 contextID,
//                              IN JUB_ENUM_BTC_ADDRESS_FORMAT addrFmt,
//                              IN INPUT_BTC inputs[], IN JUB_UINT16 iCount,
//                              IN OUTPUT_BTC outputs[], IN JUB_UINT16 oCount,
//                              IN JUB_UINT32 lockTime,
//                              OUT JUB_CHAR_PTR_PTR raw);
- (NSString*)JUB_SignTransactionBTC:(NSUInteger)contextID
                            addrFmt:(JUB_NS_ENUM_BTC_ADDRESS_FORMAT)addrFmt
                         inputArray:(NSArray*)inputArray
                        outputArray:(NSArray*)outputArray
                           lockTime:(NSUInteger)lockTime;
- (NSString*)JUB_SignTransactionBTC:(NSUInteger)contextID
                         inputArray:(NSArray*)inputArray
                        outputArray:(NSArray*)outputArray
                           lockTime:(NSUInteger)lockTime;

//JUB_RV JUB_ParseTransactionBTC(IN JUB_UINT16 contextID,
//                               IN JUB_CHAR_PTR incRaw,
//                               OUT std::vector<INPUT_BTC>& inputs,
//                               OUT std::vector<OUTPUT_BTC>& outputs,
//                               OUT JUB_UINT32_PTR plockTime);
- (TxBTC*)JUB_ParseTransactionBTC:(NSUInteger)contextID
                           incRaw:(NSString*)incRaw;

//JUB_RV JUB_SetUnitBTC(IN JUB_UINT16 contextID,
//                      IN JUB_BTC_UNIT_TYPE unit);
- (void)JUB_SetUnitBTC:(NSUInteger)contextID
                  unit:(JUB_NS_BTC_UNIT_TYPE)unit;

//JUB_RV JUB_BuildUSDTOutputs(IN JUB_UINT16 contextID,
//                            IN JUB_CHAR_PTR USDT_to,
//                            IN JUB_UINT64 amount,
//                            OUT OUTPUT_BTC outputs[2]);
- (NSArray*)JUB_BuildUSDTOutputs:(NSUInteger)contextID
                          USDTTo:(NSString*)USDTTo
                          amount:(NSUInteger)amount;

//JUB_RV JUB_BuildQRC20Outputs(IN JUB_UINT16 contextID,
//                             IN JUB_CHAR_PTR contractAddress, IN JUB_UINT8 decimal, IN JUB_CHAR_PTR symbol,
//                             IN JUB_UINT64 gasLimit, IN JUB_UINT64 gasPrice,
//                             IN JUB_CHAR_PTR to, IN JUB_CHAR_PTR value,
//                             OUT OUTPUT_BTC outputs[1]);
- (NSArray*)JUB_BuildQRC20Outputs:(NSUInteger)contextID
                     contractAddr:(NSString*)contractAddr
                          decimal:(NSUInteger)decimal
                           symbol:(NSString*)symbol
                         gasLimit:(NSUInteger)gasLimit
                         gasPrice:(NSUInteger)gasPrice
                               to:(NSString*)to
                            value:(NSString*)value;

@end

NS_ASSUME_NONNULL_END
