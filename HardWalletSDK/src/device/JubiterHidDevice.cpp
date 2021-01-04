#include "JUB_SDK.h"

#ifdef HID_MODE

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "utility/util.h"
#include "device/JubiterHidDevice.hpp"
#include "device/DeviceIOLogHelper.hpp"

namespace jub {

JubiterHidDevice::JubiterHidDevice():
    _pid(PID),
    _vid(VID),
    _handle(NULL),
    _bFirstCmd(true) {
        hid_init();
}

JubiterHidDevice::~JubiterHidDevice() { hid_exit(); }

std::vector<std::string> JubiterHidDevice::EnumDevice() {

	std::vector<std::string> vTokenList;

	auto hidDev = hid_enumerate(VID, PID);
	auto hidDevHead = hidDev;
	while (hidDev) {
		vTokenList.push_back(hidDev->path);
		hidDev = hidDev->next;
	}

	hid_free_enumeration(hidDevHead);

	return vTokenList;
}

JUB_RV JubiterHidDevice::Connect(const std::string path) {

	if (0 != hid_init()) {
		return JUBR_INIT_DEVICE_LIB_ERROR;
	}

	// vid, pid can be parsed from params
	_handle = hid_open_path(path.c_str());
	if (NULL == _handle) {
		return JUBR_CONNECT_DEVICE_ERROR;
	}
	_path = path;

	return JUBR_OK;
}

JUB_RV JubiterHidDevice::Disconnect() {

    _bFirstCmd = false;

    if (NULL != _handle) {
        hid_close(_handle);
        _handle = NULL;

        if (0 != hid_exit()) {
            return JUBR_ERROR;
        }
    }

    return JUBR_OK;
}

JUB_RV JubiterHidDevice::SendData(IN JUB_BYTE_CPTR sendData, IN JUB_ULONG ulSendLen,
                                  OUT JUB_BYTE_PTR retData, INOUT JUB_ULONG_PTR pulRetDataLen,
                                  IN JUB_ULONG ulMiliSecondTimeout) {
    // auto connect
    if (NULL == _handle) {
        JUB_VERIFY_RV(Connect(_path));
    }

#ifdef _DEBUG
	DeviceIOLogHelper handler(sendData, ulSendLen, retData, pulRetDataLen);
#endif

    // fix cid
    JUB_BYTE cid[] = {0x00, 0x00, 0x00, 0x01};
    if (_bFirstCmd) {
        _bFirstCmd = false;

        std::vector<JUB_BYTE> vBufferSend;
        vBufferSend.push_back(0x00);
        vBufferSend.push_back(cid[0]);
        vBufferSend.push_back(cid[1]);
        vBufferSend.push_back(cid[2]);
        vBufferSend.push_back(cid[3]);
        vBufferSend.push_back(0x86);
        vBufferSend.push_back(0x00);
        vBufferSend.push_back(0x00);

        int res = _Write(vBufferSend.data(), vBufferSend.size());
        if (0 > res) {
            Disconnect();
            return JUBR_TRANSMIT_DEVICE_ERROR;
        }

        JUB_BYTE buffer[HID_PACKET_SIZE] = {0,};
        res = _Read(buffer, sizeof(buffer)/sizeof(JUB_BYTE));
        if (0 > res) {
            Disconnect();
            return JUBR_TRANSMIT_DEVICE_ERROR;
        }
    }

    JUB_ULONG offset = 0;
    JUB_ULONG nPackets = 0;
    while (offset < ulSendLen) {
		std::vector<JUB_BYTE> vBufferSend;
		vBufferSend.push_back(0);
		vBufferSend.push_back(cid[0]);
		vBufferSend.push_back(cid[1]);
		vBufferSend.push_back(cid[2]);
		vBufferSend.push_back(cid[3]);

		constexpr JUB_ULONG kFirstPackMaxLen = HID_PACKET_SIZE - HID_PACKET_HEAD_FRIST;
		constexpr JUB_ULONG kNextPackMaxLen  = HID_PACKET_SIZE - HID_PACKET_HEAD_ROUND;
        if (0 == offset) {
			vBufferSend.push_back(0x83);

            // first packet
            vBufferSend.push_back((JUB_BYTE)(ulSendLen & 0xff00));
            vBufferSend.push_back((JUB_BYTE)(ulSendLen & 0x00ff));

            if (kFirstPackMaxLen > ulSendLen) {
                vBufferSend.insert(vBufferSend.end(), sendData,
                                   sendData + ulSendLen);
                offset = ulSendLen;
            }
            else {
                vBufferSend.insert(vBufferSend.end(), sendData,
                                   sendData + (kFirstPackMaxLen));
                offset = kFirstPackMaxLen;
            }
        }
        else {
            vBufferSend.push_back((JUB_BYTE)(nPackets++));

            if (ulSendLen - offset < kNextPackMaxLen) {
                vBufferSend.insert(vBufferSend.end(), sendData + offset,
                                   sendData + ulSendLen);
                offset += ulSendLen - offset;
            }
            else {
                vBufferSend.insert(vBufferSend.end(), sendData + offset,
                                   sendData + offset + kNextPackMaxLen);
                offset += kNextPackMaxLen;
            }
        }

        int res = 0;
        res = _Write(vBufferSend.data(), vBufferSend.size());
        if (0 > res) {
            Disconnect();
            return JUBR_TRANSMIT_DEVICE_ERROR;
        }
    }

    std::vector<JUB_BYTE> vRecv;
    offset = 0;
    JUB_ULONG ulPlayloadSize = 0;
    JUB_BYTE buff[HID_PACKET_SIZE] = {0,};
    int res = 0;
    while (true) {
        do {
            memset(buff, 0, sizeof(buff));
            res = hid_read_timeout(_handle, buff, sizeof(buff), (int)ulMiliSecondTimeout);
            if (res <= 0) {
                Disconnect();
                return JUBR_TRANSMIT_DEVICE_ERROR;
            }
        } while (res > 5 && buff[4] == FIDO2_WAIT_FLAG);//fido2 protocol

        if (0 == offset) {
            // first packet
            ulPlayloadSize = (buff[5] << 8) + buff[6];
            if (*pulRetDataLen < ulPlayloadSize) {
                return JUBR_BUFFER_TOO_SMALL;
            }

            vRecv.resize(ulPlayloadSize, 0);

            if ((HID_PACKET_GNU_SIZE - HID_PACKET_RHEAD_FRIST) > ulPlayloadSize) {
                memcpy(&vRecv[offset], buff + HID_PACKET_RHEAD_FRIST, ulPlayloadSize);
                offset += ulPlayloadSize;
            }
            else {
                memcpy(&vRecv[offset], buff + HID_PACKET_RHEAD_FRIST, HID_PACKET_GNU_SIZE - HID_PACKET_RHEAD_FRIST);
                offset += HID_PACKET_GNU_SIZE - HID_PACKET_RHEAD_FRIST;
            }
        }
        else {
            if ((HID_PACKET_GNU_SIZE - HID_PACKET_RHEAD_ROUND) > (ulPlayloadSize - offset)) {
                memcpy(&vRecv[offset], buff + HID_PACKET_RHEAD_ROUND, ulPlayloadSize - offset);
                offset += ulPlayloadSize - offset;
            }
            else {
                memcpy(&vRecv[offset], buff + HID_PACKET_RHEAD_ROUND, HID_PACKET_GNU_SIZE - HID_PACKET_RHEAD_ROUND);
                offset += HID_PACKET_GNU_SIZE - HID_PACKET_RHEAD_ROUND;
            }
        }

        if (offset >= ulPlayloadSize) {
            *pulRetDataLen = ulPlayloadSize;
            memcpy(retData, vRecv.data(), *pulRetDataLen);

            break;
        }
    }

    return JUBR_OK;
/*
    HID_3KHN_PACK sendHidPacket;
    HID_3KHN_PACK_READ_64 recvHidPacket;

    int txPacketSize = ulSendLen;
    int txOffset = 0, offset = 0, recvLen = *pulRetDataLen;
    int txSize = 0;

    int rxPacketSize = 0;
    int rxOffset = 0;
    int rxSize = 0;
    JUB_ULONG dwResult = 0, dwTimeOutTimes = 0;

    while (ulSendLen) {
        memset(&sendHidPacket, 0, sizeof(sendHidPacket));
        //-------------------------------------------------------------------------------
        //  prepare a data package
        txSize = ulSendLen > 58 ? 58 : ulSendLen;
        if (0 == txSize) {
            break;
        }
        sendHidPacket.uc0PacketSizeH = txPacketSize >> 8;
        sendHidPacket.uc1PacketSizeL = txPacketSize;
        sendHidPacket.uc2BlockOffsetH = txOffset >> 8;
        sendHidPacket.uc3BlockOffsetL = txOffset;
        sendHidPacket.uc4BlockSizeH = txSize >> 8;
        sendHidPacket.uc5BlockSizeL = txSize;
        memmove(sendHidPacket.uc6Buffer, sendData, txSize);
        txOffset += txSize;
        sendData += txSize;
        ulSendLen-= txSize;
        //-------------------------------------------------------------------------------
        //  send this package
        sendHidPacket.id = 0;
        // ESLOG_INF(("send writeFile ulSendLen = %d\n", ulSendLen));

        if (!_Write((JUB_BYTE*)(&sendHidPacket), sizeof(HID_3KHN_PACK))) {
            disconnect();
            return JUBR_TRANSMIT_DEVICE_ERROR;
        } else {
        }
    }

    while (1) {
        memset(&recvHidPacket, 0, sizeof(recvHidPacket));
        int res = 0;
        res = read((JUB_BYTE*)&recvHidPacket, sizeof(HID_3KHN_PACK_READ_64));
        if (0 > res) {
            disconnect();
            return JUBR_TRANSMIT_DEVICE_ERROR;
        }
        else {
            if (   0 == recvHidPacket.uc0PacketSizeH
                && 0 == recvHidPacket.uc1PacketSizeL
                ) {
#if (defined(WIN32) || defined(_WIN32))
                Sleep(100);
#else
                usleep(100 * 1000);
#endif
            }
            else {
                rxPacketSize = recvHidPacket.uc0PacketSizeH * 256 +
                               recvHidPacket.uc1PacketSizeL;
                rxOffset = recvHidPacket.uc2BlockOffsetH * 256 +
                           recvHidPacket.uc3BlockOffsetL;
                rxSize = recvHidPacket.uc4BlockSizeH * 256 +
                         recvHidPacket.uc5BlockSizeL;
                memmove(retData + rxOffset, recvHidPacket.uc6Buffer, rxSize);
                if (rxOffset + rxSize == rxPacketSize) {
                    *pulRetDataLen = rxPacketSize;
                    break;
                }
            }
        }
    }*/
}

int JubiterHidDevice::_Write(const unsigned char* data, size_t length) {
    return hid_write(_handle, data, length);
}

int JubiterHidDevice::_Read(unsigned char* data, size_t length) {
    return hid_read(_handle, data, length);
}

}  // namespace jub end

#endif //HID_MODE
