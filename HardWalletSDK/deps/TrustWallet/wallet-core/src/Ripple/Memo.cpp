// Copyright © 2017-2020 JuBiter.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "BinaryCoding.h"
#include "../BinaryCoding.h"
#include "Memo.h"
#include "Transaction.h"

#include "mSIGNA/stdutils/uchar_vector.h"
#include <stdexcept>

using namespace TW;
using namespace TW::Ripple;

// JuBiter-defined
Memo::Memo(const Data& memoType, const Data& memoData, const Data& memoFormat)
    : memoType(memoType),
      memoData(memoData),
      memoFormat(memoFormat) {
}

// JuBiter-defined
size_t Memo::size() {
    size_t memoTypeLen = 0;
    size_t memoDataLen = 0;
    size_t memoFormatLen = 0;

    Data o;
    if (0 < memoType.size()) {
        o.clear();
        encodeBytes(memoType, o, memoTypeLen);
    }
    if (0 < memoData.size()) {
        o.clear();
        encodeBytes(memoData, o, memoDataLen);
    }
    if (0 < memoFormat.size()) {
        o.clear();
        encodeBytes(memoFormat, o, memoFormatLen);
    }

    return (1
            + (0 == memoTypeLen   ? 0:1) + memoTypeLen   + memoType.size()
            + (0 == memoDataLen   ? 0:1) + memoDataLen   + memoData.size()
            + (0 == memoFormatLen ? 0:1) + memoFormatLen + memoFormat.size()
            + 1);
}

// JuBiter-defined
size_t Memo::sizeType() {
    return memoType.size();
}

// JuBiter-defined
size_t Memo::sizeData() {
    return memoData.size();
}

// JuBiter-defined
size_t Memo::sizeFormat() {
    return memoFormat.size();
}

// JuBiter-defined
void Memo::deserialize(const Data& o) {
    uchar_vector vMemo(o);

    int key = 0;
    /// FieldType::stobject: memo
    uchar_vector vTypeField;
    vTypeField << vMemo.read_uint8();
    FieldType typeField = decodeType(key, vTypeField);
    if (!(        FieldType::stobject  == typeField
          && (int)FieldObjectKey::memo == key)
        ) {
        throw std::invalid_argument("Invalid memo type");
    }
    int offset = (int)(vMemo.get_cur_it() - vMemo.begin());
    vMemo.reset_it();
    vMemo.reset_it(offset);

    // "memo"
    for (size_t i=0; i<3; ++i) {
        uchar_vector rMemo(vMemo.get_cur_it(), vMemo.end());
        vTypeField.clear();
        vTypeField << rMemo.read_uint8();
        typeField = decodeType(key, vTypeField);
        if (!(FieldType::vl == typeField)) {
            throw std::invalid_argument("Invalid memo item");
        }

        uchar_vector rbytes(rMemo.get_cur_it(), rMemo.end());
        Data bytes;
        size_t lengthCnt = 0;
        decodeBytes(rbytes, bytes, lengthCnt);

        offset = (int)(vMemo.get_cur_it() - vMemo.begin());
        offset += 1;
        offset += lengthCnt;

        switch ((FieldMemoKey)key) {
        /// "memoType"
        case FieldMemoKey::type:
            memoType = bytes;
            memoTypeIndex = offset;
            break;
        /// "memoData"
        case FieldMemoKey::data:
            memoData = bytes;
            memoDataIndex = offset;
            break;
        /// "memoFormat"
        case FieldMemoKey::format:
            memoFormat = bytes;
            memoFormatIndex = offset;
            break;
        default:
            throw std::invalid_argument("Invalid memo item");
        }

        offset += bytes.size();
        vMemo.reset_it();
        vMemo.reset_it(offset);

        if (1 == (o.size()-offset)
            ) {
            break;
        }
    }

    /// FieldType::stobject: end
    vTypeField.clear();
    vTypeField << vMemo.read_uint8();
    typeField = decodeType(key, vTypeField);
    if (!(        FieldType::stobject == typeField
          && (int)FieldObjectKey::end == key)
        ) {
        throw std::invalid_argument("Invalid object end");
    }
}

// JuBiter-defined
void Memo::serialize(Data& o) noexcept  {

    size_t offset = 0;
    size_t varLen = 0;

    if (0 == memoData.size()) {
        return;
    }

    encodeType(FieldType::stobject, (int)FieldObjectKey::memo, o);

    if (0 < memoType.size()) {
        encodeType(FieldType::vl, (int)FieldMemoKey::type, o);
        offset = o.size();
        encodeBytes(memoType, o, varLen);
        memoTypeIndex = offset + varLen;
    }

    if (0 < memoData.size()) {
        encodeType(FieldType::vl, (int)FieldMemoKey::data, o);
        offset = o.size();
        encodeBytes(memoData, o, varLen);
        memoDataIndex = offset + varLen;
    }

    if (0 < memoFormat.size()) {
        encodeType(FieldType::vl, (int)FieldMemoKey::format, o);
        offset = o.size();
        encodeBytes(memoFormat, o, varLen);
        memoFormatIndex = offset + varLen;
    }

    encodeType(FieldType::stobject, (int)FieldObjectKey::end, o);
}
