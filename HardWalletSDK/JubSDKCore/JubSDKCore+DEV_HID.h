//
//  JubSDKCore+DEV_HID.h
//  JubSDKCore
//
//  Created by Pan Min on 2019/7/17.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "JubSDKCore.h"

NS_ASSUME_NONNULL_BEGIN

@interface JubSDKCore (DEV_HID)

//JUB_RV JUB_ListDeviceHid(OUT JUB_UINT16 deviceIDs[MAX_DEVICE]);
- (NSArray*)JUB_ListDeviceHid;

//JUB_RV JUB_ConnetDeviceHid(IN JUB_UINT16 deviceID);
- (void)JUB_ConnetDeviceHid:(NSUInteger)deviceID;

//JUB_RV JUB_DisconnetDeviceHid(IN JUB_UINT16 deviceID);
- (void)JUB_DisconnetDeviceHid:(NSUInteger)deviceID;

@end

NS_ASSUME_NONNULL_END
