//
//  ViewController.m
//  JubSDKDemo
//
//  Created by Pan Min on 2019/6/22.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import "ViewController.h"
#import "JUB_SDK.h"
#import "DevicePool.h"
#import "Notification.h"
#import "Object.h"
#import "JubSDKCore+COIN_ETH.h"
#import "JubSDKCore+DEV_BLE.h"

@interface ViewController ()

@property (weak, nonatomic) IBOutlet UITableView *tableview;
@property (nonatomic, strong) NSMutableArray *scanArray;


@end

@implementation ViewController
@synthesize _sdk;
@synthesize _deviceDict;

void Scan(unsigned char* devName, unsigned char* uuid, unsigned int type) {
    NSLog(@"Scan: [%s:%s]", devName, uuid);
    ScanDeviceInfo* deviceInfo = [[ScanDeviceInfo alloc] init];
    deviceInfo.name = [NSString stringWithCString:(char*)devName
                                         encoding:NSUTF8StringEncoding];
    deviceInfo.uuid = [NSString stringWithCString:(char*)uuid
                                         encoding:NSUTF8StringEncoding];
    deviceInfo.type = NSUInteger(type);
    [[NSNotificationCenter defaultCenter] postNotificationName:NOTIF_DEVICE_SCAN
                                                        object:nil
                                                      userInfo:@{@"uuid":[NSString stringWithCString:(char*)uuid
                                                                                            encoding:NSUTF8StringEncoding],
                                                                 @"device":deviceInfo
                                                                 }];    
}

void Disc(unsigned char* uuid) {
    NSLog(@"Disc: [%s]", uuid);
    [[NSNotificationCenter defaultCenter] postNotificationName:NOTIF_DEVICE_DISC
                                                        object:nil
                                                      userInfo:@{@"uuid":[NSString stringWithCString:(char*)uuid
                                                                                            encoding:NSUTF8StringEncoding]
                                                                 }];
}

- (void)deviceArrivalObserver:notification {
    dispatch_async(dispatch_get_main_queue(), ^{
        [self updateUI];
        
        DeviceInitParam* init = [[DeviceInitParam alloc] init];
        init.readCallBack = nil;
        init.scanCallBack = &Scan;
        init.discCallBack = &Disc;
        [self._sdk JUB_initDevice:init];
        NSLog(@"[%li] JUB_initDevice(void)", (long)[self._sdk JUB_LastError]);
    });
}

- (void)deviceScanObserver:(NSNotification *) notification {
    if (nil == [_deviceDict objectForKey:[[notification userInfo] objectForKey:@"uuid"]]) {
        
        [_deviceDict setObject:[[notification userInfo] objectForKey:@"device"]
                        forKey:[[notification userInfo] objectForKey:@"uuid"]];
    }
    
    [self updateUI];
}

- (void)updateUI {
    dispatch_async(dispatch_get_main_queue(), ^{
        if (self->_deviceDict && 0 == [self->_deviceDict count]) {
            [self->_tableview setHidden:true];
        }
        else {
            self.scanArray = [NSMutableArray arrayWithArray:self._deviceDict.allValues];
            [self->_tableview setHidden:false];
            [self->_tableview reloadData];
        }
    });
}

- (NSInteger)tableView:(UITableView *)tableView
 numberOfRowsInSection:(NSInteger)section {
    if (   _deviceDict
        && _deviceDict.allKeys.count > 0
        ) {
        return _deviceDict.allKeys.count;
    }
    else {
        return 0;
    }
}
//panmin JUB_GetDeviceInfo
- (void)       tableView:(UITableView *)tableView
 didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    ScanDeviceInfo* deviceInfo = [self.scanArray objectAtIndex:indexPath.row];
    NSLog(@"selected %@", deviceInfo.name);
    
    __deviceID = [_sdk JUB_connectDevice:deviceInfo.uuid
                             connectType:deviceInfo.type
                                 timeout:30000];
    NSLog(@"[%li] JUB_connectDevice(...)", [_sdk JUB_LastError]);
}

- (UITableViewCell *)tableView:(UITableView *)tableView
         cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
                                                   reuseIdentifier:@"cell"];
    ScanDeviceInfo* deviceInfo = [self.scanArray objectAtIndex:indexPath.row];
    cell.textLabel.text = deviceInfo.name;
    return cell;
}


- (IBAction)enumAction:(UIButton *)sender {
    [_sdk JUB_enumDevices];
    NSLog(@"[%li] JUB_enumDevices(void)", (long)[_sdk JUB_LastError]);
}

- (IBAction)stopEnumAction:(UIButton *)sender {
    [_sdk JUB_stopEnumDevices];
    NSLog(@"[%li] JUB_stopEnumDevices(void)", (long)[_sdk JUB_LastError]);
}

- (IBAction)loginAction:(UIButton *)sender {
    ContextConfigETH *cfg = [[ContextConfigETH alloc] init];
    cfg.mainPath = @"m'/44'/60'";
    cfg.chainID = 1;
    __contextID = [_sdk JUB_CreateContextETH:__deviceID
                                         cfg:cfg];
    NSLog(@"[%li] JUB_CreateContextETH(%li)", __deviceID, (long)[_sdk JUB_LastError]);
    [_sdk JUB_ShowVirtualPwd:__contextID];
    NSLog(@"[%li] JUB_ShowVirtualPwd(%li)", __contextID, (long)[_sdk JUB_LastError]);
}

// This method is called once we click inside the textField
-(void)textFieldDidBeginEditing:(UITextField *)textField {
    NSLog(@"Text field did begin editing");
    
}

// This method is called once we complete editing
-(void)textFieldDidEndEditing:(UITextField *)textField {
    NSLog(@"Text field ended editing");
}

// called when 'return' key pressed. return
- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    __pwd = textField.text;
    NSLog(@"Text field should return %@", __pwd);
    NSUInteger retry = [_sdk JUB_VerifyPIN:__contextID
                                    pinMix:__pwd];
    NSLog(@"[%li] JUB_VerifyPIN(%li) retry=%li", __contextID, retry, (long)[_sdk JUB_LastError]);
    return true;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    [self updateUI];
    
    if (nil == _deviceDict) {
        _deviceDict = [NSMutableDictionary dictionary];
    }
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(deviceArrivalObserver:)
                                                 name:NSNotificationName(@"jubitherwallet.device.arrival")
                                               object:[DevicePool shareDevicePool]];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(deviceScanObserver:)
                                                 name:NSNotificationName(@"jubitherwallet.device.scan")
                                               object:nil];
    _sdk = [JubSDKCore alloc];
    
    NSInteger lastError = [_sdk JUB_LastError];
    NSLog(@"[%li] JUB_LastError(void)", (long)lastError);
    
    NSString* version = [_sdk JUB_GetVersion];
    lastError = [_sdk JUB_LastError];
    NSLog(@"[%li] JUB_GetVersion: %@", (long)lastError, version);
}

//    BIP32Path *path32;
//    NSString* strRaw = [sdk JUB_SignTransactionETH:2
//                                              path:path32
//                                             nonce:13
//                                          gasLimit:310000
//                                     gasPriceInWei:@"1"
//                                                to:@"xxxxxxx"
//                                        valueInWei:@""
//                                             input:@""];
@end
