//
//  Deserialization.h
//  JubSDK
//
//  Created by panmin on 2019/9/26.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#pragma once

#include <nlohmann/json.hpp>

#include "../Data.h"
#include "../BinaryCoding.h"

#include <vector>
#include <set>
#include <string>
#include "mSIGNA/stdutils/uchar_vector.h"

namespace TW::Bravo {
inline void decodeVarInt64(const Data& os, uint64_t& x, int& varIntByteSize) {
    x = 0;
    varIntByteSize = 0;

    uint64_t c = 0;
    int shift = 0, n = 0;
    for (shift = 0; shift < 64; shift += 7) {
        varIntByteSize += 1;
        c = (uint64_t) os[n++];
        x |= (c & 0x7F) << shift;
        if ((c & 0x80) == 0) {
            break;
        }
    }
}

inline void decodeVarInt32(const Data& os, uint32_t& x, int& varIntByteSize) {
    decodeVarInt64(os, (uint64_t&)x, varIntByteSize);
}

inline void decodeString(const Data& os, std::string& s, int& varIntByteSize) {
    varIntByteSize = 0;
    uint64_t size = 0;
    decodeVarInt64(os, size, varIntByteSize);
    uchar_vector vOs(os.begin() + varIntByteSize,
                     os.begin() + varIntByteSize + size);
    s = vOs.getCharsAsString();
}

inline void decodeCollection(const Data& os, uint64_t& collectionCnt, int& varIntByteSize) {
    varIntByteSize = 0;
    collectionCnt = 0;
    decodeVarInt64(os, collectionCnt, varIntByteSize);
}

using json = nlohmann::json;

template<typename Collection>
inline void decodeCollection(const json& array, Collection& collection) {
//    json array = json::array();
//    for (const auto& item : collection) {
//        array.push_back(item.serialize());
//    }
//
//    return array;
}

template<typename Collection>
inline void decodePointerCollection(const json& array, Collection& collection) {
//    json array = json::array();
//    for (const auto& item : collection) {
//        array.push_back(item->serialize());
//    }
//
//    return array;
}
} // namespace TW::Bravo end
