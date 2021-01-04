//
//  test_eos.mm
//  JubSDKCore
//
//  Created by Pan Min on 2019/10/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JubSDKCore.h"
#import "JubSDKCore+COIN_EOS.h"

#include <json/json.h>
#include <iostream>     // std::cout
#include <sstream>      // std::istringstream
#include <string>       // std::string
#include <vector>
#include <fstream>
#include <ctime>

using namespace std;

extern JubSDKCore* g_sdk;

void error_exit(const char* message);
void main_test();

NSUInteger verify_pin(NSUInteger contextID);
void set_timeout_test(NSUInteger contextID);

void  get_address_pubkey_EOS(NSUInteger contextID);
void    transaction_test_EOS(NSUInteger contextID, Json::Value root);
void set_my_address_test_EOS(NSUInteger contextID);

// JUB_NS_EOS_ACTION_TYPE <- unsigned int
JUB_NS_EOS_ACTION_TYPE (^inlineNSEOSActionType)(unsigned int) = ^(unsigned int argument) {
    JUB_NS_EOS_ACTION_TYPE u;
    switch (argument) {
        case 0x00:
            u = NS_XFER;
            break;
        case 0x01:
            u = NS_DELE;
            break;
        case 0x02:
            u = NS_UNDELE;
            break;
        case 0x03:
            u = NS_BUYRAM;
            break;
        case 0x04:
            u = NS_SELLRAM;
            break;
        default:
            u = NS_ITEM_ACTION_TYPE_NS;
            break;
    }

    return u;
};

void EOS_test(NSUInteger deviceID, const char* json_file) {
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
    
    ContextConfigEOS* cfgEOS = [[ContextConfigEOS alloc] init];
    cfgEOS.mainPath = mainPath;
    NSUInteger contextID = [g_sdk JUB_CreateContextEOS:deviceID
                                                   cfg:cfgEOS];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_CreateContextEOS(%li)\n", lastError, deviceID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"ContextEOS: %li\n", contextID);
    
    while (true) {
        NSLog(@"|----------------------------------|\n");
        NSLog(@"|******* Jubiter Wallet EOS *******|\n");
        NSLog(@"| 1. show_address_pubkey_test.     |\n");
        NSLog(@"| 2. transaction_test.             |\n");
        NSLog(@"| 4. set_my_address_test.          |\n");
        NSLog(@"| 5. set_timeout_test.             |\n");
        NSLog(@"|                                  |\n");
        NSLog(@"| 9. return.                       |\n");
        NSLog(@"|----------------------------------|\n");
        NSLog(@"* Please enter your choice:         \n");
        
        int choice = 0;
        std::cin >> choice;
        switch (choice) {
            case 1:
                get_address_pubkey_EOS(contextID);
                break;
            case 2:
                transaction_test_EOS(contextID, root);
                break;
            case 4:
                set_my_address_test_EOS(contextID);
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

void get_address_pubkey_EOS(NSUInteger contextID) {
    NSUInteger lastError = JUBR_ERROR;
    
    NSLog(@"please input change level (non-zero means 1):\n");
    int change = 0;
    cin >> change;
    NSLog(@"please input index \n");
    long index = 0;
    cin >> index;
    
    BIP32Path* path = [[BIP32Path alloc] init];
    path.change = JUB_NS_ENUM_BOOL(change);
    path.addressIndex = index;
    NSString* address = [g_sdk JUB_GetAddressEOS:contextID
                                            path:path
                                           bShow:BOOL_NS_TRUE];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetAddressEOS(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"address: %@\n", address);
}

void transaction_test_EOS(NSUInteger contextID, Json::Value root) {
    NSUInteger lastError = JUBR_ERROR;
    
    lastError = verify_pin(contextID);
    if (JUBR_OK != lastError) {
        return;
    }
    
    //EOS Test
    NSMutableArray* actionArray = [NSMutableArray array];
    for (Json::Value::iterator it = root["EOS"]["actions"].begin(); it != root["EOS"]["actions"].end(); ++it) {//NSString
        ActionEOS* action = [[ActionEOS alloc] init];
        
        action.type = inlineNSEOSActionType((*it)["type"].asUInt());
        action.currency = [NSString stringWithUTF8String:(char*)(*it)["currency"].asCString()];
        action.name     = [NSString stringWithUTF8String:(char*)(*it)["name"].asCString()];
        
        const char* sType = std::to_string(action.type).c_str();
        switch (action.type) {
            case JUB_NS_EOS_ACTION_TYPE::NS_XFER:
                action.xfer = [[XferAction alloc] init];
                action.xfer.from  = [NSString stringWithUTF8String:(char*)(*it)[sType]["from"].asCString()];
                action.xfer.to    = [NSString stringWithUTF8String:(char*)(*it)[sType]["to"].asCString()];
                action.xfer.asset = [NSString stringWithUTF8String:(char*)(*it)[sType]["asset"].asCString()];
                action.xfer.memo  = [NSString stringWithUTF8String:(char*)(*it)[sType]["memo"].asCString()];
                break;
            case JUB_NS_EOS_ACTION_TYPE::NS_DELE:
                action.dele = [[DeleAction alloc] init];
                action.dele.from     = [NSString stringWithUTF8String:(char*)(*it)[sType]["from"].asCString()];
                action.dele.receiver = [NSString stringWithUTF8String:(char*)(*it)[sType]["receiver"].asCString()];
                action.dele.netQty = [NSString stringWithUTF8String:(char*)(*it)[sType]["stake_net_quantity"].asCString()];
                action.dele.cpuQty = [NSString stringWithUTF8String:(char*)(*it)[sType]["stake_cpu_quantity"].asCString()];
                action.dele.bStake = JUB_NS_ENUM_BOOL::BOOL_NS_TRUE;
                break;
            case JUB_NS_EOS_ACTION_TYPE::NS_UNDELE:
                action.dele = [[DeleAction alloc] init];
                action.dele.from     = [NSString stringWithUTF8String:(char*)(*it)[sType]["from"].asCString()];
                action.dele.receiver = [NSString stringWithUTF8String:(char*)(*it)[sType]["receiver"].asCString()];
                action.dele.netQty = [NSString stringWithUTF8String:(char*)(*it)[sType]["unstake_net_quantity"].asCString()];
                action.dele.cpuQty = [NSString stringWithUTF8String:(char*)(*it)[sType]["unstake_cpu_quantity"].asCString()];
                action.dele.bStake = JUB_NS_ENUM_BOOL::BOOL_NS_FALSE;
                break;
            case JUB_NS_EOS_ACTION_TYPE::NS_BUYRAM:
                action.buyRam = [[BuyRamAction alloc] init];
                action.buyRam.payer    = [NSString stringWithUTF8String:(char*)(*it)[sType]["payer"].asCString()];
                action.buyRam.quant    = [NSString stringWithUTF8String:(char*)(*it)[sType]["quant"].asCString()];
                action.buyRam.receiver = [NSString stringWithUTF8String:(char*)(*it)[sType]["receiver"].asCString()];
                break;
            case JUB_NS_EOS_ACTION_TYPE::NS_SELLRAM:
                action.sellRam = [[SellRamAction alloc] init];
                action.sellRam.account = [NSString stringWithUTF8String:(char*)(*it)[sType]["account"].asCString()];
                action.sellRam.bytes   = [NSString stringWithUTF8String:(char*)(*it)[sType]["bytes"].asCString()];
                break;
            case JUB_NS_EOS_ACTION_TYPE::NS_ITEM_ACTION_TYPE_NS:
            default:
                break;
        }
        [actionArray addObject:action];
    }
    NSString* actions = [g_sdk JUB_BuildActionEOS:contextID
                                           action:actionArray];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_BuildActionEOS(%li)\n", lastError, contextID);
    NSLog(@"[actions]: %@\n", actions);
    
    BIP32Path* path = [[BIP32Path alloc] init];
    if (root["EOS"]["bip32_path"]["change"].asBool()) {
        path.change = BOOL_NS_TRUE;
    }
    else {
        path.change = BOOL_NS_FALSE;
    }
    path.addressIndex = root["EOS"]["bip32_path"]["addressIndex"].asUInt();
    
    NSString* chainID  = [NSString stringWithUTF8String:(char*)root["EOS"]["chainID"].asCString()];
    NSString* expiration  = [NSString stringWithUTF8String:(char*)root["EOS"]["expiration"].asCString()];
    
    NSString*  referenceBlockId  = [NSString stringWithUTF8String:(char*)root["EOS"]["referenceBlockId"].asCString()];
    
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    NSDate *dateBlockTime = [formatter dateFromString:[NSString stringWithUTF8String:(char*)root["EOS"]["referenceBlockTime"].asCString()]];
    NSTimeZone *zone = [NSTimeZone localTimeZone];
    NSInteger zoneTime = [zone secondsFromGMT];
    NSDate* dateSum = [dateBlockTime dateByAddingTimeInterval:zoneTime];
    NSInteger refBlockTime = [dateSum timeIntervalSince1970];
    NSString* referenceBlockTime = [NSString stringWithFormat: @"%ld", refBlockTime];
    NSString* raw = [g_sdk JUB_SignTransactionEOS:contextID
                                             path:path
                                          chainID:@""
                                       expiration:expiration
                                 referenceBlockId:referenceBlockId
                               referenceBlockTime:referenceBlockTime
                                           action:actions];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SignTransactionEOS(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"raw: %@\n", raw);
}

void set_my_address_test_EOS(NSUInteger contextID) {
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
    
    NSString* address = [g_sdk JUB_SetMyAddressEOS:contextID
                                              path:path];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SetMyAddressEOS(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"set my address is: %@\n", address);
}

//"transaction declares authority '{"actor":"zijunzimo555","permission":"active"}', but does not have signatures for it under a provided delay of 0 ms, provided permissions [], provided keys ["EOS7aRgvZzzzrReT4DWUs73ugdB2xiuZmxnwKacBcGuqQYo4zmV6y"], and a delay max limit of 3888000000 ms"
//"file":"authorization_manager.cpp"
//"line_number":524
//"method":"check_authorization"
