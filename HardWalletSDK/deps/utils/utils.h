//
// Created by FT on 2018/4/26.
//

#ifndef JUBITER_UTILS_H
#define JUBITER_UTILS_H


#include <jni.h>
#include <JUB_SDK.h>


#ifndef CHECK_NULL
#define CHECK_NULL(OBJ, ERR) if (OBJ == NULL) {return ERR;}
#endif





void setScanCallbackObj(JUB_VOID_PTR javaVM, jobject callback);

void setDiscCallbackObj(JUB_VOID_PTR javaVM, jobject callback);

void callback(unsigned int type);

void scanCallback(JUB_BYTE_PTR devname, JUB_BYTE_PTR uuid, JUB_ULONG devType);

void disconnectCallback(JUB_BYTE_PTR devname);

int getMnemonicStrength(jint strength);

JUB_RV jobjectToInitParam(JNIEnv *env, JUB_VOID_PTR javaVM, DEVICE_INIT_PARAM *param);


#endif //JUBITER_UTILS_H
