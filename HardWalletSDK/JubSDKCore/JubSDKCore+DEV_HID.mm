//
//  JubSDKCore+DEV_HID.mm
//  JubSDKCore
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore+DEV_HID.h"
#import "JUB_SDK_DEV_HID.h"

@implementation JubSDKCore (DEV_HID)

//JUB_RV JUB_ListDeviceHid(OUT JUB_UINT16 deviceIDs[MAX_DEVICE]);
- (NSArray*)JUB_ListDeviceHid
{
    self.lastError = JUBR_OK;
    
    JUB_UINT16 deviceIDs[MAX_DEVICE];
    JUB_RV rv = JUB_ListDeviceHid(deviceIDs);
    if (JUBR_OK != rv) {
        self.lastError = rv;
        return @[];
    }
    
    NSMutableArray *deviceIDArray =  [NSMutableArray arrayWithCapacity:MAX_DEVICE];
    for (int i=0; i<MAX_DEVICE; ++i) {
        if (0 != deviceIDs[i]) {
            [deviceIDArray addObject:[NSNumber numberWithInt:deviceIDs[i]]];
        }
    }
    
    return deviceIDArray;
}

//JUB_RV JUB_ConnetDeviceHid(IN JUB_UINT16 deviceID);
- (void)JUB_ConnetDeviceHid:(NSUInteger)deviceID
{
    self.lastError = JUB_ConnetDeviceHid(deviceID);
}

//JUB_RV JUB_DisconnetDeviceHid(IN JUB_UINT16 deviceID);
- (void)JUB_DisconnetDeviceHid:(NSUInteger)deviceID
{
    self.lastError = JUB_DisconnetDeviceHid(deviceID);
}

@end
