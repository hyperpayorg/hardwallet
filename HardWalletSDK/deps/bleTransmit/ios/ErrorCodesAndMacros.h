//
//  ErrorCodesAndMacro.h
//  FTTransmission
//
//  Created by FTsafe ID Department on 2017/4/14.
//  Copyright © 2017年 FT. All rights reserved.
//

#ifndef ErrorCodesAndMacro_h
#define ErrorCodesAndMacro_h

// 返回码 采用PKCS#11返回值
#define CKR_OK                                  0x00000000  // 正确
#define CKR_ARGUMENTS_BAD                       0x00000007  // 参数错误
#define CKR_DEVICE_ERROR                        0x00000030  // 设备异常
#define CKR_DEVICE_MEMORY                       0x00000031  // 设备空间不足
#define CKR_USER_TYPE_INVALID                   0x00000103
#define CKR_BUFFER_TOO_SMALL                    0x00000150  // 参数空间太小
#define CKR_CRYPTOKI_NOT_INITIALIZED            0x00000190  // SDK尚未初始化
#define CKR_CRYPTOKI_ALREADY_INITIALIZED        0x00000191  // SDK已经初始化

#define CKR_VENDOR_DEFINED                      0x80000000
#define CKR_DEVICE_IS_BUSY                      (CKR_VENDOR_DEFINED + 1)
#define CKR_COMMLIB_NOT_INITIALIZED             (CKR_VENDOR_DEFINED + 2)
#define CKR_COMMLIB_ALREADY_INITIALIZED         (CKR_VENDOR_DEFINED + 3)

#define CKR_TIMEOUT                             (CKR_VENDOR_DEFINED + 8)
#define CKR_USER_CANCEL                         (CKR_VENDOR_DEFINED + 9)
#define CKR_USER_END                            (CKR_VENDOR_DEFINED + 10)
#define CKR_NO_DEVICE                           (CKR_VENDOR_DEFINED + 11)

//蓝牙通讯库定义错误码
#define CKR_VENDOR_BLE_DEFINED                  0x81000000
#define CKR_BLE_POWEROFF                        (CKR_VENDOR_BLE_DEFINED + 1)
#define CKR_BLE_NOT_SUPPORT                     (CKR_VENDOR_BLE_DEFINED + 2)
#define CKR_BLE_CONNECT_FAIL                    (CKR_VENDOR_BLE_DEFINED + 3)
#define CKR_BLE_BOND_FAIL                       (CKR_VENDOR_BLE_DEFINED + 4)
#define CKR_BLE_NOT_AUTHORIZE                   (CKR_VENDOR_BLE_DEFINED + 5)
#define CKR_BLE_UNKNOW                          (CKR_VENDOR_BLE_DEFINED + 6)

//蓝牙设备连接状态宏定义
#define BLES_UNCONNECT                1     //未连接
#define BLES_ISCONNECTING             2     //正在连接
#define BLES_CONNECTED_DISVERING_CHAR 3     //已连接正在扫描特征
#define BLES_CONNECTED_OPENING_NOTIFY 4     //已连接正在打开notify
#define BLES_CONNECTED_CAN_SEND       5     //已连接可通信
#define BLES_ISDISCONNECTING          6

//蓝牙相关设置的宏定义
#define MAX_CONNECTED_DEVICE_COUNT    7
#define KEY_VERSION_INDEX_IN_ADV      22

#define INITIALIZE_TIMEOUT   2000

//设备类型
#define BLE_GENERAL_DEVICE            0
#define BLE_KEY_DEVICE                1
//#define BLE_EPAYPOS_DEVICE            "paypos"   目前无特殊需求  扫描不需要过滤


//蓝牙设备服务和特征UUID
#define UUIDSTR_DEVICE_INFO_SERVICE             @"180A"
#define UUIDSTR_MANUFACTURE_NAME_CHAR           @"2A29"
#define UUIDSTR_MODEL_NUMBER_CHAR               @"2A24"
#define UUIDSTR_SERIAL_NUMBER_CHAR              @"2A25"
#define UUIDSTR_HARDWARE_REVISION_CHAR          @"2A27"
#define UUIDSTR_FIRMWARE_REVISION_CHAR          @"2A26"
#define UUIDSTR_SOFTWARE_REVISION_CHAR          @"2A28"
#define UUIDSTR_SYSTEM_ID_CHAR                  @"2A23"
#define UUIDSTR_IEEE_11073_20601_CHAR           @"2A2A"

#define UUIDSTR_ISSC_PROPRIETARY_SERVICE        @"49535343-FE7D-4AE5-8FA9-9FAFD205E455"
#define UUIDSTR_CONNECTION_PARAMETER_CHAR       @"49535343-6DAA-4D02-ABF6-19569ACA69FE"
#define UUIDSTR_AIR_PATCH_CHAR                  @"49535343-ACA3-481C-91EC-D85E28A60318"

#define UUIDSTR_ISSC_TRANS_TX                   @"49535343-1E4D-4BD9-BA61-23C647249616"
#define UUIDSTR_ISSC_TRANS_RX                   @"49535343-8841-43F4-A8D4-ECBE34729BB3"
#define UUIDSTR_ISSC_MP                         @"49535343-ACA3-481C-91EC-D85E28A60318"


#define UUIDSTR_C3_PROPRIETARY_SERVICE          @"46540001-0001-00C3-0000-465453414645"
#define UUIDSTR_C3_TRANS_WX                     @"46540002-0001-00C3-0000-465453414645"
#define UUIDSTR_C3_TRANS_NX                     @"46540003-0001-00C3-0000-465453414645"

#define UUIDSTR_E1_PROPRIETARY_SERVICE          @"46540001-0001-00E1-0000-465453414645"
#define UUIDSTR_E1_TRANS_WX                     @"46540002-0001-00E1-0000-465453414645"
#define UUIDSTR_E1_TRANS_NX                     @"46540003-0001-00E1-0000-465453414645"

#define UUIDSTR_C4_PROPRIETARY_SERVICE          @"46540001-0001-00C4-0000-465453414645"
#define UUIDSTR_C4_TRANS_WX                     @"46540002-0001-00C4-0000-465453414645"
#define UUIDSTR_C4_TRANS_NX                     @"46540003-0001-00C4-0000-465453414645"

#define UUIDSTR_SMART_C4_PROPRIETARY_SERVICE    @"46540001-0004-00C4-0000-465453414645"
#define UUIDSTR_SMART_C4_TRANS_WX               @"46540002-0004-00C4-0000-465453414645"
#define UUIDSTR_SMART_C4_TRANS_NX               @"46540003-0004-00C4-0000-465453414645"

#define UUIDSTR_NORDIC_PROPRIETARY_SERVICE      @"46540001-0006-00C4-0000-465453414645"
#define UUIDSTR_NORDIC_TRANS_WX                 @"46540002-0006-00C4-0000-465453414645"
#define UUIDSTR_NORDIC_TRANS_NX                 @"46540003-0006-00C4-0000-465453414645"

#define UUIDSTR_Z32HUB_PROPRIETARY_SERVICE      {0xFF, 0x00}
#define UUIDSTR_Z32HUB_TRANS_WX                 {0xFF, 0x01}
#define UUIDSTR_Z32HUB_TRANS_NX                 {0xFF, 0x02}

//通讯库对外设备类型
#define DEV_TYPE_BLE                1   /**< 蓝牙设备 */

//通讯库使用通讯相关宏定义值
#define DEV_KEY_MAX_PACKAGE_LEN     20 //KEY一包最多发20字节
#define DEV_KEY_EACH_PACKAGE_WAIT_TIME 0.006 //KEY每包之间的间隔时间

#define DEV_GENERAL_MAX_PACKAGE_LEN 20
#define DEV_GENERAL_EACH_PACKAGE_WAIT_TIME 0.006

#endif /* ErrorCodesAndMacro_h */
