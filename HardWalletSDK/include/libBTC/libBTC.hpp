#pragma once

#ifndef __libBTC__
#define __libBTC__

#include "JUB_SDK_BTC.h"
#include "context/ContextBTC.h"

#include "mSIGNA/stdutils/uchar_vector.h"

namespace jub {

namespace btc {

constexpr JUB_UINT32 version1 = 0x01;
constexpr JUB_UINT32 sequence = 0xffffffff;
constexpr JUB_BYTE   marker	  = 0x00;
constexpr JUB_BYTE   flag     = 0x01;

constexpr JUB_BYTE  p2shVersion = 0x05;
constexpr JUB_BYTE p2pkhVersion = 0x00;

constexpr JUB_BYTE  p2shVersion_BCH = 0x08;

constexpr JUB_BYTE  p2shVersion_LTC = 0x32;
constexpr JUB_BYTE p2pkhVersion_LTC = 0x30;

constexpr JUB_BYTE  p2shVersion_QTUM = 0x32;
constexpr JUB_BYTE p2pkhVersion_QTUM = 0x3a;

constexpr char segwit_hrl[] = "bc";

constexpr char segwit_hrl_QTUM[] = "qc";

JUB_RV buildScriptPubFromAddress(const std::string& address, uchar_vector& scriptPub);

//JUB_RV buildScriptPubFromScript(const uchar_vector& redeemScript, uchar_vector& scriptPub);

JUB_RV buildRedeemScript(const JUB_BTC_TRANS_TYPE& type,
                         const unsigned long m,
                         const unsigned long n,
                         const std::vector<uchar_vector>& vOrderedPublickey,
                         uchar_vector& redeemScript);
JUB_RV buildRedeemScriptTlv(const JUB_BTC_TRANS_TYPE& type,
                            const unsigned long m,
                            const unsigned long n,
                            const std::vector<uchar_vector>& vOrderedPublickey,
                            const long orderedIndex,
                            uchar_vector& redeemScriptTlv);

JUB_RV serializeUnsignedTx(const JUB_BTC_TRANS_TYPE type,
                           const std::vector<INPUT_BTC>& vInputs,
                           const std::vector<OUTPUT_BTC>& vOutputs,
                           const JUB_UINT32 lockTime,
                           uchar_vector& unsignedRaw);

JUB_RV serializePreimage(const JUB_ENUM_COINTYPE_BTC coinType,
                         const JUB_BTC_TRANS_TYPE type,
                         const std::size_t for_sign_input_index,
                         const std::vector<INPUT_BTC>& vInputs,
                         const std::vector<uchar_vector>& vRedeemScript,
                         const std::vector<OUTPUT_BTC>& vOutputs,
                         const JUB_UINT32 lockTime,
                         uchar_vector& preimageRaw,
                         const bool bWitness = false);

JUB_RV serializeTx(const JUB_ENUM_COINTYPE_BTC coinType,
                   const JUB_BTC_TRANS_TYPE type,
                   const unsigned long m,
                   const std::vector<INPUT_BTC>& vInputs,
                   const MAP_COSIGNER_SIGNATURE& mapCosignerSignatures,
                   const std::vector<uchar_vector>& vRedeemScript,
                   const std::vector<OUTPUT_BTC>& vOutputs,
                   const JUB_UINT32 lockTime,
                   uchar_vector& signedIncRaw);

JUB_RV deserializeTx(bool bWitness,
                     const std::string hexTx,
                     std::vector<INPUT_BTC>& vInputs,
                     std::vector<OUTPUT_BTC>& vOutputs,
                     JUB_UINT32_PTR plockTime);
JUB_RV deserializeTx(bool bWitness,
                     const std::vector<uchar_vector>& vRedeemScript,
                     const std::string hexIncTx,
                     MAP_COSIGNER_SIGNATURE& mapCosignerSignature,
                     uchar_vector& unsignedTx);
} // namespace btc end

} // namespace jub end

#endif
