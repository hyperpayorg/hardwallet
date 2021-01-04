//
//  JUB_SDK_BTC.h
//  JubSDK
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#ifndef JUB_SDK_BTC_h
#define JUB_SDK_BTC_h

#include "JUB_SDK_COMM.h"
#include <string>
#include <vector>
#include <map>

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus
    
    
typedef enum {
    COINBTC  = 0x00,
    COINBCH  = 0x01,
    COINLTC  = 0x02,
    COINUSDT = 0x03,
    COINDASH = 0x04,
    COINQTUM = 0x05,
    Default  = COINBTC
} JUB_ENUM_COINTYPE_BTC;

typedef enum {
    p2pkh = 0,
    //p2pwpkh,
    p2sh_p2wpkh,
    p2sh_multisig,
    p2wsh_multisig,
    p2pk,
    trans_type_ns_item
/*
     p2sh_p2wsh_multisig,
*/
} JUB_BTC_TRANS_TYPE;

typedef enum {
    BTC = 0x00,
    cBTC,
    mBTC,
    uBTC,
    Satoshi,
    ns
} JUB_BTC_UNIT_TYPE;

typedef enum {
    OWN = 0x00,
    LEGACY,
    DFLT = OWN
} JUB_ENUM_BTC_ADDRESS_FORMAT;

typedef struct stContextCfgBTC : stContextCfg {
    JUB_ENUM_COINTYPE_BTC   coinType;// = { JUB_ENUM_COINTYPE_BTC::COINBTC };
    JUB_BTC_TRANS_TYPE      transType;

    stContextCfgBTC();
    stContextCfgBTC(JUB_ENUM_COINTYPE_BTC _coinType,
                    JUB_CHAR_PTR _mainPath,
                    JUB_BTC_TRANS_TYPE _transType);
    virtual ~stContextCfgBTC() {}
} CONTEXT_CONFIG_BTC;

typedef struct stContextCfgMultisigBTC : stContextCfgBTC {
    unsigned long m;
    unsigned long n;
    std::vector<std::string> vCosignerMainXpub;

    stContextCfgMultisigBTC();
    virtual ~stContextCfgMultisigBTC() {}
} CONTEXT_CONFIG_MULTISIG_BTC;

typedef enum {
    P2PKH   = 0x00,
    RETURN0 = 0x01,
    P2SH_MULTISIG = 0x02,
    QRC20 = 0x03,
    P2WSH_MULTISIG = 0x04,
} SCRIPT_BTC_TYPE;

typedef struct stInput {
    SCRIPT_BTC_TYPE type;
    JUB_CHAR_PTR    preHash;
    JUB_UINT16      preIndex;
    JUB_UINT32      nSequence;
    JUB_UINT64      amount;
    BIP32_Path      path;

     stInput();
    ~stInput() = default;
} INPUT_BTC;

typedef struct stOutput {
    JUB_CHAR_PTR    address;
    JUB_UINT64      amount;
    JUB_ENUM_BOOL   changeAddress;
    BIP32_Path      path;

     stOutput();
    ~stOutput() = default;
} OUTPUT;

typedef struct stOutputReturn0 {
    JUB_UINT64      amount;
    JUB_UINT16      dataLen;
    JUB_BYTE        data[40];

     stOutputReturn0();
    ~stOutputReturn0() = default;
} OUTPUT_RETURN0;

typedef struct stOutputQRC20 {
    JUB_UINT16      dataLen;
    JUB_BYTE        data[200];

     stOutputQRC20();
    ~stOutputQRC20() = default;
} OUTPUT_QRC20;

typedef struct stOutputBTC {
    SCRIPT_BTC_TYPE type;
    union {
        OUTPUT stdOutput;
        OUTPUT_RETURN0 return0;
        OUTPUT_QRC20 qrc20;
    };

     stOutputBTC();
    ~stOutputBTC() = default;
} OUTPUT_BTC;

/*****************************************************************************
 * @function name : JUB_CreateContextBTC
 * @in  param : cfg
 *          : deviceID - device ID
 * @out param : contextID
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_CreateContextBTC(IN CONTEXT_CONFIG_BTC* cfg,
                            IN JUB_UINT16 deviceID,
                            OUT JUB_UINT16* contextID);

/*****************************************************************************
 * @function name : JUB_GetHDNodeBTC
 * @in  param : contextID - context ID
 *            : path
 * @out param : xpub
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_GetHDNodeBTC(IN JUB_UINT16 contextID,
                        IN BIP32_Path path,
                        OUT JUB_CHAR_PTR_PTR xpub);

/*****************************************************************************
 * @function name : JUB_GetMainHDNodeBTC
 * @in  param : contextID - context ID
 * @out param : xpub
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_GetMainHDNodeBTC(IN JUB_UINT16 contextID,
                            OUT JUB_CHAR_PTR_PTR xpub);

/*****************************************************************************
 * @function name : JUB_GetAddressBTC
 * @in  param : contextID - context ID
 *          : addrFmt
 *          : path
 *          : bShow
 * @out param : address
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_GetAddressBTC(IN JUB_UINT16 contextID,
                         IN JUB_ENUM_BTC_ADDRESS_FORMAT addrFmt,
                         IN BIP32_Path path,
                         IN JUB_ENUM_BOOL bShow,
                         OUT JUB_CHAR_PTR_PTR address);

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
//                                     OUT JUB_CHAR_PTR_PTR address);

/*****************************************************************************
 * @function name : JUB_SetMyAddressBTC
 * @in  param : contextID - context ID
 *          : addrFmt
 *          : path
 * @out param : address
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SetMyAddressBTC(IN JUB_UINT16 contextID,
                           IN JUB_ENUM_BTC_ADDRESS_FORMAT addrFmt,
                           IN BIP32_Path path,
                           OUT JUB_CHAR_PTR_PTR address);

/*****************************************************************************
 * @function name : JUB_SignTransactionBTC
 * @in  param : contextID - context ID
 *          : addrFmt
 *          : inputs
 *          : iCount
 *          : outputs
 *          : oCount
 *          : lockTime
 * @out param : raw
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SignTransactionBTC(IN JUB_UINT16 contextID,
                              IN JUB_ENUM_BTC_ADDRESS_FORMAT addrFmt,
                              IN INPUT_BTC inputs[], IN JUB_UINT16 iCount,
                              IN OUTPUT_BTC outputs[], IN JUB_UINT16 oCount,
                              IN JUB_UINT32 lockTime,
                              OUT JUB_CHAR_PTR_PTR raw);

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
                               OUT JUB_UINT32_PTR plockTime);

/*****************************************************************************
 * @function name : JUB_SetUnitBTC
 * @in  param : contextID - context ID
 *            : unit
 * @out param :
 * @last change :
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_SetUnitBTC(IN JUB_UINT16 contextID,
                      IN JUB_BTC_UNIT_TYPE unit);

/*****************************************************************************
 * @function name : JUB_BuildUSDTOutputs
 * @in  param : contextID - context ID
 *            : USDTTo - to address
 *            : amount
 * @out param : outputs
 * @last change : build the return0 and dust 2 outputs
 *****************************************************************************/
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_BuildUSDTOutputs(IN JUB_UINT16 contextID,
                            IN JUB_CHAR_PTR USDTTo,
                            IN JUB_UINT64 amount,
                            OUT OUTPUT_BTC outputs[2]);


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
JUB_COINCORE_DLL_EXPORT
JUB_RV JUB_BuildQRC20Outputs(IN JUB_UINT16 contextID,
                             IN JUB_CHAR_PTR contractAddress, IN JUB_UINT8 decimal, IN JUB_CHAR_PTR symbol,
                             IN JUB_UINT64 gasLimit, IN JUB_UINT64 gasPrice,
                             IN JUB_CHAR_PTR to, IN JUB_CHAR_PTR value,
                             OUT OUTPUT_BTC outputs[1]);

#ifdef __cplusplus
}
#endif // #ifdef __cplusplus
#endif /* JUB_SDK_BTC_h */
