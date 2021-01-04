//
//  main.mm
//  JubSDKCore
//
//  Created by Pan Min on 2019/7/31.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JubSDKCore.h"
#import "JubSDKCore+DEV_HID.h"
#import "JubSDKCore+COIN_BTC.h"

#include "TrezorCrypto/TrezorCrypto.h"

#include <iostream>     // std::cout
#include <sstream>      // std::istringstream
#include <string>       // std::string
#include <vector>
#include <map>
#include "mSIGNA/stdutils/uchar_vector.h"
#include "utility/util.h"
#include "HDKey/HDKey.hpp"
#include "libbitcoin/machine/opcode.hpp"

#define USE_DEVICE    1

#define TREZOR_CRYPTO 0
#if TREZOR_CRYPTO
#import "test_trezor_crypto.h"
#endif // #if TREZOR_CRYPTO

using namespace std;

JubSDKCore* g_sdk;

void get_device_info_test(NSUInteger deviceID);
void   BTC_test(NSUInteger deviceID, const char* json_file, JUB_NS_ENUM_COINTYPE_BTC coinType);
void  USDT_test(NSUInteger deviceID, const char* json_file);
void  QTUM_test(NSUInteger deviceID, const char* json_file);
void   ETH_test(NSUInteger deviceID, const char* json_file);
void   ETH_contract_test(NSUInteger deviceID, const char* json_file);
void Hcash_test(NSUInteger deviceID, const char* json_file);
void HcashMultisig_test(NSUInteger deviceID, const char* json_file);
void   EOS_test(NSUInteger deviceID, const char* json_file);
void   XRP_test(NSUInteger deviceID, const char* json_file);

void error_exit(const char* message) {
    
    cout << message << endl;
    cout << "press any key to exit" << endl;
    char str[9] = { 0, };
    cin >> str;
    exit(0);
}

vector<string> split(const string &str, char delim, bool bSkipEmpty = true) {
    
    istringstream iss(str);
    vector<string> elems;
    for (string item; getline(iss, item, delim); ) {
        if (bSkipEmpty && item.empty()) {
            continue;
        }
        else {
            elems.push_back(item);
        }
    }
    
    return elems;
}

#define FROMHEX_MAXLEN 512

const uint8_t *fromhex(const char *str) {
    static uint8_t buf[FROMHEX_MAXLEN];
    size_t len = strlen(str) / 2;
    if (len > FROMHEX_MAXLEN) len = FROMHEX_MAXLEN;
    for (size_t i = 0; i < len; i++) {
        uint8_t c = 0;
        if (str[i * 2] >= '0' && str[i*2] <= '9') c += (str[i * 2] - '0') << 4;
        if ((str[i * 2] & ~0x20) >= 'A' && (str[i*2] & ~0x20) <= 'F') c += (10 + (str[i * 2] & ~0x20) - 'A') << 4;
        if (str[i * 2 + 1] >= '0' && str[i * 2 + 1] <= '9') c += (str[i * 2 + 1] - '0');
        if ((str[i * 2 + 1] & ~0x20) >= 'A' && (str[i * 2 + 1] & ~0x20) <= 'F') c += (10 + (str[i * 2 + 1] & ~0x20) - 'A');
        buf[i] = c;
    }
    return buf;
}

void main_test() {
    if (nil == g_sdk) {
        g_sdk = [[JubSDKCore alloc] init];
    }
    
    NSUInteger lastError = (long)[g_sdk JUB_LastError];
    
    NSString* version = [g_sdk JUB_GetVersion];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetVersion(void): \n", lastError);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSLog(@"GetVersion: %@\n", version);
    
#if USE_DEVICE
    NSArray* devices = [g_sdk JUB_ListDeviceHid];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_ListDeviceHid(void): \n", lastError);
    if (JUBR_OK != lastError) {
        return ;
    }
    NSUInteger deviceCnt = [devices count];
    NSLog(@"Hid Device count: %li\n", deviceCnt);
    
    if (0 == deviceCnt) {
        NSLog(@"No Hid devices...");
        exit(0);
    }
#endif // #if USE_DEVICE
    
    while (true) {
        NSLog(@"|-----------------------------------|\n");
        NSLog(@"|******* Jubiter Wallet Test *******|\n");
#if USE_DEVICE
        NSLog(@"|    1. get_device_info_test.       |\n");
        NSLog(@"|    2. send_one_apdu_test.         |\n");
        NSLog(@"|   31. BTC_test.                   |\n");
        NSLog(@"|   32. BTC_segwit_test.            |\n");
        NSLog(@"|   33. BTC_mutilsign_test.         |\n");
        NSLog(@"|   39. BTC-USDT_test.              |\n");
        NSLog(@"| 145 . BCH_test.                   |\n");
        NSLog(@"| 1453. BCH_mutilsign_test.         |\n");
        NSLog(@"|   21. LTC_test.                   |\n");
        NSLog(@"|   23. LTC_mutilsign_test.         |\n");
        NSLog(@"|   60. ETH_test & ETC_test.        |\n");
        NSLog(@"|   61. ETH_contract_create_test.   |\n");
        NSLog(@"|   63. ETH_contract_test.          |\n");
        NSLog(@"|   64. ETH_UniswapV2Router02_test. |\n");
        NSLog(@"| 171 . hcash_test.                 |\n");
        NSLog(@"| 1713. hcash_mutilsign_test.       |\n");
        NSLog(@"|                                   |\n");
        NSLog(@"|   88. QTUM_QRC20_test.            |\n");
        NSLog(@"| 2301. QTUM_test.                  |\n");
        NSLog(@"|  144.  XRP_test.                  |\n");
        NSLog(@"|  194.  EOS_test.                  |\n");
#endif // #if USE_DEVICE
        NSLog(@"|                                   |\n");
#if TREZOR_CRYPTO
        NSLog(@"|   91. derive xpub key from xpub.  |\n");
        NSLog(@"|   92. redeem script.              |\n");
        NSLog(@"|   93. address code test.          |\n");
        NSLog(@"|   98. hd addr from mnemonic test. |\n");
        NSLog(@"|   99. hd from mnemonic test.      |\n");
#endif // #if TREZOR_CRYPTO
        NSLog(@"|    0. exit.                       |\n");
        NSLog(@"|-----------------------------------|\n");
        NSLog(@"* Please enter your choice:          \n");
        
        int choice = 0;
        std::cin >> choice;
        if (0 == choice) {
            exit(0);
        }
        
#if USE_DEVICE
        NSUInteger deviceID = [devices[0] unsignedIntegerValue];
//        for (NSUInteger i=0; i<deviceCnt; ++i) {
//            [g_sdk JUB_DisconnetDeviceHid:[devices[i] unsignedIntegerValue]];
//        }
        [g_sdk JUB_DisconnetDeviceHid:deviceID];
        
        [g_sdk JUB_ConnetDeviceHid:deviceID];
        lastError = (long)[g_sdk JUB_LastError];
        NSLog(@"[%li] JUB_ConnetDeviceHid(%li)\n", lastError, deviceID);
        if (JUBR_OK != lastError) {
            return;
        }
#endif // #if USE_DEVICE
        
        switch (choice) {
            case 0:
                exit(0);
#if USE_DEVICE
            case 1:
                get_device_info_test(deviceID);
                break;
            case 2:
                break;
            case 31:
                BTC_test(deviceID, "json/testBTC44.json", NS_COINBTC);
                break;
            case 32:
                BTC_test(deviceID, "json/testBTC49.json", NS_COINBTC);
                break;
            case 33:
//                BTC_test(deviceID, "json/testBTC45_1.json", NS_COINBTC);
                BTC_test(deviceID, "json/testBTC45_2.json", NS_COINBTC);
                break;
            case 145:
                BTC_test(deviceID, "json/testBCH.json", NS_COINBCH);
                break;
            case 1453:
//                BTC_test(deviceID, "json/testBCH45_2-1.json", NS_COINBCH);
//                BTC_test(deviceID, "json/testBCH45_2-2.json", NS_COINBCH);
//                BTC_test(deviceID, "json/testBCH45_3-5-1.json", NS_COINBCH);
//                BTC_test(deviceID, "json/testBCH45_3-5-2.json", NS_COINBCH);
                BTC_test(deviceID, "json/testBCH45_3-5-3.json", NS_COINBCH);
                break;
            case 21:
                BTC_test(deviceID, "json/testLTC.json", NS_COINLTC);
                break;
            case 23:
//                BTC_test(deviceID, "json/testLTC45_1.json", NS_COINLTC);
                BTC_test(deviceID, "json/testLTC45_2.json", NS_COINLTC);
                break;
            case 39:
                USDT_test(deviceID, "json/testUSDT.json");
                break;
            case 60:
//                ETH_test(deviceID, "json/testETH.json");
                ETH_test(deviceID, "json/testETH_newToken.json");
                break;
            case 61:
                ETH_contract_test(deviceID, "json/testETH_multiSig_create.json");
                break;
            case 63:
                ETH_contract_test(deviceID, "json/testETH_multiSig_op.json");
                break;
            case 64:
                ETH_test(deviceID, "json/testETH_UniswapV2Router02.json");
                break;
            case 171:
                Hcash_test(deviceID, "json/testHCash.json");
                break;
            case 1713:
//                HcashMultisig_test(deviceID, "json/testHCash_multisig.json");
                HcashMultisig_test(deviceID, "json/testHCash_multisig_2.json");
                break;
            case 88:
                QTUM_test(deviceID, "json/testQTUM_qrc20.json");
                break;
            case 2301:
                BTC_test(deviceID, "json/testQTUM.json", NS_COINQTUM);
                break;
            case 144:
                XRP_test(deviceID, "json/testXRP.json");
                break;
            case 194:
                EOS_test(deviceID, "json/testEOS_huobi.json");
//                EOS_test(deviceID, "json/testEOS.json");
//                EOS_test(deviceID, "json/testEOS_long.json");
//                EOS_test(deviceID, "json/testEOS_stake.json");
//                EOS_test(deviceID, "json/testEOS_unstake.json");
//                EOS_test(deviceID, "json/testEOS_buyram.json");
//                EOS_test(deviceID, "json/testEOS_sellram.json");
//                EOS_test(deviceID, "json/testEOS_mix.json");
                break;
#endif // #if USE_DEVICE
#if TREZOR_CRYPTO
            case 91:
                hdFromXpub();
                break;
            case 92:
                redeemScript();
                break;
            case 93:
                address_code_test();
                break;
            case 98:
                hdFromMnemonicBip45();
                break;
            case 99:
                loopHdFromMnemonicBip45();
                break;
#endif // #if TREZOR_CRYPTO
            default:
                continue;
        }
    }
}

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // insert code here...
        NSLog(@"Hello, World!");
        main_test();
    }
    return 0;
}
