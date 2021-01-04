//
//  BLEInterface.h
//  FTTransmission
//
//  Created by FTsafe ID Department on 2017/5/4.
//  Copyright © 2017年 FT. All rights reserved.
//

#ifndef BLEInterface_h
#define BLEInterface_h

#include "BLETemplate.h"

#if   defined(JSSAFE)
#define __PASTE(x,y)                           x##y
#define ADDPRE(name)                           __PASTE(JS_, name)
#else
#define __PASTE(x,y)                           x##y
#define ADDPRE(name)                           __PASTE(FT_, name)
#endif

#ifdef __cplusplus
extern "C"
{
#endif



unsigned int  ADDPRE(BLESetLibConfig(char * companyFirstTag[] ,char * companySecondTag[],unsigned char*supportProductType));
    
unsigned int  ADDPRE(BLEInitialize(BLE_AUX_INIT_PARAM* initPara));
//模块使用结束函数
unsigned int  ADDPRE(BLEFinalizedBLE());
//扫描函数
unsigned int  ADDPRE(BLEScan());
//停止扫描函数
unsigned int  ADDPRE(BLEStopScan());
//连接函数   超时时间单位为ms

unsigned int  ADDPRE(BLEConnectDev(const char* pcuuid, unsigned long* pulHandle, unsigned long ulTimeout));
//取消连接
unsigned int  ADDPRE(BLECancelConnect(const char* pcuuid));
//断开连接函数
unsigned int  ADDPRE(BLEDisconnect(unsigned long ulHandle));
//发送数据
unsigned int  ADDPRE(BLESendData(unsigned long ulHandle,const unsigned char* bData, unsigned int dlen));
//查询连接状态
bool          ADDPRE(BLEIsConnected(unsigned long ulHandle));
//设置是否重连
void          ADDPRE(BLESetIsReConnected(bool on));
//获取库版本
unsigned int  ADDPRE(BLEGetLibVersion(char * libVersion));
    
#ifdef __cplusplus
}
#endif
#endif /* BLEInterface_h */
