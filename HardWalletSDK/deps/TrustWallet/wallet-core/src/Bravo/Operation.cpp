#include "Operation.h"
#include "Deserialization.h"
#include "mSIGNA/stdutils/uchar_vector.h"

#include <stdexcept>

using namespace TW::Bravo;

using json = nlohmann::json;

/// Transfer Operation
TransferOperation::TransferOperation(const std::string& from, const std::string& to,
                                     const Asset& asset, const std::string& memo)
    : from(from), to(to), memo(memo), asset(asset) {
    validate();
}

TransferOperation::TransferOperation(const std::string& from, const std::string& to, int64_t amount,
                                     bool isTestNet, const std::string& memo)
    : TransferOperation(from, to, Asset(amount, isTestNet), memo) {}

TransferOperation::TransferOperation(const std::string& from, const std::string& to,
                                     const std::string& asset, const std::string& memo)
    : TransferOperation(from, to, Asset::fromString(asset), memo) {}

void TransferOperation::validate() {
    if (from.size() > MaxAccountNameSize) {
        throw std::invalid_argument("\"from\" cannot be greater than " +
                                    std::to_string(MaxAccountNameSize));
    }

    if (to.size() > MaxAccountNameSize) {
        throw std::invalid_argument("\"to\" cannot be greater than " +
                                    std::to_string(MaxAccountNameSize));
    }

    if (memo.size() > MaxMemoSize) {
        throw std::invalid_argument("\"memo\" cannot be greater than " +
                                    std::to_string(MaxMemoSize));
    }

    if (asset.amount <= 0) {
        throw std::invalid_argument("Cannot transfer a negative amount (aka: stealing)");
    }
}

void TransferOperation::serialize(Data& os) const noexcept {
    encodeVarInt32(TransferOperation::OperationId, os);
    encodeString(from, os);
    encodeString(to, os);
    asset.serialize(os);
    encodeString(memo, os);
}

json TransferOperation::serialize() const noexcept {
    json data;
    data["from"] = from;
    data["to"] = to;
    data["amount"] = asset.string();
    data["memo"] = memo;

    return json::array({"transfer", data});
}

// JuBiter-defined
void TransferOperation::deserialize(const Data& o) noexcept {
    uchar_vector vTransfer(o);

    // TransferOperation::OperationId
    Data dataOpId(vTransfer.get_cur_it(), vTransfer.end());
    uint32_t opId = 0;
    int varIntByteSize = 0;
    decodeVarInt32(dataOpId, opId, varIntByteSize);
    if (TransferOperation::OperationId != opId ) {
        return;
    }
    vTransfer.reset_it(varIntByteSize);

    // from
    Data dataFrom(vTransfer.get_cur_it(), vTransfer.end());
    decodeString(dataFrom, from, varIntByteSize);
    vTransfer.reset_it(varIntByteSize);

    // to
    Data dataTo(vTransfer.get_cur_it(), vTransfer.end());
    decodeString(dataTo, to, varIntByteSize);
    vTransfer.reset_it(varIntByteSize);

    //Bravo::Asset
    asset.amount = vTransfer.read_le_uint64();
    asset.symbol = vTransfer.read_le_uint64();

    //memo
    Data dataMemo(vTransfer.get_cur_it(), vTransfer.end());
    decodeString(dataMemo, memo, varIntByteSize);
    vTransfer.reset_it(varIntByteSize);

    if (vTransfer.end() != vTransfer.get_cur_it()) {
        return;
    }
}
