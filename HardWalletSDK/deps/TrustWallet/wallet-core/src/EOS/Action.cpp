// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Action.h"
#include "../HexCoding.h"
#include "../Bravo/Serialization.h"
#include "../Bravo/Deserialization.h"
#include "mSIGNA/stdutils/uchar_vector.h"

using namespace TW;
using namespace TW::EOS;
using json = nlohmann::json;

// JuBiter-defined
size_t PermissionLevel::size() {
    return (sizeof(uint64_t)/sizeof(uint8_t))*2;
}

void PermissionLevel::serialize(Data& o) const {
    actor.serialize(o);
    permission.serialize(o);
}

json PermissionLevel::serialize() const noexcept {
    json obj;
    obj["actor"] = actor.string();
    obj["permission"] = permission.string();
    return obj;
}

// JuBiter-defined
void Action::deserialize(const Data& o) noexcept {
    uchar_vector vAction(o);

    // account
    account.value = vAction.read_le_uint64();

    // name
       name.value = vAction.read_le_uint64();

    // Collection - authorizations
    uchar_vector vTempAuthorizations(vAction.get_cur_it(), vAction.end());
    Data tempTempAuthorizations(vTempAuthorizations);
    uint64_t authorizationCnt = 0;
    int varIntByteSize = 0;
    Bravo::decodeVarInt64(tempTempAuthorizations, authorizationCnt, varIntByteSize);
    vAction.reset_it(varIntByteSize);
    for (uint64_t i=0; i<authorizationCnt; ++i) {
        Name actor, permission;
        actor.value = vAction.read_le_uint64();
        permission.value = vAction.read_le_uint64();

        PermissionLevel auth(actor, permission);
        authorization.push_back(auth);
    }

    // data
    uchar_vector vTempData(vAction.get_cur_it(), vAction.end());
    Data tempData(vTempData);
    uint64_t dataSize = 0;
    varIntByteSize = 0;
    Bravo::decodeVarInt64(tempData, dataSize, varIntByteSize);
    vAction.reset_it(varIntByteSize);
    data = vAction.read_vector(dataSize);
}

// JuBiter-defined
void Action::deserializeWithType(const Data& o) noexcept {
    type = o[0];
    Data a(o.begin()+1, o.end());
    Action::deserialize(a);
}

// JuBiter-defined
uint64_t Action::size() const noexcept {
    uint64_t authorizationCnt = authorization.size();
    Data authorizationVarInt64Len;
    Bravo::encodeVarInt64(authorizationCnt, authorizationVarInt64Len);

    uint64_t dataSize = data.size();
    Data dataVarInt64Len;
    Bravo::encodeVarInt64(dataSize, dataVarInt64Len);

    return (  Name::size()
            + Name::size()
            + authorizationVarInt64Len.size() + PermissionLevel::size() * authorizationCnt
            + dataVarInt64Len.size() + dataSize);
}

// JuBiter-defined
uint64_t Action::sizeWithType() const noexcept {
    return (1 + Action::size());
}

// JuBiter-defined
uint16_t Action::startingPostData() const noexcept {
    uint16_t actionOffset = 0;
    actionOffset += account.size();
    actionOffset += name.size();
    uint16_t authOffset = 1;    // auth count
    for (const TW::EOS::PermissionLevel& auth : authorization) {
        authOffset += auth.actor.size();
        authOffset += auth.permission.size();
    }
    actionOffset += authOffset;

    Data dataVarInt64Len;
    TW::Bravo::encodeVarInt64(data.size(), dataVarInt64Len);
    actionOffset += dataVarInt64Len.size();  // data length

    return actionOffset;
}

void Action::serialize(Data& o) const {
    account.serialize(o);
    name.serialize(o);
    Bravo::encodeCollection(authorization, o);
    Bravo::encodeVarInt64(data.size(), o);
    append(o, data);
}

// JuBiter-defined
void Action::serializeWithType(Data& o) const {
    o.insert(o.end(), type);
    Action::serialize(o);
}

nlohmann::json Action::serialize() const noexcept {
    nlohmann::json obj;
    if (   0 == account.value
        || 0 == name.value
        || 0 == authorization.size()
        || 0 == data.size()
        ) {
        return obj;
    }

    obj["account"] = account.string();
    obj["name"] = name.string();
    obj["authorizations"] = Bravo::encodeCollection(authorization);
//    obj["data"] = hex(data);
    obj["hex_data"] = hex(data);

    return obj;
}

// JuBiter-defined
//{
//    "account":"metpacktoken",
//    "authorizations":[{
//        "actor":"abcdenero123",
//        "permission":"active"
//    }],
//    "data":{
//        ...
//    },
//    "hex_data":"...",
//    "name":"..."
//}
void Action::deserialize(const nlohmann::json& inJson) noexcept {
    account = Name(inJson["account"]);
    name = Name(inJson["name"]);
    for (int i=0; i<inJson["authorizations"].size(); ++i) {
        authorization.push_back(
            PermissionLevel(Name(inJson["authorizations"][i]["actor"]),
                            Name(inJson["authorizations"][i]["permission"]))
        );
    }
}

TransferAction::TransferAction(const std::string& currency,
                               const std::string& actName,
                               const std::string& from,
                               const std::string& to,
                               const Bravo::Asset& asset,
                               const std::string& memo) {
    account = Name(currency);
//    name = Name("transfer");
    name = Name(actName);
    authorization.push_back(PermissionLevel(Name(from), Name("active")));

    setData(from, to, asset, memo);
}

void TransferAction::setData(const std::string& from, const std::string& to, const Bravo::Asset& asset, const std::string& memo) {
    if (asset.amount <= 0) {
        throw std::invalid_argument("Amount in a transfer action must be greater than zero.");
    }

    this->from = Name(from);
    this->to = Name(to);
    this->asset = asset;
    Bravo::encodeString(memo, this->memo);

    this->from.serialize(data);
    this->to.serialize(data);
    this->asset.serialize(data);
    copy(this->memo.begin(), this->memo.end(), back_inserter(data));
}

// JuBiter-defined
void TransferAction::deserialize(const Data& o) noexcept {
    uchar_vector vTransfer(o);
    from.value = vTransfer.read_le_uint64();
      to.value = vTransfer.read_le_uint64();

    //Bravo::Asset
    asset.amount = vTransfer.read_le_uint64();
    asset.symbol = vTransfer.read_le_uint64();

    //memo
    uchar_vector vTemp(vTransfer.get_cur_it(), vTransfer.end());
    Data temp(vTemp);
    memo = temp;
}

// JuBiter-defined
uint16_t TransferAction::startingPostData() const noexcept {
    uint16_t memoOffset = 0;

    memoOffset += from.size();
    memoOffset += to.size();
    memoOffset += asset.size();

    Data os;
    TW::Bravo::encodeVarInt64(memo.size(), os);
    memoOffset += os.size();

    return memoOffset;
}

// JuBiter-defined
uint16_t TransferAction::memoLength() const noexcept {
    uint16_t memoLength = memo.size();

    Data os;
    TW::Bravo::encodeVarInt64(memo.size(), os);
    memoLength -= os.size();

    return memoLength;
}

// JuBiter-defined
void TransferAction::serialize(Data& o) const {
    append(o, data);
}

// JuBiter-defined
nlohmann::json TransferAction::serialize() const noexcept {
    nlohmann::json obj = Action::serialize();

    nlohmann::json data;
    data["from"] = from.string();
    data["to"] = to.string();
    data["quantity"] = asset.string();
    std::string memo;
    int varIntByteSize = 0;
    Bravo::decodeString(this->memo, memo, varIntByteSize);
    data["memo"] = memo;

    if (obj.empty()) {
        return data;
    }
    else {
        obj["data"] = data;
        return obj;
    }
}

// JuBiter-defined
//{
//    "account":"metpacktoken",
//    "authorizations":[
//    {
//        "actor":"abcdenero123",
//        "permission":"active"
//    }],
//    "data":{
//        "from":"abcdenero123",
//        "memo":"@gmail.com",
//        "quantity":"14.285 MPT",
//        "to":"gsy123451111"
//    },
//    "hex_data":"3044a0574d95d031104208850c113c66cd37000000000000034d5054000000000a40676d61696c2e636f6d",
//    "name":"transfer"
//}
void TransferAction::deserialize(const nlohmann::json& inJson) noexcept {
    Action::deserialize(inJson);

    setData(inJson["data"]["from"], inJson["data"]["to"],
            TW::Bravo::Asset::fromString(inJson["data"]["quantity"]),
            inJson["data"]["memo"]);
}

// JuBiter-defined
DelegateAction::DelegateAction(const std::string& currency,
                               const std::string& actName,
                               const std::string& from,
                               const std::string& receiver,
                               const Bravo::Asset& netQty,
                               const Bravo::Asset& cpuQty,
                               bool bStake) {
    account = Name(currency);
//    name = Name("delegatebw");
//    name = Name("undelegatebw");
    name = Name(actName);
    authorization.push_back(PermissionLevel(Name(from), Name("active")));

    transfer = 0x00;
    setData(from, receiver,
            netQty, cpuQty,
            transfer);

    _bStake = bStake;
}

// JuBiter-defined
void DelegateAction::setData(const std::string& from, const std::string& receiver,
                             const Bravo::Asset& netQty, const Bravo::Asset& cpuQty,
                             uint8_t transfer) {
    if (netQty.amount <= 0) {
        throw std::invalid_argument("Amount in a transfer action must be greater than zero.");
    }
    if (cpuQty.amount <= 0) {
        throw std::invalid_argument("Amount in a transfer action must be greater than zero.");
    }

    this->from = Name(from);
    this->receiver = Name(receiver);
    this->netQty = netQty;
    this->cpuQty = cpuQty;
    this->transfer = transfer;

    this->from.serialize(data);
    this->receiver.serialize(data);
    this->netQty.serialize(data);
    this->cpuQty.serialize(data);
    data.push_back(transfer);
}

// JuBiter-defined
//{
//    "account":"eosio",
//    "authorizations":[
//    {
//        "actor":"ftsafetest55",
//        "permission":"active"
//    }],
//    "data":{
//        "from":"ftsafetest55",
//        "receiver":"ftsafetest55",
//        "stake_cpu_quantity":"0.7000 EOS",
//        "stake_net_quantity":"0.3000 EOS",
//        "transfer":0
//    },
//    "hex_data":"504ac62aab65705e504ac62aab65705eb80b00000000000004454f5300000000581b00000000000004454f530000000000",
//    "name":"delegatebw"
//}
void DelegateAction::deserialize(const Data& o) noexcept {
    uchar_vector vDelegate(o);
    from.value = vDelegate.read_le_uint64();
receiver.value = vDelegate.read_le_uint64();

    //Bravo::Asset
    netQty.amount = vDelegate.read_le_uint64();
    netQty.symbol = vDelegate.read_le_uint64();

    //Bravo::Asset
    cpuQty.amount = vDelegate.read_le_uint64();
    cpuQty.symbol = vDelegate.read_le_uint64();

    transfer = vDelegate.read_uint8();
}

// JuBiter-defined
void DelegateAction::serialize(Data& o) const {
    append(o, data);
}

// JuBiter-defined
nlohmann::json DelegateAction::serialize() const noexcept {
    nlohmann::json obj = Action::serialize();

    nlohmann::json data;
    data["from"] = from.string();
    data["receiver"] = receiver.string();
    if (_bStake) {
        data["stake_cpu_quantity"] = cpuQty.string();
        data["stake_net_quantity"] = netQty.string();
    }
    else {
        data["unstake_cpu_quantity"] = cpuQty.string();
        data["unstake_net_quantity"] = netQty.string();
    }
    data["transfer"] = transfer;

    if (obj.empty()) {
        return data;
    }
    else {
        obj["data"] = data;
        return obj;
    }
}

// JuBiter-defined
void DelegateAction::deserialize(const nlohmann::json& inJson) noexcept {
    Action::deserialize(inJson);

    if (_bStake) {
        setData(inJson["data"]["from"], inJson["data"]["receiver"],
                TW::Bravo::Asset::fromString(inJson["data"]["stake_net_quantity"]),
                TW::Bravo::Asset::fromString(inJson["data"]["stake_cpu_quantity"]),
                inJson["data"]["transfer"]);
    }
    else {
    setData(inJson["data"]["from"], inJson["data"]["receiver"],
            TW::Bravo::Asset::fromString(inJson["data"]["unstake_net_quantity"]),
            TW::Bravo::Asset::fromString(inJson["data"]["unstake_cpu_quantity"]),
            inJson["data"]["transfer"]);
    }
}

// JuBiter-defined
BuyRamAction::BuyRamAction(const std::string& currency, const std::string& actName,
                           const std::string& payer, const std::string& receiver,
                           const Bravo::Asset& quant) {
    account = Name(currency);
//    name = Name("buyram");
    name = Name(actName);
    authorization.push_back(PermissionLevel(Name(payer), Name("owner")));

    setData(payer, receiver,
            quant);
}

// JuBiter-defined
void BuyRamAction::setData(const std::string& payer, const std::string& receiver,
                           const Bravo::Asset& quant) {
    if (quant.amount <= 0) {
        throw std::invalid_argument("Amount in a transfer action must be greater than zero.");
    }

    this->payer = Name(payer);
    this->receiver = Name(receiver);
    this->quant = quant;

    this->payer.serialize(data);
    this->receiver.serialize(data);
    this->quant.serialize(data);
}

// JuBiter-defined
//{
//    "account":"eosio",
//    "authorizations":[
//    {
//        "actor":"punkneverdie",
//        "permission":"owner"
//    }],
//    "data":{
//        "payer":"punkneverdie",
//        "quant":"0.4000 EOS",
//        "receiver":"punkneverdie"
//    },
//    "hex_data":"a05cba6aab09a7aea05cba6aab09a7aea00f00000000000004454f5300000000",
//    "name":"buyram"
//}
void BuyRamAction::deserialize(const Data& o) noexcept {
    uchar_vector vBuyRam(o);
   payer.value = vBuyRam.read_le_uint64();
receiver.value = vBuyRam.read_le_uint64();

    //Bravo::Asset
    quant.amount = vBuyRam.read_le_uint64();
    quant.symbol = vBuyRam.read_le_uint64();
}

// JuBiter-defined
void BuyRamAction::serialize(Data& o) const {
    append(o, data);
}

// JuBiter-defined
nlohmann::json BuyRamAction::serialize() const noexcept {
    nlohmann::json obj = Action::serialize();

    nlohmann::json data;
    data["payer"] = payer.string();
    data["quant"] = quant.string();
    data["receiver"] = receiver.string();

    if (obj.empty()) {
        return data;
    }
    else {
        obj["data"] = data;
        return obj;
    }

    return obj;
}

// JuBiter-defined
//{
//    "account":"eosio",
//    "authorizations":[
//    {
//        "actor":"punkneverdie",
//        "permission":"owner"
//    }],
//    "data":{
//        "payer":"punkneverdie",
//        "quant":"0.4000 EOS",
//        "receiver":"punkneverdie"
//    },
//    "hex_data":"a05cba6aab09a7aea05cba6aab09a7aea00f00000000000004454f5300000000",
//    "name":"buyram"
//}
void BuyRamAction::deserialize(const nlohmann::json& inJson) noexcept {
    Action::deserialize(inJson);

    setData(inJson["data"]["payer"], inJson["data"]["receiver"],
            TW::Bravo::Asset::fromString(inJson["data"]["quant"]));
}

// JuBiter-defined
SellRamAction::SellRamAction(const std::string& currency, const std::string& actName,
                             const std::string& acct, const int64_t bytes) {
    account = Name(currency);
//    name = Name("sellram");
    name = Name(actName);
    authorization.push_back(PermissionLevel(Name(acct), Name("owner")));

    setData(acct, bytes);
}

// JuBiter-defined
void SellRamAction::setData(const std::string& acct, const int64_t bytes) {

    this->acct = Name(acct);
    this->bytes = bytes;

    this->acct.serialize(data);
    encode64LE(bytes, data);
}

// JuBiter-defined
//{
//    "account":"eosio",
//    "authorizations":[
//    {
//        "actor":"gq4demjrhege",
//        "permission":"owner"
//    }],
//    "data":{
//        "account":"gq4demjrhege",
//        "bytes":134246400
//    },
//    "hex_data":"a0986af7499588650070000800000000",
//    "name":"sellram"
//}
void SellRamAction::deserialize(const Data& o) noexcept {
    uchar_vector vSellRam(o);
    account.value = vSellRam.read_le_uint64();
    bytes = vSellRam.read_le_uint64();
}

// JuBiter-defined
void SellRamAction::serialize(Data& o) const {
    append(o, data);
}

// JuBiter-defined
nlohmann::json SellRamAction::serialize() const noexcept {
    nlohmann::json obj = Action::serialize();

    nlohmann::json data;
    data["account"] = acct.string();
    data["bytes"] = bytes;

    if (obj.empty()) {
        return data;
    }
    else {
        obj["data"] = data;
        return obj;
    }

    return obj;
}

// JuBiter-defined
//{
//    "account":"eosio",
//    "authorizations":[
//    {
//        "actor":"gq4demjrhege",
//        "permission":"owner"
//    }],
//    "data":{
//        "account":"gq4demjrhege",
//        "bytes":134246400
//    },
//    "hex_data":"a0986af7499588650070000800000000",
//    "name":"sellram"
//}
void SellRamAction::deserialize(const nlohmann::json& inJson) noexcept {
    Action::deserialize(inJson);

    setData(inJson["data"]["account"], inJson["data"]["bytes"]);
}
