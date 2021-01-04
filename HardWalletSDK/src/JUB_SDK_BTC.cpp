//
//  JUB_SDK_BTC.cpp
//  JubSDK
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#include "JUB_SDK_BTC.h"

#include "utility/util.h"

#include "context/ContextBTC.h"
#include "token/interface/TokenInterface.hpp"
#include "libBTC/libBTC.hpp"

JUB_RV _allocMem(JUB_CHAR_PTR_PTR memPtr, const std::string &strBuf);

stContextCfgBTC::stContextCfgBTC() {
     coinType = JUB_ENUM_COINTYPE_BTC::Default;
    transType = JUB_BTC_TRANS_TYPE::trans_type_ns_item;
}

stContextCfgBTC::stContextCfgBTC(JUB_ENUM_COINTYPE_BTC _coinType,
                                 JUB_CHAR_PTR _mainPath,
                                 JUB_BTC_TRANS_TYPE _transType) {
     mainPath = _mainPath;
     coinType =  _coinType;
    transType = _transType;
}

stContextCfgMultisigBTC::stContextCfgMultisigBTC() {
    m = 0;
    n = 0;
    vCosignerMainXpub.clear();
}

stInput::stInput() {
    type = SCRIPT_BTC_TYPE::P2PKH;

    preHash = NULL;
    preIndex = 0;
    nSequence = jub::btc::sequence;
    amount = 0;
    path = BIP32_Path();
}

stOutput::stOutput() {
    address = NULL;
    amount = 0;
    changeAddress = JUB_ENUM_BOOL::BOOL_FALSE;
    path = BIP32_Path();
}

stOutputReturn0::stOutputReturn0() {
    amount = 0;
    dataLen = 0;
    memset(data, 0x00, sizeof(data)/sizeof(JUB_BYTE));
}

stOutputQRC20::stOutputQRC20() {
    dataLen = 0;
    memset(data, 0x00, sizeof(data)/sizeof(JUB_BYTE));
}

stOutputBTC::stOutputBTC() {
    type = SCRIPT_BTC_TYPE::P2PKH;

    stdOutput = OUTPUT();
    return0   = OUTPUT_RETURN0();
    qrc20     = OUTPUT_QRC20();
}

/*****************************************************************************
 * @function name : JUB_CreateContextBTC
 * @in  param : cfg
 *            : deviceID - device ID
 * @out param : contextID
 * @last change :
 *****************************************************************************/
JUB_RV JUB_CreateContextBTC(IN CONTEXT_CONFIG_BTC* cfg,
                            IN JUB_UINT16 deviceID,
                            OUT JUB_UINT16* contextID) {

    if (nullptr == jub::TokenManager::GetInstance()->GetOne(deviceID)) {
        JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
    }

    jub::ContextBTC* context = NULL;
    switch (cfg->transType) {
        case p2pkh:
        case p2sh_p2wpkh:
        {
            context = new jub::ContextBTC(*cfg, deviceID);

            JUB_VERIFY_RV(context->ActiveSelf());
            break;
        }
        case p2sh_multisig:
        case p2wsh_multisig:
        {
            CONTEXT_CONFIG_MULTISIG_BTC* multisigBTC = dynamic_cast<CONTEXT_CONFIG_MULTISIG_BTC*>(cfg);
            context = new jub::ContextMultisigBTC(*multisigBTC, deviceID);

            if (!dynamic_cast<jub::ContextMultisigBTC*>(context)->isValid()) {
                delete context; context = NULL;
                JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
            }

            JUB_VERIFY_RV(context->ActiveSelf());

            // get main xpub
            std::string mainXpub;
            JUB_VERIFY_RV(context->GetMainHDNode(mainXpub));
            dynamic_cast<jub::ContextMultisigBTC*>(context)->order(mainXpub);
            break;
        }
        default:
            JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
    }
    JUB_CHECK_NULL(context);

    *contextID = jub::ContextManager::GetInstance()->AddOne(context);

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_GetHDNodeBTC
 * @in  param : contextID - context ID
 *            : path
 * @out param : xpub
 * @last change :
 *****************************************************************************/
JUB_RV JUB_GetHDNodeBTC(IN JUB_UINT16 contextID,
                        IN BIP32_Path path,
                        OUT JUB_CHAR_PTR_PTR xpub) {

    JUB_CHECK_CONTEXT_BTC(contextID);

    auto context = (jub::ContextBTC*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_xpub;
    JUB_VERIFY_RV(context->GetHDNode(path, str_xpub));
    JUB_VERIFY_RV(_allocMem(xpub, str_xpub));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_GetMainHDNodeBTC
 * @in  param : contextID - context ID
 * @out param : xpub
 * @last change :
 *****************************************************************************/
JUB_RV JUB_GetMainHDNodeBTC(IN JUB_UINT16 contextID,
                            OUT JUB_CHAR_PTR_PTR xpub) {

    JUB_CHECK_CONTEXT_BTC(contextID);

    auto context = (jub::ContextBTC*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_xpub;
    JUB_VERIFY_RV(context->GetMainHDNode(str_xpub));
    JUB_VERIFY_RV(_allocMem(xpub, str_xpub));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_GetAddressBTC
 * @in  param : contextID - context ID
 *          : path
 *          : bShow
 * @out param : address
 * @last change :
 *****************************************************************************/
JUB_RV JUB_GetAddressBTC(IN JUB_UINT16 contextID,
                         IN JUB_ENUM_BTC_ADDRESS_FORMAT addrFmt,
                         IN BIP32_Path path,
                         IN JUB_ENUM_BOOL bshow,
                         OUT JUB_CHAR_PTR_PTR address) {

    JUB_CHECK_CONTEXT_BTC(contextID);

    auto context = (jub::ContextBTC*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_address;
    JUB_VERIFY_RV(context->GetAddress(addrFmt, path, bshow, str_address));
    JUB_VERIFY_RV(_allocMem(address, str_address));

    return JUBR_OK;
}

///*****************************************************************************
// * @function name : JUB_GetP2SHMultisigAddressBTC
// * @in  param : contextID - context ID
// *            : path
// *            : bShow
// * @out param : address
// * @last change :
// *****************************************************************************/
//JUB_COINCORE_DLL_EXPORT
//JUB_RV JUB_GetP2SHMultisigAddressBTC(IN JUB_UINT16 contextID,
//                                     IN BIP32_Path path,
//                                     IN JUB_ENUM_BOOL bShow,
//                                     OUT JUB_CHAR_PTR_PTR address) {
//    JUB_CHECK_CONTEXT_BTC(contextID);
//
//    auto context = (jub::ContextBTC*)jub::ContextManager::GetInstance()->GetOne(contextID);
//    JUB_CHECK_NULL(context);
//
//    if (!dynamic_cast<jub::ContextMultisigBTC*>(context)) {
//        return JUBR_ARGUMENTS_BAD;
//    }
//
//    std::string str_address;
//    JUB_VERIFY_RV(dynamic_cast<jub::ContextMultisigBTC*>(context)->GetP2SHMultisigAddress(path, bShow, str_address));
//    JUB_VERIFY_RV(_allocMem(address, str_address));
//
//    return JUBR_OK;
//}
//
/*****************************************************************************
 * @function name : JUB_SetMyAddressBTC
 * @in  param : contextID - context ID
 *            : path
 * @out param : address
 * @last change :
 *****************************************************************************/
JUB_RV JUB_SetMyAddressBTC(IN JUB_UINT16 contextID,
                           IN JUB_ENUM_BTC_ADDRESS_FORMAT addrFmt,
                           IN BIP32_Path path,
                           OUT JUB_CHAR_PTR_PTR address) {

    JUB_CHECK_CONTEXT_BTC(contextID);

    auto context = (jub::ContextBTC*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_address;
    JUB_VERIFY_RV(context->SetMyAddress(addrFmt, path, str_address));
    JUB_VERIFY_RV(_allocMem(address, str_address));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_SignTransactionBTC
 * @in  param : contextID - context ID
 *            : inputs
 *            : iCount
 *            : outputs
 *            : oCount
 *            : lockTime
 * @out param : raw
 * @last change :
 *****************************************************************************/
JUB_RV JUB_SignTransactionBTC(IN JUB_UINT16 contextID,
                              IN JUB_ENUM_BTC_ADDRESS_FORMAT addrFmt,
                              IN INPUT_BTC inputs[], IN JUB_UINT16 iCount,
                              IN OUTPUT_BTC outputs[], IN JUB_UINT16 oCount,
                              IN JUB_UINT32 lockTime,
                              OUT JUB_CHAR_PTR_PTR raw) {
    JUB_RV rv = JUBR_ERROR;

    JUB_CHECK_CONTEXT_BTC(contextID);

    std::vector<INPUT_BTC> vInputs(inputs, inputs + iCount);
    std::vector<OUTPUT_BTC> vOutputs(outputs, outputs + oCount);

    bool isQrc20    = false;
    int changeCount = 0;

    for (OUTPUT_BTC output : vOutputs) {
        if (output.type == QRC20) {
            isQrc20 = true;
            continue;
        }
        if (output.type == P2PKH) {
            if (output.stdOutput.changeAddress == BOOL_TRUE) {
                changeCount++;
            }
        }
    }
    if (isQrc20) {
        if (   2 == vOutputs.size()
            && 1 == changeCount) {
        }
        else {
            return JUBR_QRC20_WRONG_FORMAT;
        }
    }

    jub::ContextBTC* context = (jub::ContextBTC*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string str_raw;
    rv = context->SignTX(addrFmt, vInputs, vOutputs, lockTime, str_raw);
    JUB_RV ret = rv;
    if (dynamic_cast<jub::ContextMultisigBTC*>(context)) {
        if (JUBR_MULTISIG_OK == rv) {
            ret = JUBR_OK;
        }
    }
    JUB_VERIFY_RV(ret);

    JUB_VERIFY_RV(_allocMem(raw, str_raw));

    return rv;
}

/*****************************************************************************
 * @function name : JUB_ParseTransactionBTC
 * @in  param : contextID - context ID
 *            : incRaw
 * @out param : inputs
 *            : outputs
 *            : plockTime
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_ParseTransactionBTC(IN JUB_UINT16 contextID,
                               IN JUB_CHAR_PTR incRaw,
                               OUT std::vector<INPUT_BTC>& inputs,
                               OUT std::vector<OUTPUT_BTC>& outputs,
                               OUT JUB_UINT32_PTR plockTime) {

    JUB_CHECK_CONTEXT_BTC(contextID);

    jub::ContextBTC* context = (jub::ContextBTC*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    std::string hexIncRaw(incRaw);
    JUB_VERIFY_RV(context->ParseTransaction(hexIncRaw,
                                            inputs, outputs,
                                            plockTime));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_SetUnitBTC
 * @in  param : contextID - context ID
 *            : unit
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_RV JUB_SetUnitBTC(IN JUB_UINT16 contextID,
                      IN JUB_BTC_UNIT_TYPE unit) {

    JUB_CHECK_CONTEXT_BTC(contextID);

    auto context = (jub::ContextBTC*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    JUB_VERIFY_RV(context->SetUnit(unit));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : JUB_BuildUSDTOutputs
 * @in  param : contextID - context ID
 *            : USDTTo - to address
 *            : amount
 * @out param : outputs
 * @last change : build the return0 and dust 2 outputs
 *****************************************************************************/
JUB_RV JUB_BuildUSDTOutputs(IN JUB_UINT16 contextID,
                            IN JUB_CHAR_PTR USDTTo,
                            IN JUB_UINT64 amount,
                            OUT OUTPUT_BTC outputs[2]) {

    JUB_CHECK_CONTEXT_BTC(contextID);

    jub::ContextBTC* context = (jub::ContextBTC*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    JUB_VERIFY_RV(context->BuildUSDTOutputs(USDTTo, amount, outputs));

    return JUBR_OK;
}

/*****************************************************************************
 * @function name : BuildQRC20Outputs
 * @in  param : contextID - context ID
 *            : contractAddress - contract address for QRC20 token
 *            : decimal         - decimal for QRC20 token
 *            : symbol          - symbol for QRC20 token
 *            : gasLimit - gas limit
 *            : gasPrice - gas price
 *            : to    - to address for transfer
 *            : value - amount for transfer
 * @out param : outputs
 * @last change : build the QRC20 outputs
 *****************************************************************************/
JUB_RV JUB_BuildQRC20Outputs(IN JUB_UINT16 contextID,
                             IN JUB_CHAR_PTR contractAddress, IN JUB_UINT8 decimal, IN JUB_CHAR_PTR symbol,
                             IN JUB_UINT64 gasLimit, IN JUB_UINT64 gasPrice,
                             IN JUB_CHAR_PTR to, IN JUB_CHAR_PTR value,
                             OUT OUTPUT_BTC outputs[1]) {

    JUB_CHECK_CONTEXT_BTC(contextID);

    jub::ContextBTC* context = (jub::ContextBTC*)jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    JUB_VERIFY_RV(context->SetQRC20Token(contractAddress,decimal,symbol));
    JUB_VERIFY_RV(context->BuildQRC20Outputs(gasLimit, gasPrice, contractAddress, to, value, outputs));

    return JUBR_OK;
}
