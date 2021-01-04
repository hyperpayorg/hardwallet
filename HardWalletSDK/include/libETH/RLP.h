// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "airbitz-core/abcd/util/Data.hpp"

namespace jub {
namespace eth {
    using namespace abcd;
/// Implementation of Ethereum's RLP encoding.
///
/// - SeeAlso: https://github.com/ethereum/wiki/wiki/RLP
struct RLP {
    /// Encodes a string;
    static DataChunk encode(const std::string& string) noexcept {
        return encode(DataChunk(string.begin(), string.end()));
    }

    /// Wraps encoded data as a list.
    static DataChunk encodeList(const DataChunk& encoded) noexcept;

    /// Encodes a block of data.
    static DataChunk encode(const DataChunk& data) noexcept;

    /// Encodes a static array.
    template <std::size_t N>
    static DataChunk encode(const std::array<uint8_t, N>& data) noexcept {
        if (N == 1 && data[0] <= 0x7f) {
            // Fits in single byte, no header
            return DataChunk(data.begin(), data.end());
        }

        auto encoded = encodeHeader(data.size(), 0x80, 0xb7);
        encoded.insert(encoded.end(), data.begin(), data.end());
        return encoded;
    }

    /// Encodes a list of elements.
    template <typename T>
    static DataChunk encodeList(T elements) noexcept {
        auto encodedData = DataChunk();
        for (const auto& el : elements) {
            auto encoded = encode(el);
            if (encoded.empty()) {
                return {};
            }
            encodedData.insert(encodedData.end(), encoded.begin(), encoded.end());
        }

        auto encoded = encodeHeader(encodedData.size(), 0xc0, 0xf7);
        encoded.insert(encoded.end(), encodedData.begin(), encodedData.end());
        return encoded;
    }

    /// Encodes a list header.
    static DataChunk encodeHeader(uint64_t size, uint8_t smallTag, uint8_t largeTag) noexcept;

    /// Returns the representation of an integer using the least number of bytes
    /// needed.
    static DataChunk putint(uint64_t i) noexcept;
}; // struct RLP end
} // namespace eth
} // namespace jub
