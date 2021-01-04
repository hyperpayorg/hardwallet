//
//  test_hcash.mm
//  JubSDKCore
//
//  Created by Pan Min on 2019/8/5.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JubSDKCore.h"
#import "JubSDKCore+HCash.h"

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

void HcashMultisig_test(NSUInteger deviceID, const char* json_file);
void get_address_test_hcash(NSUInteger contextID, Json::Value root, JUB_NS_ENUM_BOOL bShow=JUB_NS_ENUM_BOOL::BOOL_NS_FALSE);
void transaction_test_hcash(NSUInteger contextID, Json::Value root);

void Hcash_test(NSUInteger deviceID, const char* json_file) {
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
    
    NSUInteger contextID = 0;
    try {
        NSString* mainPath = [NSString stringWithUTF8String:(char*)root["main_path"].asCString()];
        
        ContextConfigHC* cfgHC = [[ContextConfigHC alloc] init];
        cfgHC.mainPath = mainPath;
        contextID = [g_sdk JUB_CreateContextHC:deviceID
                                           cfg:cfgHC];
        lastError = (long)[g_sdk JUB_LastError];
        NSLog(@"[%li] JUB_CreateContextHC(%li)\n", lastError, deviceID);
        if (JUBR_OK != lastError) {
            return ;
        }
        NSLog(@"ContextHC: %li\n", contextID);
    }
    catch (...) {
        error_exit("Error format json file\n");
    }
    
    while (true) {
        NSLog(@"|------------------------------------|\n");
        NSLog(@"|******* Jubiter Wallet HCash *******|\n");
        NSLog(@"| 1. get_address_test.               |\n");
        NSLog(@"| 2. show_address_test.              |\n");
        NSLog(@"| 3. transaction_test.               |\n");
        NSLog(@"|                                    |\n");
        NSLog(@"| 9. return.                         |\n");
        NSLog(@"|------------------------------------|\n");
        NSLog(@"* Please enter your choice:           \n");
        
        int choice = 0;
        std::cin >> choice;
        switch (choice) {
            case 1:
                get_address_test_hcash(contextID, root);
                break;
            case 2:
                get_address_test_hcash(contextID, root, JUB_NS_ENUM_BOOL::BOOL_NS_TRUE);
                break;
            case 3:
                transaction_test_hcash(contextID, root);
                break;
            case 9:
                main_test();
                break;
            default:
                continue;
        }
    }
}

void get_address_test_hcash(NSUInteger contextID, Json::Value root, JUB_NS_ENUM_BOOL bShow) {
    NSUInteger lastError = JUBR_ERROR;
    
    NSLog(@"please input change level (non-zero means 1):\n");
    int change = 0;
    std::cin >> change;
    
    NSLog(@"please input index\n");
    unsigned long long index = 0;
    std::cin >> index;
    
    BIP32Path* path = [[BIP32Path alloc] init];
    if (0 == change) {
        path.change = BOOL_NS_FALSE;
    }
    else {
        path.change = BOOL_NS_TRUE;
    }
    path.addressIndex = index;
    
    try {
        NSString *mainXpub = [g_sdk JUB_GetMainHDNodeHC:contextID];
        lastError = (long)[g_sdk JUB_LastError];
        NSLog(@"[%li] JUB_GetMainHDNodeHC(%li)\n", lastError, contextID);
        if (JUBR_OK != lastError) {
            return ;
        }
        NSLog(@"main xpub: %@\n", mainXpub);
        
        NSString* xpub = [g_sdk JUB_GetHDNodeHC:contextID
                                           path:path];
        lastError = (long)[g_sdk JUB_LastError];
        NSLog(@"[%li] JUB_GetHDNodeHC(%li)\n", lastError, contextID);
        if (JUBR_OK != lastError) {
            return ;
        }
        NSLog(@"JUB_GetHDNodeHC is: %@\n", xpub);
        
        NSString* address = [g_sdk JUB_GetAddressHC:contextID
                                               path:path
                                              bShow:bShow];
        lastError = (long)[g_sdk JUB_LastError];
        NSLog(@"[%li] JUB_GetAddressHC(%li)\n", lastError, contextID);
        if (JUBR_OK != lastError) {
            return ;
        }
        NSLog(@"show address is: %@\n", address);
        
    }
    catch (...) {
        error_exit("Error format json file\n");
    }
}

InputHC* JSON2InputHC(int i, Json::Value root) {
    InputHC* input = [[InputHC alloc] init];
    
    if (root["inputs"][i]["bip32_path"]["change"].asBool()) {
        input.path.change = BOOL_NS_TRUE;
    }
    else {
        input.path.change = BOOL_NS_FALSE;
    }
    input.path.addressIndex = root["inputs"][i]["bip32_path"]["addressIndex"].asInt();
    input.amount = root["inputs"][i]["amount"].asUInt64();
    
    return input;
}

OutputHC* JSON2OutputHC(int i, Json::Value root) {
    OutputHC* output = [[OutputHC alloc] init];
    
    if (root["outputs"][i]["change_address"].asBool()) {
        output.isChangeAddress = BOOL_NS_TRUE;
        output.path.change = BOOL_NS_TRUE;
    }
    else {
        output.isChangeAddress = BOOL_NS_FALSE;
        output.path.change = BOOL_NS_FALSE;
    }
    output.path.addressIndex = root["outputs"][i]["bip32_path"]["addressIndex"].asInt();
    
    return output;
}

void transaction_test_hcash(NSUInteger contextID, Json::Value root) {
    NSUInteger lastError = JUBR_ERROR;
    
    lastError = verify_pin(contextID);
    if (JUBR_OK != lastError) {
        return;
    }
    
    NSMutableArray *ins = [NSMutableArray array];
    int inputNumber = root["inputs"].size();
    for (int i = 0; i < inputNumber; i++) {
        InputHC* input = JSON2InputHC(i, root);
        [ins addObject:input];
    }
    NSArray* inputs = [NSArray arrayWithArray:ins];
    
    NSMutableArray *outs = [NSMutableArray array];
    int outputNumber = root["outputs"].size();
    for (int i = 0; i < outputNumber; i++) {
        OutputHC* output = JSON2OutputHC(i, root);
        [outs addObject:output];
    }
    NSArray* outputs = [NSArray arrayWithArray:outs];
    
    NSString* unsignedTx = [NSString stringWithUTF8String:(char*)root["unsigned_tx"].asCString()];
    
    NSString* raw = [g_sdk JUB_SignTransactionHC:contextID
                                      inputArray:inputs
                                     outputArray:outputs
                                   unsignedTrans:unsignedTx];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SignTransactionHC(%li)\n", lastError, contextID);
    if (   JUBR_OK          == lastError
        || JUBR_MULTISIG_OK == lastError
        ) {
        NSLog(@"raw: %@\n", raw);
    }
}

void HcashMultisig_test(NSUInteger deviceID, const char* json_file) {
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
    
    NSUInteger contextID = 0;
    try {
        NSString* mainPath = [NSString stringWithUTF8String:(char*)root["main_path"].asCString()];
        
        ContextCfgMultisigHC* cfgHC = [[ContextCfgMultisigHC alloc] init];
        cfgHC.mainPath = mainPath;
        cfgHC.transType = (JUB_NS_BTC_TRANS_TYPE)root["trans_type"].asUInt();
        cfgHC.m = root["m"].asUInt();
        cfgHC.n = root["n"].asUInt();
        
        int cosignerNumber = root["cosigners"].size();
        cfgHC.vCosignerMainXpub = [[NSMutableArray alloc] init];
        for (int i = 0; i < cosignerNumber; i++) {
            std::string cosigner = root["cosigners"][i].asString();
            [cfgHC.vCosignerMainXpub addObject:[NSString stringWithCString:cosigner.c_str()
                                                                  encoding:[NSString defaultCStringEncoding]]];
        }
        
        contextID = [g_sdk JUB_CreateMultiSigContextHC:deviceID
                                                   cfg:cfgHC];
        lastError = (long)[g_sdk JUB_LastError];
        NSLog(@"[%li] JUB_CreateMultiSigContextHC(%li)\n", lastError, deviceID);
        if (JUBR_OK != lastError) {
            return ;
        }
        NSLog(@"MultiSigContextHC: %li\n", contextID);
    }
    catch (...) {
        error_exit("Error format json file\n");
    }
    
    while (true) {
        NSLog(@"|------------------------------------|\n");
        NSLog(@"|******* Jubiter Wallet HCash *******|\n");
        NSLog(@"| 1. get_address_test.               |\n");
        NSLog(@"| 2. show_address_test.              |\n");
        NSLog(@"| 3. transaction_test.               |\n");
        NSLog(@"|                                    |\n");
        NSLog(@"| 9. return.                         |\n");
        NSLog(@"|------------------------------------|\n");
        NSLog(@"* Please enter your choice:           \n");
        
        int choice = 0;
        std::cin >> choice;
        switch (choice) {
            case 1:
                get_address_test_hcash(contextID, root);
                break;
            case 2:
                get_address_test_hcash(contextID, root, JUB_NS_ENUM_BOOL::BOOL_NS_TRUE);
                break;
            case 3:
                transaction_test_hcash(contextID, root);
                break;
            case 9:
                main_test();
                break;
            default:
                continue;
        }
    }
}
