//
//  test_eth.mm
//  JubSDKCore
//
//  Created by Pan Min on 2019/8/5.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JubSDKCore.h"
#import "JubSDKCore+COIN_ETH.h"

#include <json/json.h>
#include <iostream>     // std::cout
#include <sstream>      // std::istringstream
#include <string>       // std::string
#include <vector>
#include <fstream>

using namespace std;

extern JubSDKCore* g_sdk;

void error_exit(const char* message);
void main_test();

NSUInteger verify_pin(NSUInteger contextID);
void set_timeout_test(NSUInteger contextID);

void            get_address_pubkey_ETH(NSUInteger contextID);
void              transaction_test_ETH(NSUInteger contextID, Json::Value root);
void             transaction_ERC20_ETH(NSUInteger contextID, Json::Value root);
void transaction_UniswapV2Router02_ETH(NSUInteger contextID, Json::Value root);
void          transaction_Contract_ETH(NSUInteger contextID, Json::Value root, int index);
void           set_my_address_test_ETH(NSUInteger contextID);

void ETH_test(NSUInteger deviceID, const char* json_file) {
    NSUInteger lastError = JUBR_ERROR;
    
    Json::CharReaderBuilder builder;
    Json::Value root;
    ifstream in(json_file, ios::binary);
    if (!in.is_open()) {
        error_exit("Error opening json file\n");
    }
    
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, in, &root, &errs)) {
        error_exit("Error parse json file\n");
    }
    
    NSString* mainPath = [NSString stringWithUTF8String:(char*)root["main_path"].asCString()];
    
    ContextConfigETH* cfgETH = [[ContextConfigETH alloc] init];
    cfgETH.mainPath = mainPath;
    cfgETH.chainID = root["chainID"].asInt();
    NSUInteger contextID = [g_sdk JUB_CreateContextETH:deviceID
                                                   cfg:cfgETH];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_CreateContextETH(%li)\n", lastError, deviceID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"ContextETH: %li\n", contextID);
    
    while (true) {
        NSLog(@"|----------------------------------------|\n");
        NSLog(@"|********** Jubiter Wallet ETH **********|\n");
        NSLog(@"| 1.   show_address_pubkey_test.         |\n");
        NSLog(@"| 2. transaction_test.                   |\n");
        NSLog(@"| 3. transaction_ERC20_test.             |\n");
        NSLog(@"| 4. transaction_UniswapV2Router02_test. |\n");
        NSLog(@"| 5. set_my_address_test.                |\n");
        NSLog(@"| 6. set_timeout_test.                   |\n");
        NSLog(@"|                                        |\n");
        NSLog(@"| 9. return.                             |\n");
        NSLog(@"|----------------------------------------|\n");
        NSLog(@"* Please enter your choice:               \n");
        
        int choice = 0;
        std::cin >> choice;
        switch (choice) {
            case 1:
                get_address_pubkey_ETH(contextID);
                break;
            case 2:
                transaction_test_ETH(contextID, root);
                break;
            case 3:
                transaction_ERC20_ETH(contextID, root);
                break;
            case 4:
                transaction_UniswapV2Router02_ETH(contextID, root);
                break;
            case 5:
                set_my_address_test_ETH(contextID);
                break;
            case 6:
                set_timeout_test(contextID);
                break;
            case 9:
                main_test();
            default:
                continue;
        }
    }
}

void get_address_pubkey_ETH(NSUInteger contextID) {
    NSUInteger lastError = JUBR_ERROR;
    
    NSLog(@"please input change level (non-zero means 1):\n");
    int change = 0;
    cin >> change;
    NSLog(@"please input index \n");
    long index = 0;
    cin >> index;
    
    NSString* pubkey = [g_sdk JUB_GetMainHDNodeETH:contextID
                                            format:NS_HEX];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetMainHDNodeETH(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"MainXpub in hex format: %@\n", pubkey);
    
    pubkey = [g_sdk JUB_GetMainHDNodeETH:contextID
                                  format:NS_XPUB];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetMainHDNodeETH(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"MainXpub in xpub format: %@\n", pubkey);
    
    BIP32Path* path = [[BIP32Path alloc] init];
    path.change = JUB_NS_ENUM_BOOL(change);
    path.addressIndex = index;
    pubkey = [g_sdk JUB_GetHDNodeETH:contextID
                              format:NS_HEX
                                path:path];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetHDNodeETH(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"pubkey in hex format: %@\n", pubkey);
    
    pubkey = [g_sdk JUB_GetHDNodeETH:contextID
                              format:NS_XPUB
                                path:path];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetHDNodeETH(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"pubkey in xpub format: %@\n", pubkey);
    
    NSString* address = [g_sdk JUB_GetAddressETH:contextID
                                            path:path
                                           bShow:BOOL_NS_TRUE];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetAddressETH(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"address: %@\n", address);
}


void transaction_test_ETH(NSUInteger contextID, Json::Value root) {
    NSUInteger lastError = JUBR_ERROR;
    
    lastError = verify_pin(contextID);
    if (JUBR_OK != lastError) {
        return;
    }
    
    //ETH Test
    BIP32Path* path = [[BIP32Path alloc] init];
    if (root["ETH"]["bip32_path"]["change"].asBool()) {
        path.change = BOOL_NS_TRUE;
    }
    else {
        path.change = BOOL_NS_FALSE;
    }
    path.addressIndex = root["ETH"]["bip32_path"]["addressIndex"].asUInt();
    
    NSUInteger nonce = root["ETH"]["nonce"].asUInt();//.asDouble();
    NSUInteger gasLimit = root["ETH"]["gasLimit"].asUInt();//.asDouble();
    NSString* gasPriceInWei = [NSString stringWithUTF8String:(char*)root["ETH"]["gasPriceInWei"].asCString()];
    NSString* valueInWei = [NSString stringWithUTF8String:(char*)root["ETH"]["valueInWei"].asCString()];
    NSString* to = [NSString stringWithUTF8String:(char*)root["ETH"]["to"].asCString()];
    NSString* data = [NSString stringWithUTF8String:(char*)root["ETH"]["data"].asCString()];
    NSString* raw = [g_sdk JUB_SignTransactionETH:contextID
                                             path:path
                                            nonce:nonce
                                         gasLimit:gasLimit
                                    gasPriceInWei:gasPriceInWei
                                               to:to
                                       valueInWei:valueInWei
                                            input:data];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SignTransactionETH(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"raw: %@\n", raw);
}


//ERC-20 Test
void transaction_ERC20_ETH(NSUInteger contextID, Json::Value root) {
    NSUInteger lastError = JUBR_ERROR;
    
    lastError = verify_pin(contextID);
    if (JUBR_OK != lastError) {
        return;
    }
    
    NSString* tokenName = [NSString stringWithUTF8String:(char*)root["ERC20"]["tokenName"].asCString()];
    NSUInteger dp = root["ERC20"]["dp"].asUInt();
    NSString* contractAddress = [NSString stringWithUTF8String:(char*)root["ERC20"]["contract_address"].asCString()];
    NSString* tokenTo = [NSString stringWithUTF8String:(char*)root["ERC20"]["token_to"].asCString()];
    NSString* tokenValue = [NSString stringWithUTF8String:(char*)root["ERC20"]["token_value"].asCString()];
    NSString* abi = [g_sdk JUB_BuildERC20AbiETH:contextID
                                      tokenName:tokenName
                                         unitDP:dp
                                contractAddress:contractAddress
                                        tokenTo:tokenTo
                                     tokenValue:tokenValue];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_BuildERC20AbiETH(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"abi: %@\n", abi);
    
    BIP32Path* path = [[BIP32Path alloc] init];
    if (root["ERC20"]["bip32_path"]["change"].asBool()) {
        path.change = BOOL_NS_TRUE;
    }
    else {
        path.change = BOOL_NS_FALSE;
    }
    path.addressIndex = root["ERC20"]["bip32_path"]["addressIndex"].asUInt();
    
    NSUInteger nonce = root["ERC20"]["nonce"].asUInt();//.asDouble();
    NSUInteger gasLimit = root["ERC20"]["gasLimit"].asUInt();//.asDouble();
    NSString* gasPriceInWei = [NSString stringWithUTF8String:(char*)root["ERC20"]["gasPriceInWei"].asCString()];
    NSString* raw = [g_sdk JUB_SignTransactionETH:contextID
                                             path:path
                                            nonce:nonce
                                         gasLimit:gasLimit
                                    gasPriceInWei:gasPriceInWei
                                               to:contractAddress
                                       valueInWei:@""
                                            input:abi];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SignTransactionETH(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"raw: %@\n", raw);
}


//transaction_UniswapV2Router02_ETH
void transaction_UniswapV2Router02_ETH(NSUInteger contextID, Json::Value root) {
    NSUInteger lastError = JUBR_ERROR;
    
    lastError = verify_pin(contextID);
    if (JUBR_OK != lastError) {
        return;
    }
    
    //ETH UniswapV2Router02 Test
    BIP32Path* path = [[BIP32Path alloc] init];
    if (root["ETH"]["bip32_path"]["change"].asBool()) {
        path.change = BOOL_NS_TRUE;
    }
    else {
        path.change = BOOL_NS_FALSE;
    }
    path.addressIndex = root["ETH"]["bip32_path"]["addressIndex"].asUInt();
    
    NSUInteger nonce = root["ETH"]["nonce"].asUInt();//.asDouble();
    NSUInteger gasLimit = root["ETH"]["gasLimit"].asUInt();//.asDouble();
    NSString* gasPriceInWei = [NSString stringWithUTF8String:(char*)root["ETH"]["gasPriceInWei"].asCString()];
    NSString* valueInWei = [NSString stringWithUTF8String:(char*)root["ETH"]["valueInWei"].asCString()];
    NSString* to = [NSString stringWithUTF8String:(char*)root["ETH"]["to"].asCString()];
    NSString* data = [NSString stringWithUTF8String:(char*)root["ETH"]["data"].asCString()];
    
    [g_sdk JUB_SetContrAddrETH:contextID
                     contrAddr:to];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SetContrAddrETH(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    
    NSString* raw = [g_sdk JUB_SignContractETH:contextID
                                          path:path
                                         nonce:nonce
                                      gasLimit:gasLimit
                                 gasPriceInWei:gasPriceInWei
                                            to:to
                                    valueInWei:valueInWei
                                         input:data];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SignContractETH(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"raw: %@\n", raw);
}


void set_my_address_test_ETH(NSUInteger contextID) {
    NSUInteger lastError = JUBR_ERROR;
    
    lastError = verify_pin(contextID);
    if (JUBR_OK != lastError) {
        return;
    }
    
    NSLog(@"please input change level (non-zero means 1):\n");
    int change = 0;
    std::cin >> change;
    NSLog(@"please input index:\n");
    long index = 0;
    std::cin >> index;
    
    BIP32Path* path = [[BIP32Path alloc] init];
    path.change = JUB_NS_ENUM_BOOL(change);
    path.addressIndex = index;
    
    NSString* address = [g_sdk JUB_SetMyAddressETH:contextID
                                              path:path];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SetMyAddressETH(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"set my address is: %@\n", address);
}

void ETH_contract_test(NSUInteger deviceID, const char* json_file) {
    NSUInteger lastError = JUBR_ERROR;
    
    Json::CharReaderBuilder builder;
    Json::Value root;
    ifstream in(json_file, ios::binary);
    if (!in.is_open()) {
        error_exit("Error opening json file\n");
    }
    
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, in, &root, &errs)) {
        error_exit("Error parse json file\n");
    }
    
    NSString* mainPath = [NSString stringWithUTF8String:(char*)root["main_path"].asCString()];
    
    ContextConfigETH* cfgETH = [[ContextConfigETH alloc] init];
    cfgETH.mainPath = mainPath;
    cfgETH.chainID = root["chainID"].asInt();
    NSUInteger contextID = [g_sdk JUB_CreateContextETH:deviceID
                                                   cfg:cfgETH];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_CreateContextETH(%li)\n", lastError, deviceID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"ContextETH: %li\n", contextID);
    
    while (true) {
        NSLog(@"|-----------------------------------|\n");
        NSLog(@"|*** Jubiter Wallet ETH Contract ***|\n");
        NSLog(@"| 0. create_contract_test.          |\n");
//        NSLog(@"| 1. add&remove_manager_test.       |\n");
//        NSLog(@"| 2. transfer_to_test.              |\n");
        NSLog(@"|31.confirm_transaction_test.       |\n");
        NSLog(@"|32.execute_transaction_test.       |\n");
        NSLog(@"|33. revoke_transaction_test.       |\n");
//        NSLog(@"| 4. withdraw_test.                 |\n");
        NSLog(@"|51.      eth_submit_test.          |\n");
        NSLog(@"|52.eth-erc20_submit_test.          |\n");
        NSLog(@"|                                   |\n");
        NSLog(@"| 9. return.                        |\n");
        NSLog(@"|-----------------------------------|\n");
        NSLog(@"* Please enter your choice:          \n");
        
        int choice = 0;
        std::cin >> choice;
        switch (choice) {
            case 0:
//            case 1:
//            case 2:
            case 31:
            case 32:
            case 33:
//            case 4:
            case 51:
            case 52:
                transaction_Contract_ETH(contextID, root, choice);
                break;
            case 9:
                main_test();
            default:
                continue;
        }
    }
}

void transaction_Contract_ETH(NSUInteger contextID, Json::Value root, int index) {
    NSUInteger lastError = JUBR_ERROR;
    
    lastError = verify_pin(contextID);
    if (JUBR_OK != lastError) {
        return;
    }
    
    NSString* contractAddress = [NSString stringWithUTF8String:(char*)root["multiSig"]["contract_address"].asCString()];
    
    BIP32Path* path = [[BIP32Path alloc] init];
    if (root["ETH"]["bip32_path"]["change"].asBool()) {
        path.change = BOOL_NS_TRUE;
    }
    else {
        path.change = BOOL_NS_FALSE;
    }
    path.addressIndex = root["ETH"]["bip32_path"]["addressIndex"].asUInt();
    
    NSUInteger nonce = root["ETH"]["nonce"].asUInt();//.asDouble();
    NSUInteger gasLimit = root["ETH"]["gasLimit"].asUInt();//.asDouble();
    NSString* gasPriceInWei = [NSString stringWithUTF8String:(char*)root["ETH"]["gasPriceInWei"].asCString()];
    NSString* valueInWei = [NSString stringWithUTF8String:(char*)root["ETH"]["valueInWei"].asCString()];
    switch (index) {
    case 0:
    {
        if (!(char*)root["multiSig"]["0"].isArray()) {
            break;
        }
        // Create Contract
        for (unsigned int i=0; i<1; ++i) {
            NSString* abi = [NSString stringWithUTF8String:(char*)root["multiSig"]["0"][i]["input"].asCString()];
            NSString* raw = [g_sdk JUB_SignContractETH:contextID
                                                  path:path
                                                 nonce:nonce
                                              gasLimit:gasLimit
                                         gasPriceInWei:gasPriceInWei
                                                    to:nil
                                            valueInWei:valueInWei
                                                 input:abi];
            lastError = (long)[g_sdk JUB_LastError];
            NSLog(@"[%li] JUB_SignTransactionETH(%li)\n", lastError, contextID);
            if (JUBR_OK != lastError) {
                return ;
            }
            NSLog(@"raw: %@\n", raw);
        }
        break;
    }
//    case 1:
//    {
//        if (!(char*)root["multiSig"]["1"].isArray()) {
//            break;
//        }
//        
//        // Add&Remove Manager
////        for (unsigned int i=0; i<2; ++i) {
//        for (unsigned int i=0; i<1; ++i) {
//            NSString* methodID = [NSString stringWithUTF8String:(char*)root["multiSig"]["1"][i]["methodID"].asCString()];
//            NSString* address = [NSString stringWithUTF8String:(char*)root["multiSig"]["1"][i]["address"].asCString()];
//            NSString* abi = [g_sdk JUB_BuildContractWithAddrAbiETH:contextID
//                                                          methodID:methodID
//                                                           address:address];
//            lastError = (long)[g_sdk JUB_LastError];
//            NSLog(@"[%li] JUB_BuildContractWithAddrAbiETH(%li)\n", lastError, contextID);
//            if (JUBR_OK != lastError) {
//                return ;
//            }
//            NSLog(@"[%@] abi: %@\n", methodID, abi);
//
////            NSString* raw = [g_sdk JUB_SignContractETH:contextID
////                                                  path:path
////                                                 nonce:nonce
////                                              gasLimit:gasLimit
////                                         gasPriceInWei:gasPriceInWei
////                                                    to:contractAddress
////                                            valueInWei:valueInWei
////                                                 input:abi];
//            
//            NSString* raw = [g_sdk JUB_SignContractETH:contextID
//                                                  path:path
//                                                 nonce:6
//                                              gasLimit:25200
//                                         gasPriceInWei:@"20000000000"
//                                                    to:@"0xf44b0e1b808e904991a580915c6a99db90f89756"
//                                            valueInWei:@""
//                                                 input:@"0x2d06177a000000000000000000000000cee285494b7f9fc1ea5aa9c447d14e0ab1b0c4db"];
////            NSString* raw = [g_sdk JUB_SignTransactionETH:contextID
////                                                     path:path
////                                                    nonce:4
////                                                 gasLimit:25200
////                                            gasPriceInWei:@"17000000000"
////                                                       to:@"0xf44b0e1b808e904991a580915c6a99db90f89756"
////                                               valueInWei:@""
////                                                    input:@"0x2d06177a000000000000000000000000cee285494b7f9fc1ea5aa9c447d14e0ab1b0c4db"];
//            lastError = (long)[g_sdk JUB_LastError];
//            NSLog(@"[%li] JUB_SignTransactionETH(%li)\n", lastError, contextID);
//            if (JUBR_OK != lastError) {
//                return ;
//            }
//            NSLog(@"raw: %@\n", raw);
//        }
//        break;
//    }
//    case 2:
//    {
//        // TransferTo
//        for (unsigned int i=0; i<1; ++i) {
//            NSString* methodID = [NSString stringWithUTF8String:(char*)root["multiSig"]["2"][i]["methodID"].asCString()];
//            NSString* address = [NSString stringWithUTF8String:(char*)root["multiSig"]["2"][i]["address"].asCString()];
//            NSString* amount = [NSString stringWithUTF8String:(char*)root["multiSig"]["2"][i]["amount"].asCString()];
//            NSString* data = [NSString stringWithUTF8String:(char*)root["multiSig"]["2"][i]["amount"].asCString()];
//            NSString* abi = [g_sdk JUB_BuildContractWithAddrAmtDataAbiETH:contextID
//                                                                 methodID:methodID
//                                                                  address:address
//                                                                   amount:amount
//                                                                     data:data];
//            lastError = (long)[g_sdk JUB_LastError];
//            NSLog(@"[%li] JUB_BuildContractWithAddrAmtDataAbiETH(%li)\n", lastError, contextID);
//            if (JUBR_OK != lastError) {
//                return ;
//            }
//            NSLog(@"[%@] abi: %@\n", methodID, abi);
//
//            NSString* raw = [g_sdk JUB_SignContractETH:contextID
//                                                  path:path
//                                                 nonce:nonce
//                                              gasLimit:gasLimit
//                                         gasPriceInWei:gasPriceInWei
//                                                    to:contractAddress
//                                            valueInWei:valueInWei
//                                                 input:abi];
//            lastError = (long)[g_sdk JUB_LastError];
//            NSLog(@"[%li] JUB_SignTransactionETH(%li)\n", lastError, contextID);
//            if (JUBR_OK != lastError) {
//                return ;
//            }
//            NSLog(@"raw: %@\n", raw);
//        }
//        break;
//    }
    case 31:
    case 32:
    case 33:
    {
        int i = index - 30 - 1;
        // confirm/execute/revoke Transaction
        NSString* methodID = [NSString stringWithUTF8String:(char*)root["multiSig"]["3"][i]["methodID"].asCString()];
        NSString* transactionID = [NSString stringWithUTF8String:(char*)root["multiSig"]["3"][i]["transactionID"].asCString()];
        NSString* abi = [g_sdk JUB_BuildContractWithTxIDAbiETH:contextID
                                                      methodID:methodID
                                                 transactionID:transactionID];
        lastError = (long)[g_sdk JUB_LastError];
        NSLog(@"[%li] JUB_BuildContractWithTxIDAbiETH(%li)\n", lastError, contextID);
        if (JUBR_OK != lastError) {
            return ;
        }
        NSLog(@"[%@] abi: %@\n", methodID, abi);
        
        NSString* raw = [g_sdk JUB_SignContractETH:contextID
                                              path:path
                                             nonce:nonce
                                          gasLimit:gasLimit
                                     gasPriceInWei:gasPriceInWei
                                                to:contractAddress
                                        valueInWei:valueInWei
                                             input:abi];
        lastError = (long)[g_sdk JUB_LastError];
        NSLog(@"[%li] JUB_SignTransactionETH(%li)\n", lastError, contextID);
        if (JUBR_OK != lastError) {
            return ;
        }
        NSLog(@"raw: %@\n", raw);
        break;
    }
//    case 4:
//    {
//        // Withdraw
//        for (unsigned int i=0; i<1; ++i) {
//            NSString* methodID = [NSString stringWithUTF8String:(char*)root["multiSig"]["4"][i]["methodID"].asCString()];
//            NSString* amount = [NSString stringWithUTF8String:(char*)root["multiSig"]["4"][i]["amount"].asCString()];
//            NSString* abi = [g_sdk JUB_BuildContractWithTxIDAbiETH:contextID
//                                                          methodID:methodID
//                                                            amount:amount];
//            lastError = (long)[g_sdk JUB_LastError];
//            NSLog(@"[%li] JUB_BuildContractWithTxIDAbiETH(%li)\n", lastError, contextID);
//            if (JUBR_OK != lastError) {
//                return ;
//            }
//            NSLog(@"[%@] abi: %@\n", methodID, abi);
//
//            NSString* raw = [g_sdk JUB_SignContractETH:contextID
//                                                  path:path
//                                                 nonce:nonce
//                                              gasLimit:gasLimit
//                                         gasPriceInWei:gasPriceInWei
//                                                    to:contractAddress
//                                            valueInWei:valueInWei
//                                                 input:abi];
//            lastError = (long)[g_sdk JUB_LastError];
//            NSLog(@"[%li] JUB_SignTransactionETH(%li)\n", lastError, contextID);
//            if (JUBR_OK != lastError) {
//                return ;
//            }
//            NSLog(@"raw: %@\n", raw);
//        }
//        break;
//    }
    case 51:
    case 52:
    {
        int i = index - 50 - 1;
        NSString* data;
        if (1 == i) {
            NSString* tokenName = [NSString stringWithUTF8String:(char*)root["multiSig"]["5"][i]["ERC20"]["tokenName"].asCString()];
            NSUInteger dp = root["multiSig"]["5"][i]["ERC20"]["dp"].asUInt();
            NSString* contractAddress = [NSString stringWithUTF8String:(char*)root["multiSig"]["5"][i]["ERC20"]["contract_address"].asCString()];
            NSString* tokenTo = [NSString stringWithUTF8String:(char*)root["multiSig"]["5"][i]["ERC20"]["token_to"].asCString()];
            NSString* tokenValue = [NSString stringWithUTF8String:(char*)root["multiSig"]["5"][i]["ERC20"]["token_value"].asCString()];
            data = [g_sdk JUB_BuildERC20AbiETH:contextID
                                     tokenName:tokenName
                                        unitDP:dp
                               contractAddress:contractAddress
                                       tokenTo:tokenTo
                                    tokenValue:tokenValue];
            lastError = (long)[g_sdk JUB_LastError];
            NSLog(@"[%li] JUB_BuildERC20AbiETH(%li)\n", lastError, contextID);
            if (JUBR_OK != lastError) {
                return ;
            }
            NSLog(@"abi: %@\n", data);
        }
        else {
            data = [NSString stringWithUTF8String:(char*)root["multiSig"]["5"][i]["data"].asCString()];
        }
        // TransferTo
        NSString* methodID = [NSString stringWithUTF8String:(char*)root["multiSig"]["5"][i]["methodID"].asCString()];
        NSString* address = [NSString stringWithUTF8String:(char*)root["multiSig"]["5"][i]["address"].asCString()];
        NSString* amount = [NSString stringWithUTF8String:(char*)root["multiSig"]["5"][i]["amount"].asCString()];
        NSString* abi = [g_sdk JUB_BuildContractWithAddrAmtDataAbiETH:contextID
                                                             methodID:methodID
                                                              address:address
                                                               amount:amount
                                                                 data:data];
        lastError = (long)[g_sdk JUB_LastError];
        NSLog(@"[%li] JUB_BuildContractWithAddrAmtDataAbiETH(%li)\n", lastError, contextID);
        if (JUBR_OK != lastError) {
            return ;
        }
        NSLog(@"[%@] abi: %@\n", methodID, abi);
        
        NSString* raw = [g_sdk JUB_SignContractETH:contextID
                                              path:path
                                             nonce:nonce
                                          gasLimit:gasLimit
                                     gasPriceInWei:gasPriceInWei
                                                to:contractAddress
                                        valueInWei:valueInWei
                                             input:abi];
        lastError = (long)[g_sdk JUB_LastError];
        NSLog(@"[%li] JUB_SignTransactionETH(%li)\n", lastError, contextID);
        if (JUBR_OK != lastError) {
            return ;
        }
        NSLog(@"raw: %@\n", raw);
        break;
    }
    default:
        break;
    }
}
//0xf9016d8201638506c088e20083028d9a947a250d5630b4cf539739df2c5dacb4c659f2488d80b9010418cbafe50000000000000000000000000000000000000000000000004563918244f4000000000000000000000000000000000000000000000000000000030290f654ce6400000000000000000000000000000000000000000000000000000000000000a0000000000000000000000000d5e27985e3e7c008af184b9b78ddd9b75d32698c000000000000000000000000000000000000000000000000000000005f006cd700000000000000000000000000000000000000000000000000000000000000020000000000000000000000000f5d2fb29fb7d3cfee444a200298f468908cc942000000000000000000000000c02aaa39b223fe8d0a0e5c4f27ead9083c756cc225a0088ac49e5ccc674231b7efb1e0a23986fde792d02eb77e33dc4a29bcf4853742a020c8815aa535269afa1bc06f16f2769b93c922d589918fdf03af70ea6fb63b00
//0x
//f9 016d                                                               - length
//82 0163                                                               - nonce
//85 06c088e200                                                         - gas price
//83 028d9a                                                             - gas limit
//94 7a250d5630b4cf539739df2c5dacb4c659f2488d                           - to(contract address)
//80                                                                    - value
//b9 0104                                                               - contract length
//18cbafe5                                                              - contract MethodID
//0000000000000000000000000000000000000000000000004563918244f40000      - contract[0]
//00000000000000000000000000000000000000000000000000030290f654ce64      - contract[1]
//00000000000000000000000000000000000000000000000000000000000000a0      - contract[2]
//000000000000000000000000d5e27985e3e7c008af184b9b78ddd9b75d32698c      - contract[3]
//000000000000000000000000000000000000000000000000000000005f006cd7      - contract[4]
//0000000000000000000000000000000000000000000000000000000000000002      - contract[5]
//0000000000000000000000000f5d2fb29fb7d3cfee444a200298f468908cc942      - contract[6]
//000000000000000000000000c02aaa39b223fe8d0a0e5c4f27ead9083c756cc2      - contract[7]
//25                                                                    - v
//a0 088ac49e5ccc674231b7efb1e0a23986fde792d02eb77e33dc4a29bcf4853742   - r
//a0 20c8815aa535269afa1bc06f16f2769b93c922d589918fdf03af70ea6fb63b00   - s

