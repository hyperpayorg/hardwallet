// Copyright © 2017-2020 JuBiter.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../Data.h"

using Data = TW::Data;

// JuBiter-defined
enum class FieldObjectKey: int {
    memo = 0xa,
    end  = 0x1
};

// JuBiter-defined
enum class FieldMemoKey: int {
    type   = 0xc,
    data   = 0xd,
    format = 0xe
};

namespace TW::Ripple {

// JuBiter-defined
class Memo {
public:
    Data memoType;
    Data memoData;
    Data memoFormat;

    // JuBiter-defined
    Memo() { }
    Memo(const Data& memoType, const Data& memoData, const Data& memoFormat);

    size_t size();
    size_t sizeType();
    size_t sizeData();
    size_t sizeFormat();

    void deserialize(const Data& o);
    void serialize(Data& o) noexcept;

    bool isEmpty() {
        return (   0 == sizeType()
                || 0 == sizeData());
    }

    virtual uint16_t getMemoTypeIndex() const {
        return memoTypeIndex;
    }
    virtual uint16_t getMemoDataIndex() const {
        return memoDataIndex;
    }
    virtual uint16_t getMemoFormatIndex() const {
        return memoFormatIndex;
    }

private:
    uint16_t memoTypeIndex = 0;
    uint16_t memoDataIndex = 0;
    uint16_t memoFormatIndex = 0;
}; // class Memo end

} // namespace TW::Ripple end
