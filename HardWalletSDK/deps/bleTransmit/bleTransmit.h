//
//  bleTransmit.h
//  bleTransmit
//
//  Copyright © 1998-2016, FEITIAN Technologies Co., Ltd. All rights reserved.
//

#ifndef __bleTransmit_h__
#define __bleTransmit_h__

#define DISCALL                     0xFFFFFFFF

#define	IFD_SUCCESS                 0   /**< no error */
#define IFD_TIMEOUT                 1   /**< conn time out */
#define IFD_COMMUNICATION_ERROR		612 /**< generic error */
#define IFD_RESPONSE_TIMEOUT		613 /**< timeout */
#define IFD_NOT_SUPPORTED           614 /**< request is not supported */
#define IFD_NO_DEVICE               615 /**< no device>*/

typedef int (*BLE_ReadCallBack)(
                                unsigned long  devHandle,
                                unsigned char* data,
                                unsigned int   dataLen
                                );
typedef void (*BLE_ScanCallBack)(
                                 unsigned char* devName,
                                 unsigned char* uuid,
                                 unsigned int   type
                                 );
//typedef void (*BLE_StopScanCallback) ();
typedef void (*BLE_DiscCallBack)(unsigned char* uuid);

typedef struct
{
    void*            param;
    BLE_ReadCallBack callBack;
    BLE_ScanCallBack scanCallBack;
//    BLE_StopScanCallback stopScanCallback;
    BLE_DiscCallBack discCallBack;
} BLE_INIT_PARAM;


// define __PASTE
#define __PASTE(x,y)                           x##y
// define CK_DECLARE_FUNCTION
#define CK_DECLARE_FUNCTION(returnType, name)  returnType name

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
    // define CK_FUNCTION_INFO
#ifdef  JSSAFE
#define CK_FUNCTION_INFO(name)                 CK_DECLARE_FUNCTION(unsigned int, __PASTE(JS_, name))
#else
#define CK_FUNCTION_INFO(name)                 CK_DECLARE_FUNCTION(unsigned int, __PASTE(FT_, name))
#endif
    
    // define CK_NEED_ARG_LIST
#define CK_NEED_ARG_LIST 1
    
#include "bleTransmitF.h"
    
    // undef CK_NEED_ARG_LIST
#undef CK_NEED_ARG_LIST
    // undef CK_FUNCTION_INFO
#undef CK_FUNCTION_INFO
    
#ifdef __cplusplus
}
#endif /* __cplusplus */

// undef CK_DECLARE_FUNCTION
#undef CK_DECLARE_FUNCTION
// undef __PASTE
#undef __PASTE

#endif /* #ifndef __bleTransmit_h__ */
