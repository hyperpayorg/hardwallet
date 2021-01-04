//
//  BLETemplate.h
//  FTTransmission
//
//  Created by FTsafe ID Department on 2017/4/11.
//  Copyright © 2017年 FT. All rights reserved.
//

#ifndef BLETemplate_h
#define BLETemplate_h

#if   defined(JSSAFE)
#define BLE_AUX_ScanFuncCallBack     JS_BLE_AUX_ScanFuncCallBack
#define BLE_AUX_ConnFuncCallBack     JS_BLE_AUX_ConnFuncCallBack
#define BLE_AUX_DisconnFuncCallBack  JS_BLE_AUX_DisconnFuncCallBack
#define BLE_AUX_RcvDataFuncCallBack  JS_BLE_AUX_RcvDataFuncCallBack
#define BLE_AUX_INIT_PARAM           JS_BLE_AUX_INIT_PARAM
#define BLE_AUX_DidBondFuncCallBack  JS_BLE_AUX_DidBondFuncCallBack
#else
#define BLE_AUX_ScanFuncCallBack     FT_BLE_AUX_ScanFuncCallBack
#define BLE_AUX_ConnFuncCallBack     FT_BLE_AUX_ConnFuncCallBack
#define BLE_AUX_DisconnFuncCallBack  FT_BLE_AUX_DisconnFuncCallBack
#define BLE_AUX_RcvDataFuncCallBack  FT_BLE_AUX_RcvDataFuncCallBack
#define BLE_AUX_INIT_PARAM           FT_BLE_AUX_INIT_PARAM
#define BLE_AUX_DidBondFuncCallBack  FT_BLE_AUX_DidBondFuncCallBack
#endif

typedef void (*BLE_AUX_ScanFuncCallBack)
(
const char* devname, /* device name */
const char* uuid,    /* IOS:     device uuid;
                */
unsigned int    devType  /* device type */
);

typedef void  (*BLE_AUX_ConnFuncCallBack)
(
const char* uuid,     /* IOS:     device uuid;
                */
const unsigned long handle //设备句柄
);

typedef void  (*BLE_AUX_DisconnFuncCallBack)
(
const char* uuid     /* IOS:     device uuid;
                          */
);

typedef void  (*BLE_AUX_RcvDataFuncCallBack)
(
const unsigned long handle, //device handle
unsigned char * data,   /*  rcv data */
unsigned int    len     /*  data len */
);

typedef struct {
    BLE_AUX_ConnFuncCallBack     connFuncCallBack;        /* conn callback */
    BLE_AUX_RcvDataFuncCallBack  rcvDataFuncCallBack;     /* rcv data callback */
    BLE_AUX_ScanFuncCallBack     scanFuncCallBack;        /* scan callback */
    BLE_AUX_DisconnFuncCallBack  disconnFuncCallBack;     /* disconn callback */
}__attribute__ ((packed)) BLE_AUX_INIT_PARAM;

typedef void  (*BLE_AUX_DidBondFuncCallBack)
(
const unsigned long handle //device handle

);

#endif /* BLETemplate_h */
