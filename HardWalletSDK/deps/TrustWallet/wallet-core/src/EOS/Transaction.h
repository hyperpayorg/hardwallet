// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../Data.h"
#include "Action.h"
#include "Prefixes.h"

#include <nlohmann/json.hpp>

#include <set>
#include <array>

namespace TW::EOS {

class Signature {
public:
    Data data;
    Type type;

    static const size_t DataSize = 65;
    static const size_t ChecksumSize = 4;

    // JuBiter-defined
    Signature() {};
    Signature(Data sig, Type type);
    virtual ~Signature() { }

    // JuBiter-defined
    void deserialize(const std::string& string) noexcept;
    // JuBiter-defined
    void deserialize(const Data& sig, const Type& type) noexcept;
    // JuBiter-defined
    void deserialize(const Data& sig) noexcept;
    void serialize(Data& os) const noexcept;
    std::string string() const noexcept;
}; // class Signature end

class Extension {
public:
    uint16_t type;
    Data buffer;

    // JuBiter-defined
    Extension() {};
    Extension(uint16_t type, Data buffer) : type(type), buffer(buffer) { }
    virtual ~Extension() { }

    // JuBiter-defined
    void deserialize(const Data& o) noexcept;
    // JuBiter-defined
    virtual size_t size() const noexcept;

    void serialize(Data& os) const noexcept;
    nlohmann::json serialize() const noexcept;
}; // class Extension end

class Transaction {
public:
    // JuBiter-defined
    Transaction() {};
    Transaction(const Data& referenceBlockId, int32_t referenceBlockTime, int32_t expirySeconds = Transaction::ExpirySeconds);

    // JuBiter-defined
    void deserialize(const Data& o, const bool bWithType=false) noexcept;
    // JuBiter-defined
    uint16_t actionCntLength() const noexcept;

    void serialize(Data& os, const bool bWithType=false) const noexcept;
    nlohmann::json serialize() const;

    // JuBiter-modified
    inline bool isValid() const { return maxNetUsageWords < UINT32_MAX / 8UL; }

    uint16_t refBlockNumber = 0;
    uint32_t refBlockPrefix = 0;
    int32_t expiration = 0;
    uint32_t maxNetUsageWords = 0;
    uint8_t maxCPUUsageInMS = 0;
    uint32_t delaySeconds = 0;

    std::vector<Action> actions;
    std::vector<Action> contextFreeActions;
    std::vector<Extension> transactionExtensions;
    std::vector<Signature> signatures;

    Data contextFreeData;

    void setReferenceBlock(const Data& referenceBlockId);

    static const int32_t ExpirySeconds = 30;
}; // class Transaction end
} // namespace TW::EOS end
