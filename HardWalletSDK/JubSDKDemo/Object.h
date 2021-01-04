//
//  Object.h
//  JubSDKDemo
//
//  Created by Pan Min on 2019/6/26.
//  Copyright © 2019 JuBiter. All rights reserved.
//

//#ifndef Object_h
//#define Object_h
#import <Foundation/Foundation.h>

@interface ScanDeviceInfo : NSObject
@property (nonatomic, strong) NSString* name;
@property (nonatomic, strong) NSString* uuid;
@property (nonatomic, assign) NSUInteger type;
@end

//#endif /* Object_h */
