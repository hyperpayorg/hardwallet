//
//  test_usdt.mm
//  JubSDKCore
//
//  Created by Pan Min on 2019/9/20.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JubSDKCore.h"
#import "JubSDKCore+DEV.h"
#import "JubSDKCore+COIN_BTC.h"

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

void     get_address_test(NSUInteger contextID, Json::Value root);
void    show_address_test(NSUInteger contextID);
void     transaction_test(NSUInteger contextID, Json::Value root);
void transactionUSDT_test(NSUInteger contextID, Json::Value root);
void set_my_address_test_BTC(NSUInteger contextID);

 InputBTC*  JSON2InputBTC(int i, Json::Value root);
OutputBTC* JSON2OutputBTC(int i, Json::Value root);

void USDT_test(NSUInteger deviceID,
               const char* json_file) {
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
        
        ContextConfigBTC* cfgBTC = [[ContextConfigBTC alloc] init];
        cfgBTC.mainPath = mainPath;
        cfgBTC.coinType = NS_COINUSDT;
        cfgBTC.transType = ns_p2pkh;
        contextID = [g_sdk JUB_CreateContextBTC:deviceID
                                            cfg:cfgBTC];
        lastError = (long)[g_sdk JUB_LastError];
        NSLog(@"[%li] JUB_CreateContextBTC(%li)\n", lastError, deviceID);
        if (JUBR_OK != lastError) {
            return ;
        }
        NSLog(@"ContextBTC: %li\n", contextID);
    }
    catch (...) {
        error_exit("Error format json file\n");
    }
    
    while (true) {
        NSLog(@"|------------------------------|\n");
        NSLog(@"|***** Jubiter Wallet USDT ****|\n");
        NSLog(@"| 1. get_address_test.         |\n");
        NSLog(@"| 2. show_address_test.        |\n");
        NSLog(@"| 3. transaction_test.         |\n");
        NSLog(@"| 4. set_my_address_test.      |\n");
        NSLog(@"| 5. set_timeout_test.         |\n");
        NSLog(@"|                              |\n");
        NSLog(@"| 9. return.                   |\n");
        NSLog(@"|------------------------------|\n");
        NSLog(@"* Please enter your choice:     \n");

        int choice = 0;
        cin >> choice;

        switch (choice) {
            case 1:
                get_address_test(contextID, root);
                break;
            case 2:
                show_address_test(contextID);
                break;
            case 3:
                transactionUSDT_test(contextID, root);
                break;
            case 4:
                set_my_address_test_BTC(contextID);
                break;
            case 5:
                set_timeout_test(contextID);
                break;
            case 9:
                main_test();
            default:
                continue;
        }
    }
}

void transactionUSDT_test(NSUInteger contextID, Json::Value root) {
    NSUInteger lastError = JUBR_ERROR;
    
    lastError = verify_pin(contextID);
    if (JUBR_OK != lastError) {
        return;
    }
    
    try {
        NSMutableArray *ins = [NSMutableArray array];
        int inputNumber = root["inputs"].size();
        for (int i = 0; i < inputNumber; i++) {
            InputBTC* input = JSON2InputBTC(i, root);
            [ins addObject:input];
        }
        NSArray* inputs = [NSArray arrayWithArray:ins];
        
        NSMutableArray *outs = [NSMutableArray array];
        int outputNumber = root["outputs"].size();
        for (int i = 0; i < outputNumber; i++) {
            OutputBTC* output = JSON2OutputBTC(i, root);
            [outs addObject:output];
        }
        
        NSArray *usdtouts = [g_sdk JUB_BuildUSDTOutputs:contextID
                                                 USDTTo:[NSString stringWithUTF8String:(char*)root["USDT_to"].asCString()]
                                                 amount:root["USDT_amount"].asUInt64()];
        lastError = (long)[g_sdk JUB_LastError];
        NSLog(@"[%li] JUB_BuildUSDTOutputs(%li)\n", lastError, contextID);
        if (JUBR_OK != lastError) {
            return ;
        }
        NSLog(@"USDT output size %li\n", [usdtouts count]);
        
        for (NSUInteger i=0; i<[usdtouts count]; ++i) {
            [outs addObject:usdtouts[i]];
        }
        NSArray* outputs = [NSArray arrayWithArray:outs];
        
        NSString* raw = [g_sdk JUB_SignTransactionBTC:contextID
                                           inputArray:inputs
                                          outputArray:outputs
                                             lockTime:0];
        lastError = (long)[g_sdk JUB_LastError];
        NSLog(@"[%li] JUB_SignTransactionBTC(%li)\n", lastError, contextID);
        if (JUBR_USER_CANCEL == lastError) {
            NSLog(@"User cancel the transaction !\n");
            return;
        }
        if (JUBR_OK != lastError) {
            return ;
        }
        NSLog(@"USDT raw: %@\n", raw);
    }
    catch (...) {
        error_exit("Error format json file\n");
    }
}
