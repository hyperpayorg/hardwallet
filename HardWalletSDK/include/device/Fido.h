//
// Created by zhaojifeng on 2018/4/17.
//

#ifndef JUBITER_FIDO_FIDO_H
#define JUBITER_FIDO_FIDO_H

#include "JUB_SDK.h"

#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>

// 协议头
#define CMD_PING        0x81  // ping test cmd header
#define CMD_KEEP_ALIVE  0x82  // device keep alive cmd header
#define CMD_MSG         0x83  // messge cmd header
#define CMD_ERROR       0xBF  // error response cmd header

#define ERR_INVALID_CMD   0x01; //The command in the request is unknown/invalid
#define ERR_INVALID_PAR   0x02; //The parameter(s) of the command is/are invalid or missing
#define ERR_INVALID_LEN   0x03; //The length of the request is invalid
#define ERR_INVALID_SEQ   0x04; //The sequence number is invalid
#define ERR_REQ_TIMEOUT   0x05; //The request timed out
#define ERR_OTHER         0x7f; //Other, unspecified error

// 协议——协议头长度
#define FIDO_HEADER_LEN                 3
#define FIDO_DATA_BLOCK_SIZE            19
#define FIDO_BLOCK_SIZE                 20

typedef std::vector<unsigned char> MSGTYPE;

class Fido {

public:
    static Fido& instance();

private:
    ~Fido();
    Fido() = default;
    Fido(const Fido& ) = delete;
    Fido& operator=(const Fido) = delete;

public:
    unsigned int finalize();
    unsigned int clear();
    static int RecvCallBack (unsigned long  devHandle,
                             unsigned char* data,
                             unsigned int   dataLen);
    MSGTYPE response();

    unsigned int basicApduAddHeadAndLength(unsigned char paramHeader,
                                           unsigned char* pSendMsg,
                                           unsigned int   ulSendMsgLen,
                                           unsigned char* pRecvMsg,
                                           unsigned int*  pulRecvLen);

    unsigned int wrapFidoApdu(unsigned char paramHeader,
                              unsigned char* pSendMsg,
                              unsigned int   ulSendMsgLen,
                              unsigned char* pRecvMsg,
                              unsigned int*  pulRecvLen);

    unsigned int parseFidoApdu(unsigned char *pSendMsg,
                               unsigned int ulSendMsgLen,
                               unsigned char *pRecvMsg,
                               unsigned int *pulRecvLen);

    unsigned int removePkgIndex(unsigned char *pSendMsg,
                                unsigned int ulSendMsgLen,
                                unsigned char *pRecvMsg,
                                unsigned int *pulRecvLen);

    unsigned int removeCmdHeadAndStateCode(unsigned char *pSendMsg,
                                           unsigned int ulSendMsgLen,
                                           unsigned char *pRecvMsg,
                                           unsigned int *pulRecvLen);

    void stopReceiving();

    std::cv_status waitForReceive(unsigned long timeOut);

    enum class ResponseStatus {
        empty,          // have not receive
        receiving,      // not complete
        done,           // complete
        invalid,        // receive, but not valid
    }; // enum class ResponseStatus end

    ResponseStatus checkResponse();

private:
    std::mutex dog;
    std::condition_variable waiter;

    MSGTYPE mRecvMsg;       /**< 接收数据缓冲区 */
    unsigned char cmdHeader;
    unsigned char pkgSeq = 0;
    bool hasError = false;
}; // class Fido end

Fido* GetFido();

#endif //JUBITER_FIDO_FIDO_H
