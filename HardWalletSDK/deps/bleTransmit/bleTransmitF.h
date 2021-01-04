//
//  bleTransmitF.h
//  bleTransmit
//
//  Copyright © 1998-2016, FEITIAN Technologies Co., Ltd. All rights reserved.
//

#ifndef __bleTransmitF_h__
#define __bleTransmitF_h__

CK_FUNCTION_INFO(BLE_Initialize)
#ifdef CK_NEED_ARG_LIST
(BLE_INIT_PARAM param);
#endif


CK_FUNCTION_INFO(BLE_Finalize)
#ifdef CK_NEED_ARG_LIST
(void);
#endif

CK_FUNCTION_INFO(BLE_Scan)
#ifdef CK_NEED_ARG_LIST
(void);
#endif

CK_FUNCTION_INFO(BLE_StopScan)
#ifdef CK_NEED_ARG_LIST
(void);
#endif


CK_FUNCTION_INFO(BLE_ConnDev)
#ifdef CK_NEED_ARG_LIST
(
 unsigned char* bBLEUUID,      /**< ble device UUID */
 unsigned int   connectType,   /**< ble device connect type */
 unsigned long* pDevHandle,    /**< output ble device connect handle */
 unsigned int   timeout = 3000 /**< 连接操时时间 */
 );
#endif

CK_FUNCTION_INFO(BLE_CancelConnDev)
#ifdef CK_NEED_ARG_LIST
(
 unsigned char* bBLEUUID      /**< ble device UUID */
 );
#endif

CK_FUNCTION_INFO(BLE_DisConn)
#ifdef CK_NEED_ARG_LIST
(
 unsigned long devHandle       /**< output ble device connect handle */
 );
#endif

/*   IsConected？
 return value : 0 -- disConnected   ;
                 other value --  Connected*/
CK_FUNCTION_INFO(BLE_IsConn)
#ifdef CK_NEED_ARG_LIST
(
 unsigned long devHandle       /**< output ble device connect handle */
 );
#endif

/*   SetIsReConected？
 return value : 0 -- disConnected   ;
 other value --  Connected*/
CK_FUNCTION_INFO(BLE__SetIsReConnected)
#ifdef CK_NEED_ARG_LIST
(
 bool on       /**<reconnect or not  */
 );
#endif

CK_FUNCTION_INFO(BLE_SendAPDU)
#ifdef CK_NEED_ARG_LIST
(
 unsigned long  devHandle,
 unsigned char* apdu,
 unsigned int   apduLen
 );
#endif

CK_FUNCTION_INFO(BLE_SendAPDU_Sync)
#ifdef CK_NEED_ARG_LIST
(
	unsigned long  devHandle,
	unsigned char* apdu,
	unsigned int   apduLen,
	unsigned char* recvData, 
	unsigned int* recvDataLen,
	unsigned int timeout = 3000
);
#endif

#endif /* __bleTransmitF_h__ */

