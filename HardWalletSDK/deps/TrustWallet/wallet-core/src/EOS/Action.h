// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../Bravo/Asset.h"
#include "Name.h"

#include <nlohmann/json.hpp>
#include <string>

using Data = TW::Data;

namespace TW::EOS {

class PermissionLevel {
public:
    Name actor, permission;

    PermissionLevel(const Name& actor, const Name& permission) : actor(actor), permission(permission) { }
    virtual ~PermissionLevel() { }

    // JuBiter-defined
    static size_t size();

    void serialize(Data& o) const;
    nlohmann::json serialize() const noexcept;
}; // class PermissionLevel end

class Action {
public:
    Name account, name;
    std::vector<PermissionLevel> authorization;
    Data data;

    virtual ~Action() { }

    void setType(uint8_t t) {
        type = t;
    }
    uint8_t getType() {
        return type;
    }

    // JuBiter-defined
    virtual void deserialize(const Data& o) noexcept;
    // JuBiter-defined
    virtual void deserializeWithType(const Data& o) noexcept;
    // JuBiter-defined
    virtual uint64_t size() const noexcept;
    // JuBiter-defined
    virtual uint64_t sizeWithType() const noexcept;
    // JuBiter-defined
    virtual uint16_t startingPostData() const noexcept;

    virtual void serialize(Data& o) const;
    // JuBiter-defined
    virtual void serializeWithType(Data& o) const;
    virtual nlohmann::json serialize() const noexcept;
    // JuBiter-defined
    virtual void deserialize(const nlohmann::json& inJson) noexcept;

private:
    uint8_t type;
}; // class Action end

class TransferAction: public Action {
public:
    TransferAction(const std::string& currency, const std::string& actName,
                   const std::string& from, const std::string& to, const Bravo::Asset& asset, const std::string& memo);
    // JuBiter-defined
    TransferAction() { }

    // JuBiter-defined
    virtual void deserialize(const Data& o) noexcept override;
    // JuBiter-defined
    virtual uint16_t startingPostData() const noexcept override;
    // JuBiter-defined
    virtual uint16_t memoLength() const noexcept;
    // JuBiter-defined
    virtual void serialize(Data& o) const override;
    // JuBiter-defined
    virtual nlohmann::json serialize() const noexcept override;
    // JuBiter-defined
    virtual void deserialize(const nlohmann::json& inJson) noexcept override;

private:
    void setData(const std::string& from, const std::string& to, const Bravo::Asset& asset, const std::string& memo);

public:
    // JuBiter-defined
    Name from, to;
    // JuBiter-defined
    Bravo::Asset asset;
    // JuBiter-defined
    Data memo;
}; // class TransferAction end

// JuBiter-defined
class DelegateAction: public Action {
public:
    DelegateAction(const std::string& currency, const std::string& actName,
                   const std::string& from, const std::string& receiver, const Bravo::Asset& netQty, const Bravo::Asset& cpuQty, bool bStake = false);
    DelegateAction(bool bStake = false) {
        transfer = 0x00;
        _bStake = bStake;
    }

    virtual void deserialize(const Data& o) noexcept override;
    virtual void serialize(Data& o) const override;
    virtual nlohmann::json serialize() const noexcept override;
    virtual void deserialize(const nlohmann::json& inJson) noexcept override;

private:
    void setData(const std::string& from, const std::string& receiver,
                 const Bravo::Asset& netQty, const Bravo::Asset& cpuQty,
                 uint8_t transfer);
    bool _bStake;

public:
    Name from, receiver;
    Bravo::Asset netQty;
    Bravo::Asset cpuQty;
    uint8_t transfer;
}; // class DelegateAction end

// JuBiter-defined
class BuyRamAction: public Action {
public:
    BuyRamAction(const std::string& currency, const std::string& actName,
                 const std::string& payer, const std::string& receiver,
                 const Bravo::Asset& quant);
    BuyRamAction() { }

    virtual void deserialize(const Data& o) noexcept override;
    virtual void serialize(Data& o) const override;
    virtual nlohmann::json serialize() const noexcept override;
    virtual void deserialize(const nlohmann::json& inJson) noexcept override;

private:
    void setData(const std::string& payer, const std::string& receiver,
                 const Bravo::Asset& quant);

public:
    Name payer, receiver;
    Bravo::Asset quant;
}; // class BuyRamAction end

// JuBiter-defined
class SellRamAction: public Action {
public:
    SellRamAction(const std::string& currency, const std::string& actName,
                  const std::string& acct, const int64_t bytes);
    SellRamAction() { }

    virtual void deserialize(const Data& o) noexcept override;
    virtual void serialize(Data& o) const override;
    virtual nlohmann::json serialize() const noexcept override;
    virtual void deserialize(const nlohmann::json& inJson) noexcept override;

private:
    void setData(const std::string& acct, const int64_t bytes);

public:
    Name acct;
    int64_t bytes;
}; // class SellRamAction end

} // namespace TW::EOS end
