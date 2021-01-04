//
//  JubSDKCore+COIN_ETH.h
//  JubSDKCore
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore.h"

//typedef struct {
//    JUB_CHAR_PTR    main_path;
//    int             chainID;
//} CONTEXT_CONFIG_ETH;
@interface ContextConfigETH : NSObject
@property (atomic, copy  ) NSString* _Nonnull mainPath;
@property (atomic, assign) NSInteger chainID;
@end

NS_ASSUME_NONNULL_BEGIN

@interface JubSDKCore (COIN_ETH)

//JUB_RV JUB_CreateContextETH(IN CONTEXT_CONFIG_ETH cfg,
//                            IN JUB_UINT16 deviceID,
//                            OUT JUB_UINT16* contextID);
- (NSUInteger)JUB_CreateContextETH:(NSUInteger)deviceID
                               cfg:(ContextConfigETH*)cfg;

//JUB_RV JUB_GetAddressETH(IN JUB_UINT16 contextID,
//                         IN BIP32_Path path,
//                         IN JUB_ENUM_BOOL bShow,
//                         OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_GetAddressETH:(NSUInteger)contextID
                          path:(BIP32Path*)path
                         bShow:(JUB_NS_ENUM_BOOL)bShow;

//JUB_RV JUB_GetHDNodeETH(IN JUB_UINT16 contextID,
//                        IN JUB_ENUM_PUB_FORMAT format,
//                        IN BIP32_Path path,
//                        OUT JUB_CHAR_PTR_PTR pubkey);
- (NSString*)JUB_GetHDNodeETH:(NSUInteger)contextID
                       format:(JUB_NS_ENUM_PUB_FORMAT)format
                         path:(BIP32Path*)path;

//JUB_RV JUB_GetMainHDNodeETH(IN JUB_UINT16 contextID,
//                            IN JUB_ENUM_PUB_FORMAT format,
//                            OUT JUB_CHAR_PTR_PTR xpub);
- (NSString*)JUB_GetMainHDNodeETH:(NSUInteger)contextID
                           format:(JUB_NS_ENUM_PUB_FORMAT)format;

//JUB_RV JUB_SetMyAddressETH(IN JUB_UINT16 contextID,
//                           IN BIP32_Path path,
//                           OUT JUB_CHAR_PTR_PTR address);
- (NSString*)JUB_SetMyAddressETH:(NSUInteger)contextID
                            path:(BIP32Path*)path;

//JUB_RV JUB_SetContrAddrETH(IN JUB_UINT16 contextID,
//                           IN JUB_CHAR_CPTR contractAddress);
- (void)JUB_SetContrAddrETH:(NSUInteger)contextID
                  contrAddr:(NSString*)contractAddress;

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
                              input:(NSString*)input;

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
                       tokenValue:(NSString*)tokenValue;

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
                           input:(NSString*)input;

//JUB_RV JUB_BuildContractWithAddrAbiETH(IN JUB_UINT16 contextID,
//                                       IN JUB_CHAR_PTR methodID,
//                                       IN JUB_CHAR_PTR address,
//                                       OUT JUB_CHAR_PTR_PTR abi);
//- (NSString*)JUB_BuildContractWithAddrAbiETH:(NSUInteger)contextID
//                                    methodID:(NSString*)methodID
//                                     address:(NSString*)address;
//
//JUB_RV JUB_BuildContractWithAddrAmtAbiETH(IN JUB_UINT16 contextID,
//                                          IN JUB_CHAR_PTR methodID,
//                                          IN JUB_CHAR_PTR address,
//                                          IN JUB_CHAR_PTR amount,
//                                          OUT JUB_CHAR_PTR_PTR abi);
//- (NSString*)JUB_BuildContractWithAddrAmtAbiETH:(NSUInteger)contextID
//                                       methodID:(NSString*)methodID
//                                        address:(NSString*)address
//                                         amount:(NSString*)amount;
//
//JUB_RV JUB_BuildContractWithTxIDAbiETH(IN JUB_UINT16 contextID,
//                                       IN JUB_CHAR_PTR methodID,
//                                       IN JUB_CHAR_PTR transactionID,
//                                       OUT JUB_CHAR_PTR_PTR abi);
- (NSString*)JUB_BuildContractWithTxIDAbiETH:(NSUInteger)contextID
                                    methodID:(NSString*)methodID
                               transactionID:(NSString*)transactionID;

//JUB_RV JUB_BuildContractWithAmtAbiETH(IN JUB_UINT16 contextID,
//                                      IN JUB_CHAR_PTR methodID,
//                                      IN JUB_CHAR_PTR amount,
//                                      OUT JUB_CHAR_PTR_PTR abi);
//- (NSString*)JUB_BuildContractWithTxIDAbiETH:(NSUInteger)contextID
//                                    methodID:(NSString*)methodID
//                                      amount:(NSString*)amount;

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
                                               data:(NSString*)data;

@end

NS_ASSUME_NONNULL_END
