#include "JUB_SDK.h"
#include "libETH/libETH.hpp"

#include "mSIGNA/stdutils/uchar_vector.h"
//#include "Ethereum/Transaction.h"
//#include "Ethereum/RLP.h"
#include <libETH/RLP.h>
#include "TrustWallet/wallet-core/src/Hash.h"
#include <utility/util.h>

namespace jub {
namespace eth {

// TW::Ethereum::RLP has problems with large Numbers(r s)
JUB_RV serializePreimage(const std::vector<JUB_BYTE>& vNonce,
                         const std::vector<JUB_BYTE>& vGasPrice,
                         const std::vector<JUB_BYTE>& vGasLimit,
                         const std::vector<JUB_BYTE>& vTo,
                         const std::vector<JUB_BYTE>& vAmount,
                         const std::vector<JUB_BYTE>& vInput,
                         const std::vector<JUB_BYTE>& vChainID,
                         std::vector<JUB_BYTE>& raw) {

//    TW::uint256_t nonce(vNonce);
//    TW::uint256_t gasPrice(vGasPrice);
//    TW::uint256_t gasLimit(vGasLimit);
//    TW::uint256_t amount(vAmount);
//    TW::uint256_t chainID(vChainID);
//    TW::Ethereum::Transaction tx(nonce, gasPrice, gasLimit, vTo, amount, vInput);
//
//    auto encoded = TW::Data();
//    TW::append(encoded, TW::Ethereum::RLP::encode(tx.nonce));
//    TW::append(encoded, TW::Ethereum::RLP::encode(tx.gasPrice));
//    TW::append(encoded, TW::Ethereum::RLP::encode(tx.gasLimit));
//    TW::append(encoded, TW::Ethereum::RLP::encode(tx.to));
//    TW::append(encoded, TW::Ethereum::RLP::encode(tx.amount));
//    TW::append(encoded, TW::Ethereum::RLP::encode(tx.payload));
//    TW::append(encoded, TW::Ethereum::RLP::encode(chainID));
//    TW::append(encoded, TW::Ethereum::RLP::encode(0));
//    TW::append(encoded, TW::Ethereum::RLP::encode(0));
//    raw = TW::Ethereum::RLP::encodeList(encoded);

    auto encoded = TW::Data();
    TW::append(encoded, jub::eth::RLP::encode(vNonce));
    TW::append(encoded, jub::eth::RLP::encode(vGasPrice));
    TW::append(encoded, jub::eth::RLP::encode(vGasLimit));
    TW::append(encoded, jub::eth::RLP::encode(vTo));
    TW::append(encoded, jub::eth::RLP::encode(vAmount));
    TW::append(encoded, jub::eth::RLP::encode(vInput));
    TW::append(encoded, jub::eth::RLP::encode(vChainID));
    TW::append(encoded, jub::eth::RLP::encode(TW::Data(0)));
    TW::append(encoded, jub::eth::RLP::encode(TW::Data(0)));
    raw = jub::eth::RLP::encodeList(encoded);

    return JUBR_OK;
}

// TW::Ethereum::RLP has problems with large Numbers(r s)
JUB_RV preimageHash(const std::vector<JUB_BYTE>& vNonce,
                    const std::vector<JUB_BYTE>& vGasPrice,
                    const std::vector<JUB_BYTE>& vGasLimit,
                    const std::vector<JUB_BYTE>& vTo,
                    const std::vector<JUB_BYTE>& vAmount,
                    const std::vector<JUB_BYTE>& vInput,
                    const std::vector<JUB_BYTE>& vChainID,
                    std::vector<JUB_BYTE>& hash) {

    std::vector<JUB_BYTE> preimage;
    serializePreimage(vNonce,
                      vGasPrice,
                      vGasLimit,
                      vTo,
                      vAmount,
                      vInput,
                      vChainID,
                      preimage);

    hash = TW::Hash::keccak256(preimage);

    return JUBR_OK;
}

// TW::Ethereum::RLP has problems with large Numbers(r s)
JUB_RV serializeTx(const std::vector<JUB_BYTE>& vNonce,
                   const std::vector<JUB_BYTE>& vGasPrice,
                   const std::vector<JUB_BYTE>& vGasLimit,
                   const std::vector<JUB_BYTE>& vTo,
                   const std::vector<JUB_BYTE>& vAmount,
                   const std::vector<JUB_BYTE>& vInput,
                   const std::vector<JUB_BYTE>& vSignature,
                   std::vector<JUB_BYTE>& raw) {

    if (65 != vSignature.size()) {
        return JUBR_ARGUMENTS_BAD;
    }

//    TW::uint256_t nonce(vNonce);
//    TW::uint256_t gasPrice(vGasPrice);
//    TW::uint256_t gasLimit(vGasLimit);
//    TW::uint256_t amount(vAmount);
//    TW::Ethereum::Transaction tx(nonce, gasPrice, gasLimit, vTo, amount, vInput);
//
//    // parse signature
    std::vector<JUB_BYTE> r(32);
    std::copy(vSignature.begin(), vSignature.begin()+32, r.begin());
//    tx.r = TW::uint256_t(r);

    uchar_vector s(32);
    std::copy(vSignature.begin()+32, vSignature.begin()+32+32, s.begin());
//    tx.s = TW::uint256_t(s);

    uchar_vector v(1);
    std::copy(vSignature.begin()+32+32, vSignature.end(), v.begin());
//    tx.v = TW::uint256_t(v);
//
//    TW::Ethereum::RLP rlp;
//    raw = rlp.encode(tx);

    std::vector<JUB_BYTE> vRaw;
    TW::append(vRaw, jub::eth::RLP::encode(vNonce));
    TW::append(vRaw, jub::eth::RLP::encode(vGasPrice));
    TW::append(vRaw, jub::eth::RLP::encode(vGasLimit));
    TW::append(vRaw, jub::eth::RLP::encode(vTo));
    TW::append(vRaw, jub::eth::RLP::encode(vAmount));
    TW::append(vRaw, jub::eth::RLP::encode(vInput));
    TW::append(vRaw, jub::eth::RLP::encode(v));
    TW::append(vRaw, jub::eth::RLP::encode(r));
    TW::append(vRaw, jub::eth::RLP::encode(s));
    raw = jub::eth::RLP::encodeList(vRaw);

    return JUBR_OK;
}

} // namespace eth end

} // namespace jub end
