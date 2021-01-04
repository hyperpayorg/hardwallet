//
// Created by zhaojifeng on 2018/4/17.
//

#include "JUB_SDK.h"

#ifdef BLE_MODE

#include <unistd.h>
#include <mutex>

#include "utility/Debug.hpp"

#include "bleTransmit/bleTransmit.h"
#include "device/Fido.h"

#define __PASTE(x, y)                           x##y
#define ADDPRE(name)                           __PASTE(FT_, name)

#define CTOI(x)   (unsigned int)(*(x) * 0x100           \
                  | (*((x) + 1)))

#define CHECK_CMD(x, header)                \
if (x != header) {                          \
    DEBUG_ERR(">>>> CMD HEADER ERROR");     \
    fido.hasError = true;                   \
    fido.stopReceiving();                   \
}                                           \

#define CHECK_SEQ(x, seq)                   \
if (!(  ( x == 0x00                         \
       && x == seq)                         \
    || (( x == (seq + 1)                    \
     && ( x > 0x00                          \
       && x <= 0x7F)))                      \
    )) {                                    \
    DEBUG_ERR(">>>> SEQ ERROR");            \
    fido.hasError = true;                   \
    fido.stopReceiving();                   \
    return 0;                               \
}                                           \

Fido& Fido::instance() {
    static Fido instance;
    return instance;
}

Fido::~Fido() {
    ADDPRE(BLE_Finalize)();
}

unsigned int Fido::finalize() {
    ADDPRE(BLE_Finalize)();
    return 0;
}

unsigned int Fido::clear() {
    mRecvMsg.clear();
    cmdHeader = 0;
    pkgSeq = 0;
    hasError = false;
    return 0;
}

int Fido::RecvCallBack(unsigned long devHandle,
                       unsigned char *data,
                       unsigned int dataLen) {
    DEBUG_LOG("recv data: %s\n", jub::ByteArray2String(data, dataLen).c_str());

    // todo: There is an upper-level data null pointer in ios that causes crashes,
    //       It is necessary to handle null pointer judgments in the communication library.
    if (NULL == data
        || 0 == dataLen
        ) {
        return 0;
    }

    auto& fido = Fido::instance();
    if (fido.hasError) {
        return 0;
    }

    // check package
    switch (data[0]) {
    case CMD_KEEP_ALIVE:  // Filtering keep live
    {
        return 0;
    } // case CMD_KEEP_ALIVE end
    case CMD_ERROR:
    {
        DEBUG_ERR(">>>> BF ERROR");
        // notification have error
        fido.stopReceiving();
        return 0;
    } // case CMD_ERROR end
    case CMD_MSG:
    default:
        break;
    } // switch (data[0]) end

    // Non-first packet, single packet detection
    if (0 < fido.mRecvMsg.size()) {
        CHECK_SEQ(data[0], fido.pkgSeq);
        fido.pkgSeq = data[0];
    }

    // merge package
    fido.mRecvMsg.insert(fido.mRecvMsg.end(), data, data + dataLen);
    auto status = fido.checkResponse();
    switch (status) {
        case ResponseStatus::empty:
        case ResponseStatus::receiving:
            break;
        case ResponseStatus::done:
        case ResponseStatus::invalid:
            // notification have error
            fido.stopReceiving();
            break;
    } // switch (status) end

    return 0;
}

MSGTYPE Fido::response() {
    return mRecvMsg;
}

void Fido::stopReceiving() {
    waiter.notify_one();
}

std::cv_status Fido::waitForReceive(unsigned long timeOut) {
    auto lock = std::unique_lock<std::mutex>(dog);
    return waiter.wait_for(lock, std::chrono::milliseconds(timeOut));
}

Fido::ResponseStatus Fido::checkResponse() {
    if (mRecvMsg.empty()) {
        return Fido::ResponseStatus::empty;
    }

    if (FIDO_HEADER_LEN >= mRecvMsg.size()) {
        return Fido::ResponseStatus::invalid;
    }

    if (mRecvMsg[0] != cmdHeader) {
        DEBUG_ERR(">>>> CMD HEADER ERROR");
        hasError = true;
        return Fido::ResponseStatus::invalid;
    }

    unsigned int recvLen = CTOI(&mRecvMsg[1]);
    int fLen = 0;
    if (recvLen > 17) {
        fLen = (recvLen - 17) / 19;
        int lastLen = (recvLen - 17) % 19;
        if (0 < lastLen) {
            fLen += 1;
        }
    }

    // 数据长度判断，是否接收完成
    if (recvLen + FIDO_HEADER_LEN + fLen <= mRecvMsg.size()) {
        return Fido::ResponseStatus::done;
    }

    return Fido::ResponseStatus::receiving;
}

unsigned int Fido::basicApduAddHeadAndLength(unsigned char paramHeader,
                                             unsigned char *pSendMsg,
                                             unsigned int ulSendMsgLen,
                                             unsigned char *pRecvMsg,
                                             unsigned int *pulRecvLen) {
    unsigned int ret = JUBR_OK;
    if (NULL == pRecvMsg) {
        ret = JUBR_ARGUMENTS_BAD;
        return ret;
    }
    unsigned int index = 0;

    // bMessageType
    pRecvMsg[index++] = paramHeader;
    pRecvMsg[index++] = (unsigned char) ((ulSendMsgLen & 0x0000FF00) >> 8);
    pRecvMsg[index++] = (unsigned char) ((ulSendMsgLen & 0x000000FF) >> 0);

    memcpy(&pRecvMsg[index], pSendMsg, ulSendMsgLen);
    index += ulSendMsgLen;

    *pulRecvLen = index;
    return ret;
}

unsigned int Fido::wrapFidoApdu(unsigned char paramHeader,
                                unsigned char *pSendMsg,
                                unsigned int ulSendMsgLen,
                                unsigned char *pRecvMsg,
                                unsigned int *pulRecvLen) {
    unsigned int ret = JUBR_OK;
    if (NULL == pRecvMsg) {
        ret = JUBR_ARGUMENTS_BAD;
        return ret;
    }

    do {
        // 缓存当前协议头
        cmdHeader = paramHeader;
        ret = basicApduAddHeadAndLength(paramHeader, pSendMsg, ulSendMsgLen, pRecvMsg, pulRecvLen);
        if (0 != ret) {
            break;
        }
        if (FIDO_BLOCK_SIZE >= (*pulRecvLen)) {
            break;
        }

        unsigned int sendMsgIndex = 17;
        MSGTYPE cmd;
        cmd.insert(cmd.end(), pRecvMsg, pRecvMsg + FIDO_BLOCK_SIZE);

        unsigned int fIndex = 0x00;
        while (true) {
            cmd.push_back((unsigned char) fIndex);
            unsigned int capyEnd = sendMsgIndex + FIDO_DATA_BLOCK_SIZE;
            if (sendMsgIndex + FIDO_DATA_BLOCK_SIZE > ulSendMsgLen) {
                capyEnd = ulSendMsgLen;
            }

            cmd.insert(cmd.end(), pSendMsg + sendMsgIndex,
                       pSendMsg + capyEnd);

            sendMsgIndex += FIDO_DATA_BLOCK_SIZE;
            // 当数据正好填满最后一包时，未及时结束，会导致多增加一个字节；补充判断相等的情况
            if (sendMsgIndex >= ulSendMsgLen) {
                break;
            }

            fIndex++;
        }

        *pulRecvLen = (unsigned int)cmd.size();
        memcpy(pRecvMsg, &cmd[0], *pulRecvLen);
    } while (0);

    return ret;
}

unsigned int Fido::parseFidoApdu(unsigned char *pSendMsg, unsigned int ulSendMsgLen,
                                 unsigned char *pRecvMsg, unsigned int *pulRecvLen) {
    DEBUG_LOG("enter parseFidoApdu");
    unsigned int ret = JUBR_OK;
    if (NULL == pRecvMsg) {
        ret = JUBR_ARGUMENTS_BAD;
        return ret;
    }

    do {
        unsigned char tempMsg[512] = {0,};
        unsigned int tempLen = (unsigned int) sizeof(tempMsg)/sizeof(unsigned char);
        ret = removePkgIndex(pSendMsg, ulSendMsgLen, tempMsg, &tempLen);
        if (0 != ret) {
            break;
        }

        ret = removeCmdHeadAndStateCode(tempMsg, tempLen, pRecvMsg, pulRecvLen);
        tempLen = (unsigned int) sizeof(tempMsg)/sizeof(unsigned char);
    } while (0);

    return ret;
}

unsigned int Fido::removePkgIndex(unsigned char *pSendMsg, unsigned int ulSendMsgLen,
                                  unsigned char *pRecvMsg, unsigned int *pulRecvLen) {
    DEBUG_LOG("enter removePkgIndex");
    unsigned int ret = JUBR_OK;
    if (NULL == pRecvMsg) {
        ret = JUBR_ARGUMENTS_BAD;
        return ret;
    }

    do {
        unsigned int recvLen = CTOI(&pSendMsg[1]) + FIDO_HEADER_LEN;
        if (FIDO_BLOCK_SIZE >= recvLen) {
            *pulRecvLen = ulSendMsgLen;
            memcpy(pRecvMsg, pSendMsg, ulSendMsgLen);
            break;
        }

        MSGTYPE temp;
        unsigned int removeIndex = 0;
        temp.insert(temp.end(), pSendMsg, pSendMsg + FIDO_BLOCK_SIZE);
        removeIndex += FIDO_BLOCK_SIZE;

        while (1) {
            removeIndex += 1;
            unsigned int endIndex = removeIndex + FIDO_DATA_BLOCK_SIZE;

            if (removeIndex + FIDO_DATA_BLOCK_SIZE > ulSendMsgLen) {
                endIndex = ulSendMsgLen;
            }
            temp.insert(temp.end(), pSendMsg + removeIndex, pSendMsg + endIndex);

            if (endIndex == ulSendMsgLen) {
                break;
            }

            removeIndex = endIndex;
        }

        if (recvLen != temp.size()) {
            DEBUG_LOG("temp size %d", temp.size());
            ret = JUBR_ARGUMENTS_BAD;
            break;
        }

        *pulRecvLen = (unsigned int)temp.size();
        memcpy(pRecvMsg, &temp[0], *pulRecvLen);
    } while (0);

    return ret;
}

unsigned int Fido::removeCmdHeadAndStateCode(unsigned char *pSendMsg, unsigned int ulSendMsgLen,
                                             unsigned char *pRecvMsg, unsigned int *pulRecvLen) {
    DEBUG_LOG("enter removeCmdHeadAndStateCode");

    unsigned int ret = JUBR_OK;
    if (NULL == pRecvMsg) {
        ret = JUBR_ARGUMENTS_BAD;
        return ret;
    }

    do {
        MSGTYPE temp;
        temp.insert(temp.end(), pSendMsg + FIDO_HEADER_LEN, pSendMsg + ulSendMsgLen);
        *pulRecvLen = (unsigned int)temp.size();
        memcpy(pRecvMsg, &temp[0], *pulRecvLen);
    } while (0);

    return ret;
}
#endif
