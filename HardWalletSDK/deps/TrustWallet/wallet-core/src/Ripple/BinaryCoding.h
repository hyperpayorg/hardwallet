// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace TW::Ripple {

enum class FieldType;

/// Encodes a field type.
inline void encodeType(FieldType type, int key, std::vector<uint8_t>& data) {
    const auto typeValue = static_cast<int>(type);
    if (key <= 0xf) {
        data.push_back(static_cast<uint8_t>((typeValue << 4) | key));
    } else {
        data.push_back(static_cast<uint8_t>(typeValue << 4));
        data.push_back(static_cast<uint8_t>(key));
    }
}

// JuBiter-defined
inline FieldType decodeType(int& key, std::vector<uint8_t>& data) {
    FieldType type;
    if (data.size() <=1) {
        type = static_cast<FieldType>((data[0] & 0xF0) >> 4);
        key  = static_cast<int>(data[0] & 0x0F);
    }
    else {
        type = static_cast<FieldType>(data[0] >> 4);
        key  = static_cast<uint8_t>(data[1]);
    }

    return type;
}

/// Encodes a variable length.
inline void encodeVariableLength(size_t length, std::vector<uint8_t>& data) {
    if (length <= 192) {
        data.push_back(static_cast<unsigned char>(length));
    } else if (length <= 12480) {
        length -= 193;
        data.push_back(static_cast<unsigned char>(length >> 8));
        data.push_back(static_cast<unsigned char>(length & 0xff));
    } else if (length <= 918744) {
        length -= 12481;
        data.push_back(static_cast<unsigned char>(length >> 16));
        data.push_back(static_cast<unsigned char>((length >> 8) & 0xff));
        data.push_back(static_cast<unsigned char>(length & 0xff));
    }
}

// JuBiter-defined
inline size_t decodeVariableLength(const std::vector<uint8_t>& data, size_t& lengthCnt) {

    size_t varLen = static_cast<size_t>(data[0]);
    if (varLen <= 192) {
        lengthCnt = 1;
        return varLen;
    }

    // Decode has a problem with length expressed in two or three bytes.
//    varLen = static_cast<size_t>((data[0] << 8)
//                               + (data[1] +  193));
//    if (   varLen >    192
//        && varLen <= 12480
//        ) {
//        lengthCnt = 2;
//        return varLen;
//    }
//    varLen = static_cast<size_t>((data[0] << 16)
//                               + (data[1] << 8)
//                               + (data[2] +  12481));
//    if (   varLen >   12480
//        && varLen <= 918744
//        ) {
//        lengthCnt = 3;
//        return varLen;
//    }

    return 0;
}

/// Encodes a variable length bytes.
inline void encodeBytes(std::vector<uint8_t> bytes, std::vector<uint8_t>& data, size_t& varLen) {
    size_t dataLen = data.size();
    encodeVariableLength(bytes.size(), data);
    size_t length = data.size();
    varLen = length - dataLen;
    data.insert(data.end(), bytes.begin(), bytes.end());
}

// JuBiter-defined
inline size_t decodeBytes(std::vector<uint8_t> data, std::vector<uint8_t>& bytes, size_t& lengthCnt) {
    lengthCnt = 0;
    size_t length = decodeVariableLength(data, lengthCnt);
    bytes.insert(bytes.end(), data.begin()+lengthCnt, data.begin()+lengthCnt+length);
    return length;
}

} // namespace TW::Ripple
