//
//  test_xrp.mm
//  JubSDKCore
//
//  Created by Pan Min on 2019/11/29.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JubSDKCore.h"
#import "JubSDKCore+COIN_XRP.h"

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

void  get_address_pubkey_XRP(NSUInteger contextID);
void    transaction_test_XRP(NSUInteger contextID, Json::Value root);
void set_my_address_test_XRP(NSUInteger contextID);

// JUB_NS_XRP_TX_TYPE <- unsigned int
JUB_NS_XRP_TX_TYPE (^inlineNSXRPTxType)(unsigned int) = ^(unsigned int argument) {
    JUB_NS_XRP_TX_TYPE u;
    switch (argument) {
        case 0:
            u = NS_PYMT;
            break;
        default:
            u = NS_TX_XRP_TYPE_NS;
            break;
    }
    
    return u;
};

// JUB_NS_XRP_PYMT_TYPE <- unsigned int
JUB_NS_XRP_PYMT_TYPE (^inlineNSXRPPymtType)(unsigned int) = ^(unsigned int argument) {
    JUB_NS_XRP_PYMT_TYPE u;
    switch (argument) {
        case 0:
            u = NS_DXRP;
            break;
        default:
            u = NS_DXRP_XRP_TYPE_NS;
            break;
    }
    
    return u;
};

void XRP_test(NSUInteger deviceID, const char* json_file) {
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
    
    ContextConfigXRP* cfgXRP = [[ContextConfigXRP alloc] init];
    cfgXRP.mainPath = mainPath;
    NSUInteger contextID = [g_sdk JUB_CreateContextXRP:deviceID
                                                   cfg:cfgXRP];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_CreateContextXRP(%li)\n", lastError, deviceID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"ContextXRP: %li\n", contextID);
    
    while (true) {
        NSLog(@"|----------------------------------|\n");
        NSLog(@"|******* Jubiter Wallet XRP *******|\n");
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
                get_address_pubkey_XRP(contextID);
                break;
            case 2:
                transaction_test_XRP(contextID, root);
                break;
            case 4:
                set_my_address_test_XRP(contextID);
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

void get_address_pubkey_XRP(NSUInteger contextID) {
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
    NSString* address = [g_sdk JUB_GetAddressXRP:contextID
                                            path:path
                                           bShow:BOOL_NS_TRUE];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetAddressXRP(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"address: %@\n", address);
}

void transaction_test_XRP(NSUInteger contextID, Json::Value root) {
    NSUInteger lastError = JUBR_ERROR;
    
    lastError = verify_pin(contextID);
    if (JUBR_OK != lastError) {
        return;
    }
    
    //XRP Test
    TxXRP* tx = [[TxXRP alloc] init];
    
    tx.type = inlineNSXRPTxType((root["XRP"]["type"].asUInt()));
    
    tx.memo = [[XRPMemo alloc] init];
    tx.memo.type = [NSString stringWithUTF8String:(char*)root["XRP"]["memo"]["type"].asCString()];
    tx.memo.data = [NSString stringWithUTF8String:(char*)root["XRP"]["memo"]["data"].asCString()];
    tx.memo.format = [NSString stringWithUTF8String:(char*)root["XRP"]["memo"]["format"].asCString()];
    
    const char* sType = std::to_string(tx.type).c_str();
    switch (tx.type) {
        case JUB_NS_XRP_TX_TYPE::NS_PYMT:
        {
            tx.account   = [NSString stringWithUTF8String:(char*)root["XRP"]["account"].asCString()];
            tx.fee       = [NSString stringWithUTF8String:(char*)root["XRP"]["fee"].asCString()];
            tx.sequence  = [NSString stringWithUTF8String:(char*)root["XRP"]["sequence"].asCString()];
            tx.flags     = [NSString stringWithUTF8String:(char*)root["XRP"]["flags"].asCString()];
            tx.lastLedgerSequence = [NSString stringWithUTF8String:(char*)root["XRP"]["lastLedgerSequence"].asCString()];
            
            tx.pymt = [[PaymentXRP alloc] init];
            tx.pymt.type = inlineNSXRPPymtType((root["XRP"][sType]["type"].asUInt()));
//            const char* pymtType = std::to_string(tx.pymt.type).c_str();
            switch (tx.pymt.type) {
                case JUB_NS_XRP_PYMT_TYPE::NS_DXRP:
                {
                    tx.pymt.amount = [[PymtAmount alloc] init];
                    tx.pymt.amount.value   = [NSString stringWithUTF8String:(char*)root["XRP"][sType]["amount"]["value"].asCString()];
                    tx.pymt.destination    = [NSString stringWithUTF8String:(char*)root["XRP"][sType]["destination"].asCString()];
                    tx.pymt.destinationTag = [NSString stringWithUTF8String:(char*)root["XRP"][sType]["destinationTag"].asCString()];
                    break;
                }
                case JUB_NS_XRP_PYMT_TYPE::NS_DXRP_XRP_TYPE_NS:
                default:
                    break;
            }
            break;
        }
        case JUB_NS_XRP_TX_TYPE::NS_TX_XRP_TYPE_NS:
        default:
        {
            tx.accountTxnID = [NSString stringWithUTF8String:(char*)root["XRP"]["accountTxnID"].asCString()];
            tx.sourceTag = [NSString stringWithUTF8String:(char*)root["XRP"]["sourceTag"].asCString()];
            break;
        }
    }
    
    BIP32Path* path = [[BIP32Path alloc] init];
    if (root["XRP"]["bip32_path"]["change"].asBool()) {
        path.change = BOOL_NS_TRUE;
    }
    else {
        path.change = BOOL_NS_FALSE;
    }
    path.addressIndex = root["XRP"]["bip32_path"]["addressIndex"].asUInt();
    
    NSString* raw = [g_sdk JUB_SignTransactionXRP:contextID
                                             path:path
                                               tx:tx];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SignTransactionXRP(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"raw: %@\n", raw);
}

void set_my_address_test_XRP(NSUInteger contextID) {
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
    
    BIP32Path* path;
    path.change = JUB_NS_ENUM_BOOL(change);
    path.addressIndex = index;
    
    NSString* address = [g_sdk JUB_SetMyAddressXRP:contextID
                                              path:path];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_SetMyAddressXRP(%li)\n", lastError, contextID);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"set my address is: %@\n", address);
}
