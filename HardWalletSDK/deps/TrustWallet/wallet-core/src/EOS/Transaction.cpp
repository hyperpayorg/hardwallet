// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "../Base58.h"
#include "../Hash.h"
#include "../HexCoding.h"
#include "Transaction.h"
#include "../Bravo/Deserialization.h"

#include <TrezorCrypto/ripemd160.h>
#include "mSIGNA/stdutils/uchar_vector.h"
#include "utility/util.h"

#include <ctime>
#include <stdexcept>

using namespace TW;
using namespace TW::EOS;
using json = nlohmann::json;

Signature::Signature(Data sig, Type type) : data(sig), type(type) {
    if (sig.size() != DataSize) {
        throw std::invalid_argument("Invalid signature size!");
    }

    if (type == Type::Legacy) {
        throw std::invalid_argument("Legacy signatures are not supported.");
    }
}

// JuBiter-defined
void Signature::deserialize(const std::string& string) noexcept {
    std::vector<std::string> vSignPart = jub::Split(string, "_");
//    supersplit(string, vSignPart, "_");
    if (3 != vSignPart.size()) {
        return ;
    }
    if (0 != vSignPart[0].compare(Modern::sigBasePrefix)) {
        return ;
    }

    deserialize(TW::Base58::bitcoin.decode(vSignPart[2]),
                sigTypeForPrefix(vSignPart[1]));
}

// JuBiter-defined
void Signature::deserialize(const Data& sig, const Type& type) noexcept {

    if ((DataSize+ChecksumSize) > sig.size()) {
        return;
    }

    uchar_vector vSignature(sig);
    Data forHash = vSignature.read_vector(sig.size()-ChecksumSize);
    Data checkSum = vSignature.read_vector(ChecksumSize);

    // append the subPrefix to the buffer data and hash the buffer
    std::string subPrefix = sigPrefixForType(type);
    for (const char& c : subPrefix) {
        forHash.push_back(static_cast<uint8_t>(c));
    }

    Data hash;
    hash.resize(RIPEMD160_DIGEST_LENGTH);
    ripemd160(forHash.data(), static_cast<uint32_t>(forHash.size()), hash.data());

    Data asCheckSum;
    for(size_t i = 0; i < ChecksumSize; i++) {
        asCheckSum.push_back(hash[i]);
    }

    if (asCheckSum != checkSum) {
        return ;
    }

    forHash.resize(DataSize);
    data = forHash;
}

// JuBiter-defined
void Signature::deserialize(const Data& sig) noexcept {

    if ((DataSize+ChecksumSize) > sig.size()) {
        return;
    }

    uchar_vector vSignature(sig);

    Data forHash = vSignature.read_vector(sig.size()-ChecksumSize);
    Data checkSum = vSignature.read_vector(ChecksumSize);
    const std::string subPrefixes[2] = {Modern::R1::prefix, Modern::K1::prefix};
    for (const auto& subPrefix : subPrefixes) {
        // append the subPrefix to the buffer data and hash the buffer
        for (const char& c : subPrefix) {
            forHash.push_back(static_cast<uint8_t>(c));
        }

        Data hash;
        hash.resize(RIPEMD160_DIGEST_LENGTH);
        ripemd160(forHash.data(), static_cast<uint32_t>(forHash.size()), hash.data());

        Data asCheckSum;
        for(size_t i = 0; i < ChecksumSize; i++) {
            asCheckSum.push_back(hash[i]);
        }

        if (asCheckSum == checkSum) {
            if (Modern::R1::prefix == subPrefix) {
                type = Type::ModernR1;
            }
            else if (Modern::K1::prefix == subPrefix) {
                type = Type::ModernK1;
            }
            else {
                type = Type::Legacy;
            }
            break;
        }

        forHash.resize(DataSize);
    }

    forHash.resize(DataSize);
    data = forHash;
}

void Signature::serialize(Data& os) const noexcept {
    // type should never be Legacy
    uint32_t typeId = type == Type::ModernK1 ? 0 : 1;
    Bravo::encodeVarInt32(typeId, os);
    os.insert(os.end(), data.begin(), data.end());
}

std::string Signature::string() const noexcept {
    const auto& prefix = sigPrefixForType(type);
    const auto& subPrefix = type == Type::ModernR1 ? Modern::R1::prefix : Modern::K1::prefix;

    Data buffer(data);

    // append the subPrefix to the buffer data and hash the buffer
    for (const char& c : subPrefix) {
        buffer.push_back(static_cast<uint8_t>(c));
    }

    Data hash;
    hash.resize(RIPEMD160_DIGEST_LENGTH);

    ripemd160(buffer.data(), static_cast<uint32_t>(buffer.size()), hash.data());

    // drop the subPrefix and append the checksum to the bufer
    buffer.resize(DataSize);

    for(size_t i = 0; i < ChecksumSize; i++) {
        buffer.push_back(hash[i]);
    }

    return prefix + TW::Base58::bitcoin.encode(buffer);
}

// JuBiter-defined
void Extension::deserialize(const Data& o) noexcept {
    uchar_vector vExtension(o);

    // type
    type = vExtension.read_le_uint16();

    // buffer
    uchar_vector vTempData(vExtension.get_cur_it(), vExtension.end());
    Data tempData(vTempData);
    uint64_t dataSize = 0;
    int varIntByteSize = 0;
    Bravo::decodeVarInt64(tempData, dataSize, varIntByteSize);
    vExtension.reset_it(varIntByteSize);
    buffer = vExtension.read_vector(dataSize);
}

// JuBiter-defined
size_t Extension::size() const noexcept {
    uint64_t dataSize = buffer.size();
    Data dataVarInt64Len;
    Bravo::encodeVarInt64(dataSize, dataVarInt64Len);

    return (  sizeof(uint16_t)/sizeof(uint8_t)
            + dataVarInt64Len.size() + dataSize);
}

void Extension::serialize(Data& os) const noexcept {
    encode16LE(type, os);
    Bravo::encodeVarInt64(buffer.size(), os);
    append(os, buffer);
}

json Extension::serialize() const noexcept {
    return json::array({type, hex(buffer)});
}

Transaction::Transaction(const Data& referenceBlockId, int32_t referenceBlockTime, int32_t expirySeconds) {
    setReferenceBlock(referenceBlockId);
    expiration = referenceBlockTime + expirySeconds;
}

// JuBiter-defined
void Transaction::deserialize(const Data& o, const bool bWithType) noexcept {
    uchar_vector vTrx(o);

    // expiration
    expiration = vTrx.read_le_uint32();

    // refBlockNumber
    refBlockNumber = vTrx.read_le_uint16();

    // refBlockPrefix
    refBlockPrefix = vTrx.read_le_uint32();

    // maxNetUsageWords
    uchar_vector vMaxNetUsageWords(vTrx.get_cur_it(), vTrx.end());
    Data tempMaxNetUsageWords(vMaxNetUsageWords);
    int varIntByteSize = 0;
    TW::Bravo::decodeVarInt32(tempMaxNetUsageWords, maxNetUsageWords, varIntByteSize);
    vTrx.reset_it(varIntByteSize);

    // maxCPUUsageInMS
    maxCPUUsageInMS  = vTrx.read_uint8();

    // delaySeconds
    uchar_vector vDelaySeconds(vTrx.get_cur_it(), vTrx.end());
    Data tempDelaySeconds(vDelaySeconds);
    varIntByteSize = 0;
    TW::Bravo::decodeVarInt32(tempDelaySeconds, delaySeconds, varIntByteSize);
    vTrx.reset_it(varIntByteSize);

    //Collection - contextFreeActions
    uchar_vector vCtxFreeActions(vTrx.get_cur_it(), vTrx.end());
    Data tempCtxFreeActions(vCtxFreeActions);
    uint64_t collectionCnt = 0;
    varIntByteSize = 0;
    Bravo::decodeCollection(tempCtxFreeActions, collectionCnt, varIntByteSize);
    vTrx.reset_it(varIntByteSize);
    uint16_t ctxFreeActionSize = 0;
    for (uint64_t i=0; i<collectionCnt; ++i) {
        uchar_vector vCtxFreeAction(vTrx.get_cur_it()+ctxFreeActionSize, vTrx.end());
        const TW::Data ctxFreeAct(vCtxFreeAction);

        Action ctxFreeAction;
        ctxFreeAction.deserialize(ctxFreeAct);
        contextFreeActions.push_back(ctxFreeAction);
        ctxFreeActionSize += ctxFreeAct.size();
    }
    vTrx.reset_it(ctxFreeActionSize);
    if (vTrx.end() == vTrx.get_cur_it()) {
        return;
    }

    //Collection - actions
    uchar_vector vActions(vTrx.get_cur_it(), vTrx.end());
    Data tempActions(vActions);
    collectionCnt = 0;
    varIntByteSize = 0;
    Bravo::decodeCollection(tempActions, collectionCnt, varIntByteSize);
    vTrx.reset_it(varIntByteSize);
    uint16_t actionSize = 0;
    for (uint8_t i=0; i<collectionCnt; ++i) {
        uchar_vector vAction(vTrx.get_cur_it()+actionSize, vTrx.end());
        const TW::Data a(vAction);

        Action action;
        if (!bWithType) {
            action.deserialize(a);
            actionSize += action.size();
        }
        else {
            action.deserializeWithType(a);
            actionSize += action.sizeWithType();
        }
        actions.push_back(action);
    }
    vTrx.reset_it(actionSize);
    if (vTrx.end() == vTrx.get_cur_it()) {
        return;
    }

    //Collection - transactionExtensions
    uchar_vector vTransactionExtensions(vTrx.get_cur_it(), vTrx.end());
    Data tempTransactionExtensions(vTransactionExtensions);
    collectionCnt = 0;
    varIntByteSize = 0;
    Bravo::decodeCollection(tempTransactionExtensions, collectionCnt, varIntByteSize);
    vTrx.reset_it(varIntByteSize);
    uint16_t txExtSize = 0;
    for (uint8_t i=0; i<collectionCnt; ++i) {
        uchar_vector vExtension(vTrx.get_cur_it()+txExtSize, vTrx.end());
        const TW::Data ext(vExtension);

        Extension extension;
        extension.deserialize(ext);
        transactionExtensions.push_back(extension);
        txExtSize += extension.size();
    }
    vTrx.reset_it(txExtSize);
}

// JuBiter-defined
uint16_t Transaction::actionCntLength() const noexcept {
    Data os;
    TW::Bravo::encodeVarInt64(actions.size(), os);

    return os.size();
}

void Transaction::setReferenceBlock(const Data& refBlockId) {
    if (refBlockId.size() != Hash::sha256Size) {
        throw std::invalid_argument("Invalid Reference Block Id!");
    }

    refBlockNumber = decode16BE(refBlockId.data() + 2);
    refBlockPrefix = decode32LE(refBlockId.data() + 8);
}

void Transaction::serialize(Data& os, const bool bWithType) const noexcept{
    using namespace Bravo;

    encode32LE(expiration, os);
    encode16LE(refBlockNumber, os);
    encode32LE(refBlockPrefix, os);
    encodeVarInt32(maxNetUsageWords, os);
    os.push_back(maxCPUUsageInMS);
    encodeVarInt32(delaySeconds, os);

    encodeCollection(contextFreeActions, os);
    if (!bWithType) {
        encodeCollection(actions, os);
    }
    else {
        encodeCollectionWithType(actions, os);
    }
    encodeCollection(transactionExtensions, os);
}

json Transaction::serialize() const {
    using namespace Bravo;

    // get a formatted date
    char formattedDate[20] = {0x00,};
    time_t time = expiration;
    if (strftime(formattedDate, 19, "%FT%T", std::gmtime(&time)) != 19) {
        std::runtime_error("Error creating a formatted string!");
    }

    // create a json array of signatures
    json sigs = json::array();
    for (const auto& sig : signatures) {
        sigs.push_back(sig.string());
    }

    // add everything to the json object
    json obj;
    obj["ref_block_num"] = refBlockNumber;
    obj["ref_block_prefix"] = refBlockPrefix;
    obj["expiration"] = std::string(formattedDate, 19);
    obj["max_net_usage_words"] = maxNetUsageWords;
    obj["max_cpu_usage_ms"] = maxCPUUsageInMS;
    obj["delay_sec"] = delaySeconds;
    obj["context_free_actions"] = encodeCollection(contextFreeActions);
    obj["actions"] = encodeCollection(actions);
    obj["transaction_extensions"] = encodeCollection(transactionExtensions);
    obj["signatures"] = sigs;
    obj["context_free_data"] = hex(contextFreeData);

    return obj;
}
