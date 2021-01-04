// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "BinaryCoding.h"
#include "Transaction.h"
#include "../BinaryCoding.h"
#include "../HexCoding.h"

#include "mSIGNA/stdutils/uchar_vector.h"

using namespace TW;
using namespace TW::Ripple;

const int TW::Ripple::NETWORK_PREFIX = 0x53545800;
// JuBiter-added
const int TW::Ripple::NETWORK_PREFIX_MULTI_SIGN = 0x534D5400;
const int64_t MAX_ALLOWED_AMOUNT = 100000000000;

// JuBiter-defined
void Transaction::deserialize(const Data& o) {
    uchar_vector vTrx(o);

    int key = 0;
    /// "type"
    uchar_vector vTypeField;
    vTypeField << vTrx.read_uint8();
    FieldType typeField = decodeType(key, vTypeField);
    uint16_t type = vTrx.read_be_uint16();
    if (!(FieldType::int16 == typeField
          &&             2 == key)
        || uint16_t(TransactionType::payment) != type
        ) {
        throw std::invalid_argument("Invalid transaction type");
    }

    /// "flags"
    vTypeField.clear();
    vTypeField << vTrx.read_uint8();
    typeField = decodeType(key, vTypeField);
    if (FieldType::int32 != typeField
        ||             2 != key
        ) {
        throw std::invalid_argument("Invalid transaction flags");
    }
    flags = vTrx.read_be_uint32();

    /// "sequence"
    vTypeField.clear();
    vTypeField << vTrx.read_uint8();
    typeField = decodeType(key, vTypeField);
    if (FieldType::int32 != typeField
        ||             4 != key
        ) {
        throw std::invalid_argument("Invalid transaction sequence");
    }
    sequence = vTrx.read_be_uint32();

    /// "destinationTag"
    int offset = (int)(vTrx.get_cur_it() - vTrx.begin());
    vTypeField.clear();
    vTypeField << vTrx.read_uint8();
    typeField = decodeType(key, vTypeField);
    if (FieldType::int32 != typeField
        ||            14 != key
        ) {
        destination_tag = 0;
        vTrx.reset_it();
        vTrx.reset_it(offset);
    }
    else {
        destination_tag = vTrx.read_be_uint32();
    }

    /// "lastLedgerSequence"
    offset = (int)(vTrx.get_cur_it() - vTrx.begin());
    vTypeField.clear();
    vTypeField << vTrx.read_le_uint16();
    typeField = decodeType(key, vTypeField);
    if (FieldType::int32 != typeField
        ||            27 != key
        ) {
        last_ledger_sequence = 0;
        vTrx.reset_it();
        vTrx.reset_it(offset);
    }
    else {
        last_ledger_sequence = vTrx.read_be_uint32();
    }

    /// "amount"
    vTypeField.clear();
    vTypeField << vTrx.read_uint8();
    typeField = decodeType(key, vTypeField);
    if (FieldType::amount != typeField
        ||              1 != key
        ) {
        throw std::invalid_argument("Invalid transaction amount");
    }
    uint8_t amountArray[8] = {0,};
    int amountLen = sizeof(amountArray)/sizeof(uint8_t);
    std::copy(vTrx.get_cur_it(), vTrx.get_cur_it()+amountLen, amountArray);
    uchar_vector vAmount((unsigned char*)(&amountArray), amountLen);
    amount = deserializeAmount(vAmount);
    offset = (int)(vTrx.get_cur_it() - vTrx.begin());
    offset += amountLen;
    vTrx.reset_it();
    vTrx.reset_it(offset);

    /// "fee"
    vTypeField.clear();
    vTypeField << vTrx.read_uint8();
    typeField = decodeType(key, vTypeField);
    if (FieldType::amount != typeField
        ||              8 != key
        ) {
        throw std::invalid_argument("Invalid transaction fee");
    }
    uint8_t feeArray[8] = {0,};
    int feeLen = sizeof(feeArray)/sizeof(uint8_t);
    std::copy(vTrx.get_cur_it(), vTrx.get_cur_it()+feeLen, feeArray);
    uchar_vector vFee((unsigned char*)(&feeArray), feeLen);
    fee = deserializeAmount(vFee);
    offset = (int)(vTrx.get_cur_it() - vTrx.begin());
    offset += amountLen;
    vTrx.reset_it();
    vTrx.reset_it(offset);

    /// "signingPubKey"
    offset = (int)(vTrx.get_cur_it() - vTrx.begin());
    vTypeField.clear();
    vTypeField << vTrx.read_uint8();
    typeField = decodeType(key, vTypeField);
    if (FieldType::vl != typeField
        ||          3 != key
        ) {
        pub_key.clear();
        vTrx.reset_it();
        vTrx.reset_it(offset);
    }
    else {
        Data rPubkey(vTrx.get_cur_it(), vTrx.end());
        size_t lengthCnt = 0;
        decodeBytes(rPubkey, pub_key, lengthCnt);
        offset = (int)(vTrx.get_cur_it() - vTrx.begin());
        offset += lengthCnt;
        offset += pub_key.size();
        vTrx.reset_it();
        vTrx.reset_it(offset);
    }

    /// "txnSignature"
    offset = (int)(vTrx.get_cur_it() - vTrx.begin());
    vTypeField.clear();
    vTypeField << vTrx.read_uint8();
    typeField = decodeType(key, vTypeField);
    if (FieldType::vl != typeField
        ||          4 != key
        ) {
        signature.clear();
        vTrx.reset_it();
        vTrx.reset_it(offset);
    }
    else {
        Data rSignature(vTrx.get_cur_it(), vTrx.end());
        size_t lengthCnt = 0;
        decodeBytes(rSignature, signature, lengthCnt);
        offset = (int)(vTrx.get_cur_it() - vTrx.begin());
        offset += lengthCnt;
        offset += signature.size();
        vTrx.reset_it();
        vTrx.reset_it(offset);
    }

    /// "account"
    vTypeField.clear();
    vTypeField << vTrx.read_uint8();
    typeField = decodeType(key, vTypeField);
    if (FieldType::account != typeField
        ||               1 != key
        ) {
        throw std::invalid_argument("Invalid transaction account");
    }
    uint8_t acctArray[TW::Ripple::Address::size] = {0,};
    int acctLen = sizeof(acctArray)/sizeof(uint8_t);
    std::copy(vTrx.get_cur_it(), vTrx.get_cur_it()+acctLen, acctArray);
    uchar_vector vAcct((unsigned char*)(&acctArray), acctLen);
    account = deserializeAddress(vAcct);
    offset = (int)(vTrx.get_cur_it() - vTrx.begin());
    offset += acctLen;
    vTrx.reset_it();
    vTrx.reset_it(offset);

    /// "destination"
    vTypeField.clear();
    vTypeField << vTrx.read_uint8();
    typeField = decodeType(key, vTypeField);
    if (FieldType::account != typeField
        ||               3 != key
        ) {
        throw std::invalid_argument("Invalid transaction destination");
    }
    uint8_t destArray[TW::Ripple::Address::size] = {0,};
    int destLen = sizeof(destArray)/sizeof(uint8_t);
    std::copy(vTrx.get_cur_it(), vTrx.get_cur_it()+destLen, destArray);
    uchar_vector vDest((unsigned char*)(&destArray), destLen);
    destination = deserializeAddress(vDest);
    offset = (int)(vTrx.get_cur_it() - vTrx.begin());
    offset += destLen;
    vTrx.reset_it();
    vTrx.reset_it(offset);

    if (o.size() <= offset) {
        return ;
    }

    /// "memos"
    vTypeField.clear();
    vTypeField << vTrx.read_uint8();
    typeField = decodeType(key, vTypeField);
    if (FieldType::starray != typeField
        ||               9 != key
        ) {
        throw std::invalid_argument("Invalid transaction memos");
    }

    Data vMemo(vTrx.get_cur_it(), vTrx.end()-1);
    memo.deserialize(vMemo);

    offset = (int)(vTrx.get_cur_it() - vTrx.begin());
    offset += memo.size();
    vTrx.reset_it();
    vTrx.reset_it(offset);

    vTypeField.clear();
    vTypeField << vTrx.read_uint8();
    typeField = decodeType(key, vTypeField);
    if (        FieldType::starray  != typeField
        || (int)FieldObjectKey::end != key
        ) {
        throw std::invalid_argument("Invalid transaction memos end");
    }
}

// JuBiter-defined
size_t Transaction::size() {
    Data tx = serialize();
    return tx.size();
}

Data Transaction::serialize() {
    uint16_t currDataLen = 0;
    size_t varLen = 0;

    auto data = Data();
    /// field must be sorted by field type then by field name
    /// "type"
    encodeType(FieldType::int16, 2, data);
    encode16BE(uint16_t(TransactionType::payment), data);
    /// "flags"
    encodeType(FieldType::int32, 2, data);
    encode32BE(static_cast<uint32_t>(flags), data);
    /// "sequence"
    encodeType(FieldType::int32, 4, data);
    encode32BE(sequence, data);
    /// "destinationTag"
    if (destination_tag > 0) {
        encodeType(FieldType::int32, 14, data);
        currDataLen = data.size();
        encode32BE(static_cast<uint32_t>(destination_tag), data);
    }
    setDestinationTagIndex(currDataLen);
    /// "lastLedgerSequence"
    if (last_ledger_sequence > 0) {
        encodeType(FieldType::int32, 27, data);
        encode32BE(last_ledger_sequence, data);
    }
    /// "amount"
    encodeType(FieldType::amount, 1, data);
    setAmountIndex(data.size());
    append(data, serializeAmount(amount));
    /// "fee"
    encodeType(FieldType::amount, 8, data);
    setFeeIndex(data.size());
    append(data, serializeAmount(fee));
    /// "signingPubKey"
    if (!pub_key.empty()) {
        encodeType(FieldType::vl, 3, data);
        currDataLen = data.size();
        encodeBytes(pub_key, data, varLen);
        currDataLen += varLen;
        setSignPubkeyIndex(currDataLen);
    }
    /// "txnSignature"
    if (!signature.empty()) {
        encodeType(FieldType::vl, 4, data);
        encodeBytes(signature, data, varLen);
    }
    /// "account"
    encodeType(FieldType::account, 1, data);
    encodeBytes(serializeAddress(account), data, varLen);
    /// "destination"
    encodeType(FieldType::account, 3, data);
    currDataLen = data.size();
    encodeBytes(serializeAddress(destination), data, varLen);
    currDataLen += varLen;
    setDestinationIndex(currDataLen);

    /// "memos"
    if (!memo.isEmpty()) {
        encodeType(FieldType::starray, 9, data);

        Data memoByte;
        memo.serialize(memoByte);

        currDataLen = data.size();
        currDataLen += memo.getMemoDataIndex();
        setShowMemoIndex(currDataLen);

        append(data, memoByte);

        encodeType(FieldType::starray, (int)FieldObjectKey::end, data);
    }

    return data;
}

// JuBiter-defined
void Transaction::setPreImage(const Data& o) {
    uchar_vector vPreImage(o);
    uint8_t netWorkPrefixArr[4] = {0,};
    std::copy(vPreImage.begin(), vPreImage.begin() + 4, netWorkPrefixArr);
    uint32_t netWorkPrefix = decode32BE(netWorkPrefixArr);
    if (   NETWORK_PREFIX            != netWorkPrefix
        && NETWORK_PREFIX_MULTI_SIGN != netWorkPrefix
        ) {
        throw std::invalid_argument("Invalid transaction network prefix");
    }
    vPreImage.reset_it(4);

    uchar_vector vTrx(vPreImage.get_cur_it(), vPreImage.end());
    Data trx = vTrx;
    deserialize(trx);
}

// JuBiter-defined
size_t Transaction::sizePreImage() {
    Data preImage = getPreImage();
    return preImage.size();
}

Data Transaction::getPreImage() {
    auto preImage = Data();
    encode32BE(NETWORK_PREFIX, preImage);
    append(preImage, serialize());
    return preImage;
}

Data Transaction::getNetworkPrefix() {
    auto prefix = Data();
    encode32BE(NETWORK_PREFIX, prefix);
    return prefix;
}

// JuBiter-defined
int64_t Transaction::deserializeAmount(const Data& o) {
    uint8_t vAmount[8] = {0,};
    std::copy(o.begin(), o.end(), vAmount);
    vAmount[0] &= 0x0B;
    return decode64BE(vAmount);
}

Data Transaction::serializeAmount(int64_t amount) {
    if (amount > MAX_ALLOWED_AMOUNT || amount < 0) {
        return Data();
    }
    auto data = Data();
    encode64BE(uint64_t(amount), data);
    /// clear first bit to indicate XRP
    data[0] &= 0x7F;
    /// set second bit to indicate positive number
    data[0] |= 0x40;
    return data;
}

// JuBiter-defined
Address Transaction::deserializeAddress(const Data& o) {
    Data a = o;
    a[0] = 0x00;
    Address addr(a);
    return addr;
}

Data Transaction::serializeAddress(Address address) {
    auto data = Data(&address.bytes[0] + 1, &address.bytes[21]);
    return data;
}
