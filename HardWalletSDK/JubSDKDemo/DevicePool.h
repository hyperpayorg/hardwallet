//
//  DevicePool.h
//  JubSDKDemo
//
//  Created by ft on 2019/6/22.
//  Copyright © 2019 ft. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>

@interface DevicePool : NSObject<CBCentralManagerDelegate>
+ (instancetype)shareDevicePool;
@end
