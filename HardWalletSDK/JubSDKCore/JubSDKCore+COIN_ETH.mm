//
//  JubSDKCore+COIN_ETH.mm
//  JubSDKCore
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore+COIN_ETH.h"
#import "JUB_SDK_ETH.h"

//typedef struct {
//    JUB_CHAR_PTR        main_path;
//    int                 chainID;
//} CONTEXT_CONFIG_ETH;
@implementation ContextConfigETH
@synthesize mainPath;
@synthesize chainID;
@end

extern JUB_ENUM_BOOL (^inlineBool)(JUB_NS_ENUM_BOOL);
extern JUB_ENUM_PUB_FORMAT (^inlinePubFormat)(JUB_NS_ENUM_PUB_FORMAT);

@implementation JubSDKCore (COIN_ETH)

//JUB_RV JUB_CreateContextETH(IN CONTEXT_CONFIG_ETH cfg,
//                            IN JUB_UINT16 deviceID,
//                            OUT JUB_UINT16* contextID);
- (NSUInteger)JUB_CreateContextETH:(NSUInteger)deviceID
                               cfg:(ContextConfigETH*)cfg
{
    self.lastError = JUBR_OK;
    
    CONTEXT_CONFIG_ETH cfgETH;
    if (nil != cfg.mainPath) {
        cfgETH.mainPath = (JUB_CHAR_PTR)[cfg.mainPath UTF8String];
    }
    cfgETH.chainID = (int)cfg.chainID;
    JUB_UINT16 contextID = 0;
    JUB_RV rv = JUB_CreateContextETH(cfgETH,
                                     deviceID,
                                     &contextID);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return contextID;
    }
    
    return contextID;
}

//JUB_RV JUB_GetAddressETH(IN JUB_UINT16 contextID,
//                         IN BIP32_Path path,
//                         IN JUB_ENUM_BOOL bShow,
//                         OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_GetAddressETH:(NSUInteger)contextID
                          path:(BIP32Path*)path
                         bShow:(JUB_NS_ENUM_BOOL)bShow
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_ENUM_BOOL isShow = inlineBool(bShow);
    JUB_CHAR_PTR address;
    JUB_RV rv = JUB_GetAddressETH(contextID,
                                  bip32Path,
                                  isShow,
                                  &address);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strAddress = [NSString stringWithCString:address
                                              encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(address);
    
    return strAddress;
}

//JUB_RV JUB_GetHDNodeETH(IN JUB_UINT16 contextID,
//                        IN JUB_ENUM_PUB_FORMAT format,
//                        IN BIP32_Path path,
//                        OUT JUB_CHAR_PTR_PTR pubkey);
- (NSString*)JUB_GetHDNodeETH:(NSUInteger)contextID
                       format:(JUB_NS_ENUM_PUB_FORMAT)format
                         path:(BIP32Path*)path
{
    self.lastError = JUBR_OK;
    
    JUB_ENUM_PUB_FORMAT fmt = inlinePubFormat(format);
    if (PUB_FORMAT_NS_ITEM == fmt) {
        self.lastError = JUBR_ARGUMENTS_BAD;
        return @"";
    }
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR pubkey = nullptr;
    JUB_RV rv = JUB_GetHDNodeETH(contextID,
                                 fmt,
                                 bip32Path,
                                 &pubkey);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strPubkey = [NSString stringWithCString:pubkey
                                             encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(pubkey);
    
    return strPubkey;
}

//JUB_RV JUB_GetMainHDNodeETH(IN JUB_UINT16 contextID,
//                            IN JUB_ENUM_PUB_FORMAT format,
//                            OUT JUB_CHAR_PTR_PTR xpub);
- (NSString*)JUB_GetMainHDNodeETH:(NSUInteger)contextID
                           format:(JUB_NS_ENUM_PUB_FORMAT)format
{
    self.lastError = JUBR_OK;
    
    JUB_ENUM_PUB_FORMAT fmt = inlinePubFormat(format);
    if (PUB_FORMAT_NS_ITEM == fmt) {
        self.lastError = JUBR_ARGUMENTS_BAD;
        return @"";
    }
    JUB_CHAR_PTR xpub = nullptr;
    JUB_RV rv = JUB_GetMainHDNodeETH(contextID,
                                     fmt,
                                     &xpub);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strXpub = [NSString stringWithCString:xpub
                                           encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(xpub);
    
    return strXpub;
}

//JUB_RV JUB_SetMyAddressETH(IN JUB_UINT16 contextID,
//                           IN BIP32_Path path,
//                           OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_SetMyAddressETH:(NSUInteger)contextID
                            path:(BIP32Path*)path
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR address;
    JUB_RV rv = JUB_SetMyAddressETH(contextID,
                                    bip32Path,
                                    &address);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strAddress = [NSString stringWithCString:address
                                              encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(address);
    
    return strAddress;
}

//JUB_RV JUB_SetContrAddrETH(IN JUB_UINT16 contextID,
//                           IN JUB_CHAR_CPTR contractAddress);
- (void)JUB_SetContrAddrETH:(NSUInteger)contextID
                  contrAddr:(NSString*)contractAddress
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR pContractAddress = nullptr;
    if (nil != contractAddress) {
        pContractAddress = (JUB_CHAR_PTR)[contractAddress UTF8String];
    }
    
    self.lastError = JUB_SetContrAddrETH(contextID, pContractAddress);
}

//JUB_RV JUB_SignTransactionETH(IN JUB_UINT16 contextID,
//                              IN BIP32_Path path,
//                              IN JUB_UINT32 nonce,
//                              IN JUB_UINT32 gasLimit,
//                              IN JUB_CHAR_PTR gasPriceInWei,
//                              IN JUB_CHAR_PTR to,
//                              IN JUB_CHAR_PTR valueInWei,
//                              IN JUB_CHAR_PTR input,
//                              OUT JUB_CHAR_PTR_PTR raw);
- (NSString*)JUB_SignTransactionETH:(NSUInteger)contextID
                               path:(BIP32Path*)path
                              nonce:(NSUInteger)nonce
                           gasLimit:(NSUInteger)gasLimit
                      gasPriceInWei:(NSString*)gasPriceInWei
                                 to:(NSString*)to
                         valueInWei:(NSString*)valueInWei
                              input:(NSString*)input
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR vInWei = nullptr;
    if (NSOrderedSame != [valueInWei compare:@""]
        &&        nil !=  valueInWei
        ) {
        vInWei = (JUB_CHAR_PTR)[valueInWei UTF8String];
    }
    JUB_CHAR_PTR pGasPriceInWei = nullptr;
    if (nil != gasPriceInWei) {
        pGasPriceInWei = (JUB_CHAR_PTR)[gasPriceInWei UTF8String];
    }
    JUB_CHAR_PTR pTo = nullptr;
    if (nil != to) {
        pTo = (JUB_CHAR_PTR)[to UTF8String];
    }
    JUB_CHAR_PTR pInput = nullptr;
    if (nil != input) {
        pInput = (JUB_CHAR_PTR)[input UTF8String];
    }
    
    JUB_CHAR_PTR raw = nullptr;
    JUB_RV rv = JUB_SignTransactionETH(contextID,
                                       bip32Path,
                                       (JUB_UINT32)nonce,
                                       (JUB_UINT32)gasLimit,
                                       pGasPriceInWei,
                                       pTo,
                                       vInWei,
                                       pInput,
                                       &raw);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strRaw = [NSString stringWithCString:raw
                                          encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(raw);
    
    return strRaw;
}

//JUB_RV JUB_BuildERC20AbiETH(IN JUB_UINT16 contextID,
//                            IN JUB_CHAR_PTR tokenName,
//                            IN JUB_UINT16 unitDP,
//                            IN JUB_CHAR_PTR contractAddress,
//                            IN JUB_CHAR_PTR tokenTo, IN JUB_CHAR_PTR tokenValue,
//                            OUT JUB_CHAR_PTR_PTR abi);
- (NSString*)JUB_BuildERC20AbiETH:(NSUInteger)contextID
                        tokenName:(NSString*)tokenName
                           unitDP:(NSUInteger)unitDP
                  contractAddress:(NSString*)contractAddress
                          tokenTo:(NSString*)tokenTo
                       tokenValue:(NSString*)tokenValue
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR pTokenName = nullptr;
    if (nil != tokenName) {
        pTokenName = (JUB_CHAR_PTR)[tokenName UTF8String];
    }
    JUB_CHAR_PTR pContractAddress = nullptr;
    if (nil != contractAddress) {
        pContractAddress = (JUB_CHAR_PTR)[contractAddress UTF8String];
    }
    JUB_CHAR_PTR pTokenTo = nullptr;
    if (nil != tokenTo) {
        pTokenTo = (JUB_CHAR_PTR)[tokenTo UTF8String];
    }
    JUB_CHAR_PTR pTokenValue = nullptr;
    if (nil != tokenValue) {
        pTokenValue = (JUB_CHAR_PTR)[tokenValue UTF8String];
    }
    
    JUB_CHAR_PTR abi = nullptr;
    JUB_RV rv = JUB_BuildERC20AbiETH(contextID,
                                     pTokenName,
                                     unitDP,
                                     pContractAddress,
                                     pTokenTo,
                                     pTokenValue,
                                     &abi);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strAbi = [NSString stringWithCString:abi
                                          encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(abi);
    
    return strAbi;
}

//JUB_RV JUB_SignContractETH(IN JUB_UINT16 contextID,
//                           IN BIP32_Path path,
//                           IN JUB_UINT32 nonce,
//                           IN JUB_UINT32 gasLimit,
//                           IN JUB_CHAR_PTR gasPriceInWei,
//                           IN JUB_CHAR_PTR to,
//                           IN JUB_CHAR_PTR valueInWei,
//                           IN JUB_CHAR_PTR input,
//                           OUT JUB_CHAR_PTR_PTR raw);
- (NSString*)JUB_SignContractETH:(NSUInteger)contextID
                            path:(BIP32Path*)path
                           nonce:(NSUInteger)nonce
                        gasLimit:(NSUInteger)gasLimit
                   gasPriceInWei:(NSString*)gasPriceInWei
                              to:(NSString*)to
                      valueInWei:(NSString*)valueInWei
                           input:(NSString*)input
{
    self.lastError = JUBR_OK;
    
    BIP32_Path bip32Path;
    bip32Path.change = inlineBool(path.change);
    bip32Path.addressIndex = path.addressIndex;
    JUB_CHAR_PTR vInWei = nullptr;
    if (NSOrderedSame != [valueInWei compare:@""]
        &&        nil !=  valueInWei
        ) {
        vInWei = (JUB_CHAR_PTR)[valueInWei UTF8String];
    }
    
    JUB_CHAR_PTR pGasPriceInWei = nullptr;
    if (nil != gasPriceInWei) {
        pGasPriceInWei = (JUB_CHAR_PTR)[gasPriceInWei UTF8String];
    }
    JUB_CHAR_PTR pTo = nullptr;
    if (nil != to) {
        pTo = (JUB_CHAR_PTR)[to UTF8String];
    }
    JUB_CHAR_PTR pInput = nullptr;
    if (nil != input) {
        pInput = (JUB_CHAR_PTR)[input UTF8String];
    }
    
    JUB_CHAR_PTR raw = nullptr;
    JUB_RV rv = JUB_SignContractETH(contextID,
                                    bip32Path,
                                    (JUB_UINT32)nonce,
                                    (JUB_UINT32)gasLimit,
                                    pGasPriceInWei,
                                    pTo,
                                    vInWei,
                                    pInput,
                                    &raw);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strRaw = [NSString stringWithCString:raw
                                          encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(raw);
    
    return strRaw;
}

//JUB_RV JUB_BuildContractWithAddrAbiETH(IN JUB_UINT16 contextID,
//                                       IN JUB_CHAR_PTR methodID,
//                                       IN JUB_CHAR_PTR address,
//                                       OUT JUB_CHAR_PTR_PTR abi);
//- (NSString*)JUB_BuildContractWithAddrAbiETH:(NSUInteger)contextID
//                                    methodID:(NSString*)methodID
//                                     address:(NSString*)address
//{
//    self.lastError = JUBR_OK;
//    
//    JUB_CHAR_PTR pMethodID = nullptr;
//    if (nil != methodID) {
//        pMethodID = (JUB_CHAR_PTR)[methodID UTF8String];
//    }
//    
//    JUB_CHAR_PTR pAddress = nullptr;
//    if (nil != address) {
//        pAddress = (JUB_CHAR_PTR)[address UTF8String];
//    }
//    
//    JUB_CHAR_PTR abi = nullptr;
//    JUB_RV rv = JUB_BuildContractWithAddrAbiETH(contextID,
//                                                pMethodID,
//                                                pAddress,
//                                                &abi);
//    if (JUBR_OK != rv) {
//        self.lastError = rv;
//        return @"";
//    }
//    
//    NSString* strAbi = [NSString stringWithCString:abi
//                                          encoding:NSUTF8StringEncoding];
//    JUB_FreeMemory(abi);
//    
//    return strAbi;
//}
//
//JUB_RV JUB_BuildContractWithAddrAmtAbiETH(IN JUB_UINT16 contextID,
//                                          IN JUB_CHAR_PTR methodID,
//                                          IN JUB_CHAR_PTR address,
//                                          IN JUB_CHAR_PTR amount,
//                                          OUT JUB_CHAR_PTR_PTR abi);
//- (NSString*)JUB_BuildContractWithAddrAmtAbiETH:(NSUInteger)contextID
//                                       methodID:(NSString*)methodID
//                                        address:(NSString*)address
//                                         amount:(NSString*)amount
//{
//    self.lastError = JUBR_OK;
//    
//    JUB_CHAR_PTR pMethodID = nullptr;
//    if (nil != methodID) {
//        pMethodID = (JUB_CHAR_PTR)[methodID UTF8String];
//    }
//    
//    JUB_CHAR_PTR pAddress = nullptr;
//    if (nil != address) {
//        pAddress = (JUB_CHAR_PTR)[address UTF8String];
//    }
//    
//    JUB_CHAR_PTR pAmount = nullptr;
//    if (nil != amount) {
//        pAmount = (JUB_CHAR_PTR)[amount UTF8String];
//    }
//    
//    JUB_CHAR_PTR abi = nullptr;
//    JUB_RV rv = JUB_BuildContractWithAddrAmtAbiETH(contextID,
//                                                   pMethodID,
//                                                   pAddress,
//                                                   pAmount,
//                                                   &abi);
//    if (JUBR_OK != rv) {
//        self.lastError = rv;
//        return @"";
//    }
//    
//    NSString* strAbi = [NSString stringWithCString:abi
//                                          encoding:NSUTF8StringEncoding];
//    JUB_FreeMemory(abi);
//    
//    return strAbi;
//}
//
//JUB_RV JUB_BuildContractWithTxIDAbiETH(IN JUB_UINT16 contextID,
//                                       IN JUB_CHAR_PTR methodID,
//                                       IN JUB_CHAR_PTR transactionID,
//                                       OUT JUB_CHAR_PTR_PTR abi);
- (NSString*)JUB_BuildContractWithTxIDAbiETH:(NSUInteger)contextID
                                    methodID:(NSString*)methodID
                               transactionID:(NSString*)transactionID
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR pMethodID = nullptr;
    if (nil != methodID) {
        pMethodID = (JUB_CHAR_PTR)[methodID UTF8String];
    }
    
    JUB_CHAR_PTR pTransactionID = nullptr;
    if (nil != transactionID) {
        pTransactionID = (JUB_CHAR_PTR)[transactionID UTF8String];
    }
    
    JUB_CHAR_PTR abi = nullptr;
    JUB_RV rv = JUB_BuildContractWithTxIDAbiETH(contextID,
                                                pMethodID,
                                                pTransactionID,
                                                &abi);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strAbi = [NSString stringWithCString:abi
                                          encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(abi);
    
    return strAbi;
}

//JUB_RV JUB_BuildContractWithAmtAbiETH(IN JUB_UINT16 contextID,
//                                      IN JUB_CHAR_PTR methodID,
//                                      IN JUB_CHAR_PTR amount,
//                                      OUT JUB_CHAR_PTR_PTR abi);
//- (NSString*)JUB_BuildContractWithTxIDAbiETH:(NSUInteger)contextID
//                                    methodID:(NSString*)methodID
//                                      amount:(NSString*)amount
//{
//    self.lastError = JUBR_OK;
//    
//    JUB_CHAR_PTR pMethodID = nullptr;
//    if (nil != methodID) {
//        pMethodID = (JUB_CHAR_PTR)[methodID UTF8String];
//    }
//    
//    JUB_CHAR_PTR pAmount = nullptr;
//    if (nil != amount) {
//        pAmount = (JUB_CHAR_PTR)[amount UTF8String];
//    }
//    
//    JUB_CHAR_PTR abi = nullptr;
//    JUB_RV rv = JUB_BuildContractWithAmtAbiETH(contextID,
//                                               pMethodID,
//                                               pAmount,
//                                               &abi);
//    if (JUBR_OK != rv) {
//        self.lastError = rv;
//        return @"";
//    }
//    
//    NSString* strAbi = [NSString stringWithCString:abi
//                                          encoding:NSUTF8StringEncoding];
//    JUB_FreeMemory(abi);
//    
//    return strAbi;
//}

//JUB_RV JUB_BuildContractWithAddrAmtDataAbiETH(IN JUB_UINT16 contextID,
//                                              IN JUB_CHAR_PTR methodID,
//                                              IN JUB_CHAR_PTR address,
//                                              IN JUB_CHAR_PTR amount,
//                                              IN JUB_CHAR_PTR data,
//                                              OUT JUB_CHAR_PTR_PTR abi);
- (NSString*)JUB_BuildContractWithAddrAmtDataAbiETH:(NSUInteger)contextID
                                           methodID:(NSString*)methodID
                                            address:(NSString*)address
                                             amount:(NSString*)amount
                                               data:(NSString*)data
{
    self.lastError = JUBR_OK;
    
    JUB_CHAR_PTR pMethodID = nullptr;
    if (nil != methodID) {
        pMethodID = (JUB_CHAR_PTR)[methodID UTF8String];
    }
    
    JUB_CHAR_PTR pAddress = nullptr;
    if (nil != address) {
        pAddress = (JUB_CHAR_PTR)[address UTF8String];
    }
    
    JUB_CHAR_PTR pAmount = nullptr;
    if (nil != amount) {
        pAmount = (JUB_CHAR_PTR)[amount UTF8String];
    }
    
    JUB_CHAR_PTR pData = nullptr;
    if (nil != data) {
        pData = (JUB_CHAR_PTR)[data UTF8String];
    }
    
    JUB_CHAR_PTR abi = nullptr;
    JUB_RV rv = JUB_BuildContractWithAddrAmtDataAbiETH(contextID,
                                                       pMethodID,
                                                       pAddress,
                                                       pAmount,
                                                       pData,
                                                       &abi);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @"";
    }
    
    NSString* strAbi = [NSString stringWithCString:abi
                                          encoding:NSUTF8StringEncoding];
    JUB_FreeMemory(abi);
    
    return strAbi;
}

@end
