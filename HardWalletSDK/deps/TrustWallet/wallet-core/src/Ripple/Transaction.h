// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "Address.h"
#include "Memo.h"
#include "../Data.h"
//#include "../proto/Ripple.pb.h"

namespace TW::Ripple {

extern const int NETWORK_PREFIX;
// JuBiter-added
extern const int NETWORK_PREFIX_MULTI_SIGN;

//https://xrpl.org/serialization.html -> Type List
enum class FieldType: int {
    int16   = 1,
    int32   = 2,
    amount  = 6,
    vl      = 7,
    account = 8,
    stobject = 14,
    starray = 15
};

enum class TransactionType { payment = 0 };

class Transaction {
    /// We only support transaction types other than the Payment transaction.
    /// Non-XRP currencies are not supported. Float and negative amounts are not supported.
    /// See https://github.com/trezor/trezor-core/tree/master/src/apps/ripple#transactions
public:
    int64_t amount;
    int64_t fee;
    int64_t flags;
    int32_t sequence;
    int32_t last_ledger_sequence;
    Address account;
    Address destination;
    int64_t destination_tag;
    Memo memo;
    Data pub_key;
    Data signature;

    // JuBiter-defined
    Transaction() {};
    Transaction(int64_t amount, int64_t fee,
                int64_t flags,
                int32_t sequence, int32_t last_ledger_sequence,
                Address account, Address destination, int64_t destination_tag,
                Memo memo = {})
        : amount(amount)
        , fee(fee)
        , flags(flags)
        , sequence(sequence)
        , last_ledger_sequence(last_ledger_sequence)
        , account(account)
        , destination(destination)
        , destination_tag(destination_tag)
        , memo(memo) {}

public:
    // JuBiter-defined
    void deserialize(const Data& o);
    // JuBiter-defined
    virtual size_t size();
    /// simplified serialization format tailored for Payment transaction type
    /// exclusively.
    Data serialize();
    // JuBiter-defined
    void setPreImage(const Data& o);
    // JuBiter-defined
    virtual size_t sizePreImage();
    Data getPreImage();

    // JuBiter-defined
    int64_t deserializeAmount(const Data& o);
    static Data serializeAmount(int64_t amount);
    // JuBiter-defined
    Address deserializeAddress(const Data& o);
    static Data serializeAddress(Address address);

    // JuBiter-defined
    virtual void setAmountIndex(const uint16_t amtIndex) {
        amountIndex = amtIndex;
    }
    // JuBiter-defined
    virtual uint16_t getAmountIndex() const {
        return amountIndex;
    }
    // JuBiter-defined
    virtual void setFeeIndex(const uint16_t fIndex) {
        feeIndex = fIndex;
    }
    // JuBiter-defined
    virtual uint16_t getFeeIndex() const {
        return feeIndex;
    }
    // JuBiter-defined
    virtual void setSignPubkeyIndex(const uint16_t signPkIndex) {
        signPubkeyIndex = signPkIndex;
    }
    // JuBiter-defined
    virtual uint16_t getSignPubkeyIndex() const {
        return signPubkeyIndex;
    }
    // JuBiter-defined
    virtual void setDestinationIndex(const uint16_t destIndex) {
        destinationIndex = destIndex;
    }
    // JuBiter-defined
    virtual void setDestinationTagIndex(const uint16_t destTagIndex) {
        destinationTagIndex = destTagIndex;
    }
    // JuBiter-defined
    virtual void setShowMemoIndex(const uint16_t memoIndex) {
        showMemoIndex = memoIndex;
    }
    // JuBiter-defined
    virtual uint16_t getDestinationIndex() const {
        return destinationIndex;
    }
    // JuBiter-defined
    virtual uint16_t getDestinationTagIndex() const {
        return destinationTagIndex;
    }
    // JuBiter-defined
    virtual uint16_t getShowMemoIndex() const {
        return showMemoIndex;
    }
    // JuBiter-defined
    virtual size_t getShowMemoSize() const {
        return memo.memoData.size();
    }
    // JuBiter-defined
    virtual Data getNetworkPrefix();

private:
    uint16_t amountIndex = 0;
    uint16_t feeIndex = 0;
    uint16_t signPubkeyIndex = 0;
    uint16_t destinationIndex = 0;
    uint16_t destinationTagIndex = 0;
    uint16_t showMemoIndex = 0;
};

} // namespace TW::Ripple
