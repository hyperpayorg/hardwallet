//
//  DevicePool.m
//  JubSDKDemo
//
//  Created by Pan Min on 2019/6/24.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>
#import "Notification.h"

@interface DevicePool : NSObject<CBCentralManagerDelegate>
@property (nonatomic, strong) CBCentralManager *central;
@end

static DevicePool *g_devicePool = nil;

@implementation DevicePool

+ (instancetype) shareDevicePool {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        g_devicePool = [[self alloc] init];
    });
    return g_devicePool;
}

- (instancetype) init {
    if (self = [super init]) {
        self.central = [[CBCentralManager alloc] initWithDelegate:self
                                                            queue:nil];
    }
    return self;
}

- (void) centralManagerDidUpdateState:(CBCentralManager *)central {
    
//    NSLog(@"当前蓝牙状态： %ld", central.state);
    switch (central.state) {
        case CBManagerStateUnknown:
            NSLog(@"[当前蓝牙状态： %@]", @"CBManagerStateUnknown");
            break;
        case CBManagerStateResetting:
            NSLog(@"[当前蓝牙状态： %@]", @"CBManagerStateResetting");
            break;
        case CBManagerStateUnsupported:
            NSLog(@"[当前蓝牙状态： %@]", @"CBManagerStateUnsupported");
            break;
        case CBManagerStateUnauthorized:
            NSLog(@"[当前蓝牙状态： %@]", @"CBManagerStateUnauthorized");
            break;
        case CBManagerStatePoweredOff:
            NSLog(@"[当前蓝牙状态： %@]", @"CBManagerStatePoweredOff");
            break;
        case CBManagerStatePoweredOn:
            NSLog(@"[当前蓝牙状态： %@]", @"CBManagerStatePoweredOn");
            break;
    }
    
    [[NSNotificationCenter defaultCenter] postNotificationName:NOTIF_DEVICE_ARRVL
                                                        object:self
                                                      userInfo:@{@"name":@(central.state)}];
}

@end


