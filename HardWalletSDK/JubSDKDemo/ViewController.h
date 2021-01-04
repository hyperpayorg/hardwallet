//
//  ViewController.h
//  JubSDKDemo
//
//  Created by Pan Min on 2019/6/22.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Notification.h"
#import "JubSDKCore.h"

@interface ViewController : UIViewController<UITextFieldDelegate>
@property (nonatomic) JubSDKCore* _sdk;
@property (nonatomic) NSMutableDictionary* _deviceDict;
@property (nonatomic) NSString* _pwd;
@property (nonatomic) NSUInteger _deviceID;
@property (nonatomic) NSUInteger _contextID;


- (void)updateUI;

- (IBAction)enumAction:(UIButton *)sender;

- (IBAction)stopEnumAction:(UIButton *)sender;

- (void)deviceArrivalObserver:notification;

- (IBAction)loginAction:(UIButton *)sender;

@end
