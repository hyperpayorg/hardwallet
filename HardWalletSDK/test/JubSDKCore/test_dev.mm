//
//  test_dev.mm
//  JubSDKCore
//
//  Created by Pan Min on 2019/8/1.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JubSDKCore.h"
#import "JubSDKCore+DEV.h"
#import "JubSDKCore+DEV_HID.h"

#include <iostream>     // std::cout
#include <sstream>      // std::istringstream
#include <string>       // std::string
#include <vector>

using namespace std;

extern JubSDKCore* g_sdk;

vector<string> split(const string &str, char delim, bool bSkipEmpty = true);

void get_device_info_test(NSUInteger deviceID) {
    NSUInteger lastError = JUBR_ERROR;
    
    NSString *appletList = [g_sdk JUB_EnumApplets:deviceID];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_EnumApplets(%li) return %@\n", lastError, deviceID, appletList);
    if (JUBR_OK != lastError) {
        return;
    }
    
    auto vAppletList = split((char*)[appletList UTF8String], ' ');
    for (auto appletID : vAppletList) {
        NSString* version = [g_sdk JUB_GetAppletVersion:deviceID
                                                  appID:[NSString stringWithUTF8String:appletID.c_str()]];
        lastError = (long)[g_sdk JUB_LastError];
        NSLog(@"Applet Version %@\n", version);
    }
    
    DeviceInfo *devInfo = [g_sdk JUB_GetDeviceInfo:deviceID];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetDeviceInfo(%li)\n", lastError, deviceID);
    if (JUBR_OK != lastError) {
        return;
    }
    NSLog(@"device Label: %@\n", devInfo.label);
    NSLog(@"device SN:    %@\n", devInfo.sn);
    NSLog(@"device pinRetry:    %li\n", devInfo.pinRetry);
    NSLog(@"device pinMaxRetry: %li\n", devInfo.pinMaxRetry);
    NSLog(@"device      bleVersion: %@\n", devInfo.bleVersion);
    NSLog(@"device firmwareVersion: %@\n", devInfo.firmwareVersion);
    
    NSString* cert = [g_sdk JUB_GetDeviceCert:deviceID];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetDeviceCert(%li)\n", lastError, deviceID);
    if (JUBR_OK != lastError) {
        return;
    }
    NSLog(@"Device Cert: %@\n", cert);
    
    NSString* coinList = [g_sdk Jub_EnumSupportCoins:deviceID];
    lastError = (long)[g_sdk JUB_LastError];
    NSLog(@"[%li] Jub_EnumSupportCoins(%li)\n", lastError, deviceID);
    if (JUBR_OK != lastError) {
        return;
    }
    NSLog(@"Coin List: %@\n", coinList);
}

NSUInteger verify_pin(NSUInteger contextID) {
    NSUInteger lastError = JUBR_ERROR;
    
    while (lastError) {
        NSLog(@"1 2 3\n");
        NSLog(@"4 5 6\n");
        NSLog(@"7 8 9\n");
        NSLog(@"To cancel the virtualpwd iput 'c'\n");
        
        [g_sdk JUB_ShowVirtualPwd:contextID];
        lastError = (long)[g_sdk JUB_LastError];
        NSLog(@"[%li] JUB_ShowVirtualPwd(%li)\n", lastError, contextID);
        
        char pin[9] = {0,};;
        std::cin >> pin;
        NSLog(@"PIN: %s\n", pin);
        if (   'c' == pin[0]
            || 'C' == pin[0]
            ) {
            NSLog(@"cancel the VirtualPwd\n");
            [g_sdk JUB_CancelVirtualPwd:contextID];
            lastError = (long)[g_sdk JUB_LastError];
            NSLog(@"[%li] JUB_CancelVirtualPwd(%li)\n", lastError, contextID);
            if (JUBR_OK != lastError) {
                break;
            }
            
            return lastError;
        }
        
        NSUInteger retry = [g_sdk JUB_VerifyPIN:contextID
                                         pinMix:[NSString stringWithUTF8String:pin]];
        NSLog(@"[%li] JUB_VerifyPIN(%li)\n", lastError, contextID);
        if (JUBR_OK != lastError) {
            NSLog(@"retry = %li\n", retry);
            break;
        }
    }
    
    return lastError;
}

void set_timeout_test(NSUInteger contextID) {
    
    NSLog(@"* Please enter timeout in second ( < 600 ):\n");
    
    int timeout = 0;
    std::cin >> timeout;
    
    [g_sdk JUB_SetTimeOut:contextID
                  timeout:timeout];
}
