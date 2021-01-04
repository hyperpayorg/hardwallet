#ifndef _BTMANAGER_H_
#define _BTMANAGER_H_

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <jni.h>

#define BT_SUCCESS           0x0000000
#define BT_ERR_INIT          0x8000001
#define BT_ERR_PARA          0x8000002
#define BT_ERR_ENV           0x8000003
#define BT_ERR_CONTEXT       0x8000004

#define BT_ERR_NOT_ENABLE              0x8000005
#define BT_ERR_NOT_SUPPORT             0x8000006
#define BT_ERR_NO_LOCATION_PERMISSION  0x8000007
#define BT_ERR_CONNECT_FAIL            0x8000008
#define BT_ERR_TRANSMIT_ERROR          0x8000009
#define BT_ERR_BOND_FAIL               0x8000010



#ifdef  JSSAFE
#define BTManager    JS_BTManager
#else
#define BTManager    FT_BTManager
#endif // defined(JSSAFE)

typedef unsigned int         ULONG;
typedef unsigned long        LONG;
typedef unsigned char        BYTE;
typedef bool                 BOOL;

using namespace std;

typedef int (*FT_BT_ReadCallBack)(
                                unsigned long  devHandle,
                                unsigned char* data,
                                unsigned int   dataLen
                                );
typedef void (*FT_BT_ScanCallBack)(
                                 unsigned char* devName,
                                 unsigned char* uuid,
                                 unsigned int   type
                                 );
typedef void (*FT_BT_ConnCallBack)(unsigned char* uuid);

typedef struct
{
    void*                param;
    FT_BT_ReadCallBack   callBack;
    FT_BT_ScanCallBack   scanCallBack;
    FT_BT_ConnCallBack   discCallBack;
} BT_INIT_PARAM;

class BTManager
{
private:
	BTManager();
	~BTManager();

public:
	static BTManager *GetInstance()
	{
		static BTManager *mInstance = NULL;

        if (mInstance == NULL) {
        	mInstance = new BTManager();
        }
        return mInstance;
	}

public:
	ULONG  initialize(BT_INIT_PARAM param);
	ULONG  finalize();
    ULONG  btGetLibVer(BYTE* version, ULONG* verLen);

	ULONG  btStartScan(ULONG devType = 0x04,
                       ULONG scanTimeout = 60);  //调用时暂使用默认参数
	ULONG  btStopScan();
    ULONG  btConnect(BYTE* mac, LONG* handler, ULONG scanTimeout);
    ULONG  btConnectDirectly(ULONG devType, BYTE* serialNum, ULONG snLen, ULONG scanTimeout);
    ULONG  btDisconnect(LONG handler);
    ULONG  btCancelConnect(BYTE* mac);
    ULONG  btAsyncTransmit(LONG handler, BYTE* sendData, ULONG sendDataLen);
    ULONG  btSyncTransmit(LONG handler,
						  BYTE* sendData, ULONG sendDataLen,
						  BYTE* recvData, ULONG* recvDataLen,
						  ULONG timeout);
    ULONG  btCheckBtEnv();
	ULONG  btIsConnected(LONG handler);


private:
	jobject getGlobalContext(JNIEnv *env);

private:
	JavaVM *m_Jvm = NULL;

	jobject m_BluetoothKey;

	jmethodID jm_Constructor;

	jmethodID jm_Initialize;
	jmethodID jm_Finalize;
	jmethodID jm_CheckBtEnv;
	jmethodID jm_IsConnected;
	jmethodID jm_GetLibVer;

	jmethodID jm_StartScan;
	jmethodID jm_StopScan;
    jmethodID jm_Connect;
    jmethodID jm_ConnectDirectly;
    jmethodID jm_CancelConnect;
	jmethodID jm_Disconnect;
    jmethodID jm_AsyncTransmit;
    jmethodID jm_SyncTransmit;
};

#endif
