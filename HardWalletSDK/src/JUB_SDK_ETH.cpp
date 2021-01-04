//
//  JUB_SDK_ETH.cpp
//  JubSDK
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#include "JUB_SDK_ETH.h"

#include "utility/util.h"

#include "context/ContextETH.h"
#include "token/interface/TokenInterface.hpp"

JUB_RV _allocMem(JUB_CHAR_PTR_PTR memPtr, const std::string &strBuf);

stContextCfgETH::stContextCfgETH() {
    chainID = 0;
}

/*****************************************************************************
 * @function name : JUB_CreateContextETH
 * @in  param : cfg
 *          : deviceID - device ID
 * @out param : contextID
 * @last change :
 *****************************************************************************/
JUB_RV JUB_CreateContextETH(IN CONTEXT_CONFIG_ETH cfg,
                            IN JUB_UINT16 deviceID,
                            OUT JUB_UINT16* contextID) {

    if (nullptr == jub::TokenManager::GetInstance()->GetOne(deviceID)) {
        JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
    }

    jub::ContextETH* context = new jub::ContextETH(cfg, deviceID);
    *contextID = jub::ContextManager::GetInstance()->AddOne(context);
    context->ActiveSelf();

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_GetAddressETH
 * @in  param : contextID - context ID
 *          : path
 *          : bShow
 * @out param : address
 * @last change :
 *****************************************************************************/
JUB_RV JUB_GetAddressETH(IN JUB_UINT16 contextID,
                         IN BIP32_Path    path,
                         IN JUB_ENUM_BOOL bShow,
                         OUT JUB_CHAR_PTR_PTR address) {

    JUB_CHECK_CONTEXT_ETH(contextID);

    auto context = (jub::ContextETH*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_address;
    JUB_VERIFY_RV(context->GetAddress(path, bShow, str_address));
    JUB_VERIFY_RV(_allocMem(address, str_address));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_SetMyAddressETH
 * @in  param : contextID - context ID
 *          : path
 * @out param : address
 * @last change :
 *****************************************************************************/
JUB_RV JUB_SetMyAddressETH(IN JUB_UINT16 contextID,
                           IN BIP32_Path path,
                           OUT JUB_CHAR_PTR_PTR address) {

    JUB_CHECK_CONTEXT_ETH(contextID);

    auto context = (jub::ContextETH*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_address;
    JUB_VERIFY_RV(context->SetMyAddress(path, str_address));
    JUB_VERIFY_RV(_allocMem(address, str_address));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_GetHDNodeETH
 * @in  param : contextID - context ID
 *          : format - JUB_ENUM_PUB_FORMAT::HEX (0x00) for hex;
 *                 JUB_ENUM_PUB_FORMAT::XPUB(0x01) for xpub
 *          : path
 * @out param : pubkey
 * @last change :
 *****************************************************************************/
JUB_RV JUB_GetHDNodeETH(IN JUB_UINT16 contextID,
                        IN JUB_ENUM_PUB_FORMAT format,
                        IN BIP32_Path path,
                        OUT JUB_CHAR_PTR_PTR pubkey) {

    JUB_CHECK_CONTEXT_ETH(contextID);

    auto context = (jub::ContextETH*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_pubkey;
    JUB_VERIFY_RV(context->GetHDNode(format,path, str_pubkey));
    JUB_VERIFY_RV(_allocMem(pubkey, str_pubkey));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_GetMainHDNodeETH
 * @in  param : contextID - context ID
 *          : format - JUB_ENUM_PUB_FORMAT::HEX (0x00) for hex;
 *                 JUB_ENUM_PUB_FORMAT::XPUB(0x01) for xpub
 * @out param : xpub
 * @last change :
 *****************************************************************************/
JUB_RV JUB_GetMainHDNodeETH(IN JUB_UINT16 contextID,
                            IN JUB_ENUM_PUB_FORMAT format,
                            OUT JUB_CHAR_PTR_PTR xpub) {

    JUB_CHECK_CONTEXT_ETH(contextID);

    auto context = (jub::ContextETH*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_xpub;
    JUB_VERIFY_RV(context->GetMainHDNode(format, str_xpub));
    JUB_VERIFY_RV(_allocMem(xpub, str_xpub));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_SignTransactionETH
 * @in  param : contextID - context ID
 *          : path
 *          : nonce - nonce
 *          : gasLimit - gas limit
 *          : gasPriceInWei - gas price in wei
 *          : to
 *          : valueInWei - value in wei
 *          : input
 * @out param : raw
 * @last change :
 *****************************************************************************/
JUB_RV JUB_SignTransactionETH(IN JUB_UINT16 contextID,
                              IN BIP32_Path path,
                              IN JUB_UINT32 nonce,
                              IN JUB_UINT32 gasLimit,
                              IN JUB_CHAR_PTR gasPriceInWei,
                              IN JUB_CHAR_PTR to,
                              IN JUB_CHAR_PTR valueInWei,
                              IN JUB_CHAR_PTR input,
                              OUT JUB_CHAR_PTR_PTR raw) {

    JUB_CHECK_CONTEXT_ETH(contextID);

    JUB_CHECK_NULL(gasPriceInWei);
    JUB_CHECK_NULL(to);
//    JUB_CHECK_NULL(valueInWei);// it can be nullptr
    JUB_CHECK_NULL(input);

    auto context = (jub::ContextETH*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_raw;
    JUB_VERIFY_RV(context->SignTransaction(path,
                                           nonce,
                                           gasLimit,
                                           gasPriceInWei,
                                           to,
                                           valueInWei,
                                           input,
                                           str_raw));
    JUB_VERIFY_RV(_allocMem(raw, str_raw));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_BuildERC20AbiETH
 * @in  param : contextID - context ID
 *          : tokenName - ETH token name
 *          : unitDP - unit decimal place
 *          : contractAddress - contract address
 *          : tokenTo - token to
 *          : tokenValue - value for token transaction
 * @out param : abi
 * @last change :
 *****************************************************************************/
JUB_RV JUB_BuildERC20AbiETH(IN JUB_UINT16 contextID,
                            IN JUB_CHAR_CPTR tokenName,
                            IN JUB_UINT16 unitDP,
                            IN JUB_CHAR_CPTR contractAddress,
                            IN JUB_CHAR_CPTR tokenTo,
                            IN JUB_CHAR_CPTR tokenValue,
                            OUT JUB_CHAR_PTR_PTR abi) {

    JUB_CHECK_CONTEXT_ETH(contextID);

    auto context = (jub::ContextETH*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    JUB_VERIFY_RV(context->SetERC20ETHToken(tokenName, unitDP, contractAddress));

    std::string strAbi;
    JUB_VERIFY_RV(context->BuildERC20Abi(tokenTo, tokenValue, strAbi));
    JUB_VERIFY_RV(_allocMem(abi, strAbi));

    return JUBR_OK;
}

/*****************************************************************************
* @function name : JUB_SetContrAddrETH
* @in  param : contextID - context ID
*            : contractAddress - contract address
* @out param :
* @last change :
*****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SetContrAddrETH(IN JUB_UINT16 contextID,
                           IN JUB_CHAR_CPTR contractAddress) {

    JUB_CHECK_CONTEXT_ETH(contextID);

    auto context = (jub::ContextETH*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    JUB_VERIFY_RV(context->SetERC20ETHToken("", 0, contractAddress));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_SignContractETH
 * @in  param : contextID - context ID
 *          : path
 *          : nonce - nonce
 *          : gasLimit - gas limit
 *          : gasPriceInWei - gas price in wei
 *          : to
 *          : valueInWei - value in wei
 *          : input
 * @out param : raw
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SignContractETH(IN JUB_UINT16 contextID,
                           IN BIP32_Path path,
                           IN JUB_UINT32 nonce,
                           IN JUB_UINT32 gasLimit,
                           IN JUB_CHAR_PTR gasPriceInWei,
                           IN JUB_CHAR_PTR to,
                           IN JUB_CHAR_PTR valueInWei,
                           IN JUB_CHAR_PTR input,
                           OUT JUB_CHAR_PTR_PTR raw) {

    JUB_CHECK_CONTEXT_ETH(contextID);

    JUB_CHECK_NULL(gasPriceInWei);
//    JUB_CHECK_NULL(to);// it can be nullptr
    // for ETH create contract, no to arg
    std::string strTo = "";
    if (nullptr != to) {
        strTo = std::string(to);
    }
//    JUB_CHECK_NULL(valueInWei);// it can be nullptr
    JUB_CHECK_NULL(input);

    auto context = (jub::ContextETH*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_raw;
    JUB_VERIFY_RV(context->SignContract(path,
                                        nonce,
                                        gasLimit,
                                        gasPriceInWei,
                                        strTo,
                                        valueInWei,
                                        input,
                                        str_raw));
    JUB_VERIFY_RV(_allocMem(raw, str_raw));

    return JUBR_OK;
}

///*****************************************************************************
//* @function name : JUB_BuildContractWithAddrAbiETH
//* @in  param : contextID - context ID
//*                     : methodID - contract methodID
//*                     : address - The address at which you need to operate
//* @out param : specified contract abi
//* @last change :
//*****************************************************************************/
//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_BuildContractWithAddrAbiETH(IN JUB_UINT16 contextID,
//                                       IN JUB_CHAR_PTR methodID,
//                                       IN JUB_CHAR_PTR address,
//                                       OUT JUB_CHAR_PTR_PTR abi) {
//
//    JUB_CHECK_CONTEXT_ETH(contextID);
//
//    auto context = (jub::ContextETH*)jub::ContextManager::GetInstance()->GetOne(contextID);
//    JUB_CHECK_NULL(context);
//
//    std::string strAbi;
//    JUB_VERIFY_RV(context->BuildContractWithAddrAbi(methodID, address, strAbi));
//    JUB_VERIFY_RV(_allocMem(abi, strAbi));
//
//    return JUBR_OK;
//}
//
///*****************************************************************************
//* @function name : JUB_BuildContractWithAddrAmtAbiETH
//* @in  param : contextID - context ID
//*                     : methodID - contract methodID
//*                     : address - The address at which you need to operate
//*                     : amount - The amount at which you need to operate
//* @out param : specified contract abi
//* @last change :
//*****************************************************************************/
//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_BuildContractWithAddrAmtAbiETH(IN JUB_UINT16 contextID,
//                                          IN JUB_CHAR_PTR methodID,
//                                          IN JUB_CHAR_PTR address,
//                                          IN JUB_CHAR_PTR amount,
//                                          OUT JUB_CHAR_PTR_PTR abi) {
//
//    JUB_CHECK_CONTEXT_ETH(contextID);
//
//    auto context = (jub::ContextETH*)jub::ContextManager::GetInstance()->GetOne(contextID);
//    JUB_CHECK_NULL(context);
//
//    std::string strAbi;
//    JUB_VERIFY_RV(context->BuildContractWithAddrAmtAbi(methodID, address, amount, strAbi));
//    JUB_VERIFY_RV(_allocMem(abi, strAbi));
//
//    return JUBR_OK;
//}
//
/*****************************************************************************
* @function name : JUB_BuildContractWithTxIDAbiETH
* @in  param : contextID - context ID
*                     : methodID - contract methodID
*                     : transactionID - transaction ID
* @out param : specified contract abi
* @last change :
*****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_BuildContractWithTxIDAbiETH(IN JUB_UINT16 contextID,
                                       IN JUB_CHAR_PTR methodID,
                                       IN JUB_CHAR_PTR transactionID,
                                       OUT JUB_CHAR_PTR_PTR abi) {

    JUB_CHECK_CONTEXT_ETH(contextID);

    auto context = (jub::ContextETH*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string strAbi;
    JUB_VERIFY_RV(context->BuildContractWithTxIDAbi(methodID, transactionID, strAbi));
    JUB_VERIFY_RV(_allocMem(abi, strAbi));

    return JUBR_OK;
}

///*****************************************************************************
//* @function name : JUB_BuildContractWithAmtAbiETH
//* @in  param : contextID - context ID
//*                     : methodID - contract methodID
//*                     : amount - The amount at which you need to operate
//* @out param : specified contract abi
//* @last change :
//*****************************************************************************/
//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_BuildContractWithAmtAbiETH(IN JUB_UINT16 contextID,
//                                      IN JUB_CHAR_PTR methodID,
//                                      IN JUB_CHAR_PTR amount,
//                                      OUT JUB_CHAR_PTR_PTR abi) {
//
//    JUB_CHECK_CONTEXT_ETH(contextID);
//
//    auto context = (jub::ContextETH*)jub::ContextManager::GetInstance()->GetOne(contextID);
//    JUB_CHECK_NULL(context);
//
//    std::string strAbi;
//    JUB_VERIFY_RV(context->BuildContractWithAmtAbi(methodID, amount, strAbi));
//    JUB_VERIFY_RV(_allocMem(abi, strAbi));
//
//    return JUBR_OK;
//}
/*****************************************************************************
* @function name : JUB_BuildContractWithAddrAmtDataAbiETH
* @in  param : contextID - context ID
*                     : methodID - contract methodID
*                     : address - The address at which you need to operate
*                     : amount - The amount at which you need to operate
* @out param : specified contract abi
* @last change :
*****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_BuildContractWithAddrAmtDataAbiETH(IN JUB_UINT16 contextID,
                                              IN JUB_CHAR_PTR methodID,
                                              IN JUB_CHAR_PTR address,
                                              IN JUB_CHAR_PTR amount,
                                              IN JUB_CHAR_PTR data,
                                              OUT JUB_CHAR_PTR_PTR abi) {

    JUB_CHECK_CONTEXT_ETH(contextID);

    auto context = (jub::ContextETH*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string strAbi;
    JUB_VERIFY_RV(context->BuildContractWithAddrAmtDataAbi(methodID, address, amount, data, strAbi));
    JUB_VERIFY_RV(_allocMem(abi, strAbi));

    return JUBR_OK;
}
