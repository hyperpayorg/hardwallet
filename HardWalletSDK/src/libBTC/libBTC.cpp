#include "JUB_SDK_BTC.h"
#include "libBTC/libBTC.hpp"

#include <string>
#include <vector>

#include "airbitz-core/abcd/util/Data.hpp"
#include <TrezorCrypto/base58.h>
#include "bitcoin/bech32.h"
#include "bitcoin/hash.h"
#include "bitcoin/segwit_addr.h"
#include "bitcoinABC/cashaddr.h"
#include "machine/opcode.hpp"
#include "TrezorCrypto/bip32.h"
#include "TrustWallet/wallet-core/src/Hash.h"

namespace jub {
namespace btc {


JUB_RV buildScriptPubFromAddress(const std::string& address, uchar_vector& scriptPub) {

    bool rv = false;
    abcd::DataChunk vScriptPub;

    auto pubkeyDataBit5 = cashaddr::Decode(address);
    if (pubkeyDataBit5.first == "bitcoincash") {
        segwit_addr::convertbits<5, 8, false>(vScriptPub, abcd::DataChunk(pubkeyDataBit5.second.begin(), pubkeyDataBit5.second.end()));
        rv = true;
    }
    else {
        int scriptLen = (int)address.length();
        uint8_t *script = new uint8_t[scriptLen];
        memset(script, 0x00, scriptLen);
        scriptLen = base58_decode_check(address.c_str(), HasherType::HASHER_SHA2D, script, scriptLen);
        if (scriptLen > 0) {
            uchar_vector vScript(script, scriptLen);
            vScriptPub = vScript;
            rv = true;
        }
        delete[] script; script = NULL;
    }

    if (rv) {
        if (   p2shVersion     == vScriptPub[0]
            || p2shVersion_LTC == vScriptPub[0]
            || p2shVersion_BCH == vScriptPub[0]
            || p2shVersion_QTUM== vScriptPub[0]
            ) { // p2sh
            scriptPub << libbitcoin::machine::opcode::hash160;
            scriptPub & uchar_vector(vScriptPub.begin() + 1, vScriptPub.end());
            scriptPub << libbitcoin::machine::opcode::equal;
        }
        else if (   p2pkhVersion     == vScriptPub[0]
                 || p2pkhVersion_LTC == vScriptPub[0]
                 || p2pkhVersion_QTUM== vScriptPub[0]
                 ) { //p2pkh
            scriptPub << libbitcoin::machine::opcode::dup;
            scriptPub << libbitcoin::machine::opcode::hash160;
            scriptPub & uchar_vector(vScriptPub.begin()+1, vScriptPub.end());
            scriptPub << libbitcoin::machine::opcode::equalverify;
            scriptPub << libbitcoin::machine::opcode::checksig;
        }
        else {
            return JUBR_ARGUMENTS_BAD;
        }

        return JUBR_OK;
    }
    else {
        std::pair<std::string, abcd::DataChunk> dec = bech32::decode(address);
        if (   dec.first != std::string(segwit_hrl)
            && dec.first != std::string(segwit_hrl_QTUM)
            ) {
            return JUBR_ARGUMENTS_BAD;
        }
        abcd::DataChunk conv;
        segwit_addr::convertbits<5, 8, false>(conv, abcd::DataChunk(dec.second.begin() + 1, dec.second.end()));

        if (0x14 == conv.size()) { //p2wpkh
            scriptPub << (JUB_BYTE)0x00;
            scriptPub & conv;
        }
        else if (0x20 == conv.size()) { //p2wsh
            scriptPub << (JUB_BYTE)0x00;
            scriptPub & conv;
        }
        else {
            return JUBR_ARGUMENTS_BAD;
        }
    }

    return JUBR_ARGUMENTS_BAD;
}

/*
JUB_RV buildScriptPubFromScript(const uchar_vector& redeemScript, uchar_vector& scriptPub) {

    // scriptPubKey: OP_HASH160 <Hash160(redeemScript)> OP_EQUAL
    scriptPub << libbitcoin::machine::opcode::hash160;
    scriptPub << HexStr2CharPtr((char*)Hash160(redeemScript));
    scriptPub << libbitcoin::machine::opcode::equal;

    return JUBR_OK;
}
*/

JUB_RV buildRedeemScript(const JUB_BTC_TRANS_TYPE& type,
                         const unsigned long m,
                         const unsigned long n,
                         const std::vector<uchar_vector>& vOrderedPublickey,
                         uchar_vector& redeemScript) {

    if (   m  > vOrderedPublickey.size()
        || n != vOrderedPublickey.size()
        ) {
        return JUBR_ARGUMENTS_BAD;
    }

    // redeemScript: <m> <A pubkey> <B pubkey> <C pubkey...> <n> OP_CHECKMULTISIG
    unsigned char op_m = (unsigned char)libbitcoin::machine::opcode::reserved_80 + m;
    redeemScript << op_m;

    for (size_t i=0; i<vOrderedPublickey.size(); ++i) {
        redeemScript << (unsigned char)vOrderedPublickey[i].size();
        redeemScript << vOrderedPublickey[i];
    }

    unsigned char op_n = (unsigned char)libbitcoin::machine::opcode::reserved_80 + n;
    redeemScript << op_n;

    redeemScript << libbitcoin::machine::opcode::checkmultisig;

    return JUBR_OK;
}


JUB_RV buildRedeemScriptTlv(const JUB_BTC_TRANS_TYPE& type,
                            const unsigned long m,
                            const unsigned long n,
                            const std::vector<uchar_vector>& vOrderedPublickey,
                            const long orderedIndex,
                            uchar_vector& redeemScriptTlv) {

    // redeemScriptTlv: orderedIndex <m> <A pubkey> <B pubkey> <C pubkey...> n OP_CHECKMULTISIG
    redeemScriptTlv << (unsigned char)orderedIndex;

    unsigned char op_m = (unsigned char)libbitcoin::machine::opcode::reserved_80 + m;
    redeemScriptTlv << op_m;

    for (size_t i=0; i<vOrderedPublickey.size(); ++i) {
        if (i == orderedIndex) {
            continue;
        }

        redeemScriptTlv << (unsigned char)vOrderedPublickey[i].size();
        redeemScriptTlv << vOrderedPublickey[i];
    }

    unsigned char op_n = (unsigned char)libbitcoin::machine::opcode::reserved_80 + n;
    redeemScriptTlv << op_n;

    redeemScriptTlv << libbitcoin::machine::opcode::checkmultisig;

    return JUBR_OK;
}


JUB_RV serializeUnsignedTx(const JUB_BTC_TRANS_TYPE type,
                           const std::vector<INPUT_BTC>& vInputs,
                           const std::vector<OUTPUT_BTC>& vOutputs,
                           const JUB_UINT32 lockTime,
                           uchar_vector& unsignedRaw) {

    uchar_vector unsignedTrans;
    unsignedTrans << version1;
    if (   p2sh_p2wpkh    == type
        || p2wsh_multisig == type
        ) {
        unsignedTrans << marker;
        unsignedTrans << flag;
    }

    //input
    unsignedTrans << (JUB_BYTE)vInputs.size();
    //[preTXID][preOutputIndex][0x00][nSequence]
    for (auto input : vInputs) {
        switch (input.type) {
        case SCRIPT_BTC_TYPE::P2PKH:
        case SCRIPT_BTC_TYPE::P2SH_MULTISIG:
        {
            //preHash
            uchar_vector preHash = std::string(input.preHash);
            preHash.reverse();
            unsignedTrans << preHash;
            //preIndex
            unsignedTrans << (JUB_UINT32)input.preIndex;
            //scriptSig: <sig> <pubkey>
            //scriptSig: OP_0 <A sig> [B sig] [C sig] ... <redeemScript>
            unsignedTrans << (JUB_BYTE)0x00;
            unsignedTrans << input.nSequence;
            break;
        } // case SCRIPT_BTC_TYPE::P2PKH
          // case SCRIPT_BTC_TYPE::P2SH_MULTISIG end
        default:
            break;
        } // switch (input.type) end
    }
    //output
    unsignedTrans << (JUB_BYTE)vOutputs.size();
    for (auto output : vOutputs) {
        switch (output.type) {
        case SCRIPT_BTC_TYPE::RETURN0:
        {
            unsignedTrans << (uint64_t)output.return0.amount;
            uchar_vector scriptPub;
            scriptPub << (JUB_BYTE)libbitcoin::machine::opcode::return_; //op_return0
            scriptPub << (JUB_BYTE)output.return0.dataLen;
            scriptPub.insert(scriptPub.end(), output.return0.data, output.return0.data + output.return0.dataLen);

            unsignedTrans && scriptPub;
            break;
        } // case SCRIPT_BTC_TYPE::RETURN0 end
        case SCRIPT_BTC_TYPE::QRC20:
        {
            unsignedTrans << (uint64_t)0x00;
            uchar_vector scriptPub(output.qrc20.data,output.qrc20.data + output.qrc20.dataLen);
            unsignedTrans && scriptPub;
            break;
        }
        default:
        {
            //amount
            unsignedTrans << (uint64_t)output.stdOutput.amount;
            //script_pub
            uchar_vector scriptPub;
            if (JUBR_OK != buildScriptPubFromAddress(output.stdOutput.address, scriptPub)) {
                return JUBR_ERROR;
            }

            unsignedTrans && scriptPub;
            break;
        } // default
        } // switch (output.type) end
    }

    //witness 00
    if (   p2sh_p2wpkh    == type
        || p2wsh_multisig == type
        ) {
        unsignedTrans << (JUB_BYTE)0x00;
    }

    unsignedTrans << lockTime;
    unsignedRaw = unsignedTrans;

    return JUBR_OK;
}


JUB_RV serializePreimage(const JUB_ENUM_COINTYPE_BTC coinType,
                         const JUB_BTC_TRANS_TYPE type,
                         const std::size_t for_sign_input_index,
                         const std::vector<INPUT_BTC>& vInputs,
                         const std::vector<uchar_vector>& vRedeemScript,
                         const std::vector<OUTPUT_BTC>& vOutputs,
                         const JUB_UINT32 lockTime,
                         uchar_vector& preimageRaw,
                         const bool bWitness) {

    TW::Hash::Hasher hasher = TW::Hash::sha256d;

    JUB_BTC_TRANS_TYPE txType = type;
    if (COINBCH == coinType) {
        if (p2sh_multisig == type) {
            txType = p2wsh_multisig;
        }
    }

    //preimage: [nVersion][nInputCount][txInputs][nOutputCount][txOutputs][nLockTime][nHashType]
    uchar_vector preimage;
    //[nVersion]
    preimage << version1;

    //[txins]
    uchar_vector vPrevouts;
    uchar_vector vSequences;
    switch (txType) {
    case p2sh_multisig:
    {
    //       : [nInputCount]
    preimage << (JUB_BYTE)vInputs.size();
        break;
    }
    case p2wsh_multisig:
    {
        for (size_t i=0; i<vInputs.size(); ++i) {
            uchar_vector preHash = std::string(vInputs[i].preHash);
            preHash.reverse();

            // prevouts: preHash + preIndex
            vPrevouts << preHash;
            vPrevouts << (JUB_UINT32)vInputs[i].preIndex;

            // sequence
            vSequences << sequence;
        }
        break;
    }
    default:
        break;
    }

    uchar_vector vinputs;
    //                      : [txInputs]
    for (size_t i=0; i<vInputs.size(); ++i) {
        SCRIPT_BTC_TYPE inputType = vInputs[i].type;
        if (COINBCH == coinType) {
            if (SCRIPT_BTC_TYPE::P2SH_MULTISIG == inputType) {
                inputType = SCRIPT_BTC_TYPE::P2WSH_MULTISIG;
            }
        }
        switch (inputType) {
        case SCRIPT_BTC_TYPE::P2SH_MULTISIG:
        {
            //preHash
            uchar_vector preHash = std::string(vInputs[i].preHash);
            preHash.reverse();
            preimage << preHash;

            //preIndex
            preimage << (JUB_UINT32)vInputs[i].preIndex;

            //scriptCode
            if (for_sign_input_index == i) {
                //script code: 0xfd/0xfe/0xff <redeemScript>
                preimage && vRedeemScript[i];
            }
            else {
                preimage << (JUB_BYTE)0x00;
            }

            //sequence
            preimage << sequence;
            break;
        } // case SCRIPT_BTC_TYPE::P2SH_MULTISIG end
        case SCRIPT_BTC_TYPE::P2WSH_MULTISIG:
        {
            if (for_sign_input_index != i) {
                continue;
            }
            //hashPrevouts = sha256d(prevouts)
            preimage << uchar_vector(TW::Hash::hash(hasher, TW::Data(vPrevouts)));

            //hashSequence = sha256d(sequence)
            preimage << uchar_vector(TW::Hash::hash(hasher, TW::Data(vSequences)));

            //outpoint
            uchar_vector preHash = std::string(vInputs[i].preHash);
            preHash.reverse();
            preimage << preHash;

            //preIndex
            preimage << (JUB_UINT32)vInputs[i].preIndex;

            //scriptCode
            //script code: 0xfd/0xfe/0xff <redeemScript>
            preimage && vRedeemScript[i];

            //value
            preimage << (uint64_t)vInputs[i].amount;

            //sequence
            preimage << sequence;
        } // case SCRIPT_BTC_TYPE::P2WSH_MULTISIG end
        default:
            break;
        } // switch (input.type) end
    }

    switch (txType) {
    case p2sh_multisig:
    {
        preimage << vinputs;
        break;
    }
    default:
        break;
    }

    switch (txType) {
    case p2sh_multisig:
    {
    //[txouts]
    //        : [nOutputCount]
    preimage << (JUB_BYTE)vOutputs.size();
        break;
    }
    default:
        break;
    }

    uchar_vector voutputs;
    //                        : [txOutputs]
    for (auto output : vOutputs) {
        switch (output.type) {
        case SCRIPT_BTC_TYPE::RETURN0:
        {
            voutputs << (uint64_t)output.return0.amount;
            uchar_vector scriptPub;
            scriptPub << (JUB_BYTE)libbitcoin::machine::opcode::return_; //op_return0
            scriptPub << (JUB_BYTE)output.return0.dataLen;
            scriptPub.insert(scriptPub.end(), output.return0.data, output.return0.data + output.return0.dataLen);

            voutputs && scriptPub;
            break;
        } // case SCRIPT_BTC_TYPE::RETURN0 end
        default:
        {
            //amount
            voutputs << (uint64_t)output.stdOutput.amount;
            //script_pub
            uchar_vector scriptPub;
            if (JUBR_OK != buildScriptPubFromAddress(output.stdOutput.address, scriptPub)) {
                return JUBR_ERROR;
            }

            voutputs && scriptPub;
            break;
        } // default
        } // switch (output.type) end
    }

    switch (txType) {
    case p2sh_multisig:
    {
        preimage << voutputs;
        break;
    }
    case p2wsh_multisig:
    {
        preimage << uchar_vector(TW::Hash::hash(hasher, TW::Data(voutputs)));
        break;
    }
    default:
        break;
    }

    //[nLockTime]
    preimage << lockTime;

    //[nHashType]
    uint8_t hashType = 0x01;//[ALL]
    if (COINBCH == coinType) {
        hashType |= 0x40;
    }
    preimage << (uint32_t)hashType;

    preimageRaw = preimage;

    return JUBR_OK;
}


JUB_RV serializeTx(const JUB_ENUM_COINTYPE_BTC coinType,
                   const JUB_BTC_TRANS_TYPE type,
                   const unsigned long m,
                   const std::vector<INPUT_BTC>& vInputs,
                   const MAP_COSIGNER_SIGNATURE& mapCosignerSignatures,
                   const std::vector<uchar_vector>& vRedeemScript,
                   const std::vector<OUTPUT_BTC>& vOutputs,
                   const JUB_UINT32 lockTime,
                   uchar_vector& signedIncRaw) {

    JUB_RV rv = JUBR_OK;

    uchar_vector signedIncTrans;
    signedIncTrans << version1;
    if (   p2sh_p2wpkh    == type
        || p2wsh_multisig == type
        ) {
        signedIncTrans << marker;
        signedIncTrans << flag;
    }
    //input
    std::vector<long> vSignatureCnt;
    signedIncTrans << (JUB_BYTE)vInputs.size();
    for (size_t inputIndex=0; inputIndex<vInputs.size(); ++inputIndex) {
        //preHash
        uchar_vector preHash = std::string(vInputs[inputIndex].preHash);
        std::string preID = preHash.getHex();
        preHash.reverse();
        signedIncTrans << preHash;
        //preIndex
        signedIncTrans << (JUB_UINT32)vInputs[inputIndex].preIndex;

        //scriptSig
        long signatureCnt = 0;
        switch (vInputs[inputIndex].type) {
        case SCRIPT_BTC_TYPE::P2PKH:
        {
            //scriptSig: <sig> <pubkey>
//            signedIncTrans << (JUB_BYTE)0;
//            break;
            return JUBR_ARGUMENTS_BAD;
        } // case SCRIPT_BTC_TYPE::P2PKH end
        case SCRIPT_BTC_TYPE::P2SH_MULTISIG:
        case SCRIPT_BTC_TYPE::P2WSH_MULTISIG:
        {
            uchar_vector scriptSig;
            //BTC/LTC/BCH scriptSig: OP_0 <A sig> [B sig] [C sig] ... <redeemScript>
            scriptSig << libbitcoin::machine::opcode::push_size_0; // OP_0

            std::vector<std::string> vSignature = getVectorFromMap(std::to_string(inputIndex), mapCosignerSignatures);
            for (size_t j=0; j<vSignature.size(); ++j) {
                uchar_vector signature;
                signature.setHex(vSignature[j]);
                scriptSig << signature;

                signatureCnt += 1;
            }

            scriptSig & vRedeemScript[inputIndex];

            signedIncTrans && scriptSig;

            vSignatureCnt.push_back(signatureCnt);
            break;
        } // case SCRIPT_BTC_TYPE::P2SH_MULTISIG end
        default:
            break;
        } // switch (vInputs[i].type) end

        // sequence
        signedIncTrans << vInputs[inputIndex].nSequence;
    }
    JUB_VERIFY_RV(rv);

    //output
    signedIncTrans << (JUB_BYTE)vOutputs.size();
    for (auto output : vOutputs) {
        switch (output.type) {
        case SCRIPT_BTC_TYPE::RETURN0:
        {
            signedIncTrans << (uint64_t)output.return0.amount;
            uchar_vector scriptPub;
            scriptPub << (JUB_BYTE)libbitcoin::machine::opcode::return_; //op_return0
            scriptPub << (JUB_BYTE)output.return0.dataLen;
            scriptPub.insert(scriptPub.end(), output.return0.data, output.return0.data + output.return0.dataLen);

            signedIncTrans && scriptPub;
            break;
        } // case SCRIPT_BTC_TYPE::RETURN0 end
        default:
        {
            //amount
            signedIncTrans << (uint64_t)output.stdOutput.amount;
            //script_pub
            uchar_vector scriptPub;
            if (JUBR_OK != buildScriptPubFromAddress(output.stdOutput.address, scriptPub)) {
                return JUBR_ERROR;
            }
            signedIncTrans && scriptPub;
            break;
        } // default
        } // switch (output.type) end
    }

    //witness 00
    if (   p2sh_p2wpkh    == type
        || p2wsh_multisig == type
        ) {
        signedIncTrans << (JUB_BYTE)0x00;
    }

    signedIncTrans << lockTime;
    signedIncRaw = signedIncTrans;

    // Determine whether to complete the transaction signature
    bool isFinished = true;
    for (auto signatureCnt:vSignatureCnt) {
        if (signatureCnt < m) {
            isFinished = false;
        }
    }

    if (!isFinished) {
        return JUBR_MULTISIG_OK;
    }

    return JUBR_OK;
}


// !!Not handling witness yet
JUB_RV deserializeTx(bool bWitness,
                     const std::string hexTx,
                     std::vector<INPUT_BTC>& vInputs,
                     std::vector<OUTPUT_BTC>& vOutputs,
                     JUB_UINT32_PTR plockTime) {

    JUB_RV rv = JUBR_OK;

    if ("" == hexTx) {
        return JUBR_MEMORY_NULL_PTR;
    }

    vInputs.clear();
    vOutputs.clear();
    *plockTime = 0;

    // non-witness: [nVersion]              [nInputCount][txInputs][nOutputCount][txOutputs]         [nLockTime]
    //     witness: [nVersion][marker][flag][nInputCount][txInputs][nOutputCount][txOutputs][witness][nLockTime]
    uchar_vector incTx;
    incTx << HexStr2CharPtr(hexTx);
    incTx.reset_it();

    // [nVersion]
    auto version = incTx.read_le_uint32();
    if (version1 != version) {
        return JUBR_ARGUMENTS_BAD;
    }

    if (bWitness) {
        // [marker]
        auto marker = incTx.read_uint8();
        // [flag]
        auto flag = incTx.read_uint8();
        if (   0 != marker
            || 1 != flag
            ) {
            return JUBR_ARGUMENTS_BAD;
        }
    }

    // [nInputCount]
    auto inputCount = incTx.read_uint8();
    // [txInputs]
    for (auto i=0; i<inputCount; ++i) {
        INPUT_BTC input;
        input.amount = 0;

        // [preHash]
        size_t szPreHashLen = 32;
        uchar_vector preHash = incTx.read_vector(szPreHashLen);
        preHash.reverse();
        input.preHash = new JUB_CHAR[szPreHashLen*2+1];
        memset(input.preHash, 0x00, szPreHashLen*2+1);
        std::memcpy(input.preHash, preHash.getHex().c_str(), szPreHashLen*2);
        input.preIndex = incTx.read_le_uint32();

        // <scriptSig>
        uchar_vector scriptSig;
        incTx >> scriptSig;
        scriptSig.reset_it();
        if (0 == scriptSig.size()) {
            rv = JUBR_ARGUMENTS_BAD;
            break;
        }

        uint8_t scriptSigFirst = scriptSig.read_uint8();
        std::vector<std::string> vInputSignature;
        if ((uint8_t)libbitcoin::machine::opcode::push_size_0 == scriptSigFirst) {
            input.type = SCRIPT_BTC_TYPE::P2SH_MULTISIG;
            if (bWitness) {
                input.type = SCRIPT_BTC_TYPE::P2WSH_MULTISIG;
            }
            // SCRIPT_BTC_TYPE::P2SH_MULTISIG;
            auto scriptSigIt = scriptSig.begin();
            do {
                uint32_t itemLen = scriptSig.read_uint8();
                if ((uint8_t)libbitcoin::machine::opcode::push_one_size == itemLen) {
                    itemLen = scriptSig.read_uint8();
                }
                else if ((uint8_t)libbitcoin::machine::opcode::push_two_size == itemLen) {
                    itemLen = scriptSig.read_le_uint16();
                }
                else if ((uint8_t)libbitcoin::machine::opcode::push_four_size == itemLen) {
                    itemLen = scriptSig.read_le_uint32();
                }
                if (                  0 == itemLen
                    || scriptSig.size()  < itemLen
                    ) {
                    return JUBR_ARGUMENTS_BAD;
                }
                uchar_vector item = scriptSig.read_vector(itemLen);
                uint8_t tag = item[0];
                if (0x30 == tag) {
                    // signature
                }
                else {
                    // redeemScript
                }

                scriptSigIt = scriptSig.get_cur_it();
            } while (scriptSigIt != scriptSig.end());
        }
        else {
            input.type = SCRIPT_BTC_TYPE::P2PKH;
        }

        input.nSequence = incTx.read_le_uint32();

        vInputs.push_back(input);
    }
    if (   JUBR_OK != rv
        || vInputs.size() != inputCount
        ) {
        vInputs.clear();
        return JUBR_ARGUMENTS_BAD;
    }

    // [nOutputCount]
    auto outputCount = incTx.read_uint8();
    // [txOutputs]
    for (auto i=0; i<outputCount; ++i) {
        OUTPUT_BTC output;

        output.stdOutput.amount = incTx.read_le_uint64();;

        // <scriptPubKey>
        uchar_vector scriptPub;
        incTx >> scriptPub;
        scriptPub.reset_it();
        if (0 == scriptPub.size()) {
            rv = JUBR_ARGUMENTS_BAD;
            break;
        }

        uint8_t scriptPubFirst = scriptPub.read_uint8();
        uchar_vector addrScript;
        if ((uint8_t)libbitcoin::machine::opcode::dup == scriptPubFirst) {
            output.type = SCRIPT_BTC_TYPE::P2PKH;

            uint8_t op = scriptPub.read_uint8();
            if ((uint8_t)libbitcoin::machine::opcode::hash160 != op) {
                rv = JUBR_ARGUMENTS_BAD;
                break;
            }
            uint8_t scriptLen = scriptPub.read_uint8();
            uint8_t offset = scriptPub.get_cur_it() - scriptPub.begin();
            uint8_t penultimate = scriptPub[offset+scriptLen];
            uint8_t last = scriptPub[offset+scriptLen+1];
            if (   (uint8_t)libbitcoin::machine::opcode::equalverify != penultimate
                && (uint8_t)libbitcoin::machine::opcode::checksig    != last
                ) {
                rv = JUBR_ARGUMENTS_BAD;
                break;
            }
            addrScript << (unsigned char)jub::btc::p2pkhVersion;
            addrScript << scriptPub.read_vector(scriptLen);
        }
        else if ((uint8_t)libbitcoin::machine::opcode::hash160 == scriptPubFirst) {
            output.type = SCRIPT_BTC_TYPE::P2SH_MULTISIG;

            uint8_t scriptLen = scriptPub.read_uint8();
            uint8_t offset = scriptPub.get_cur_it() - scriptPub.begin();
            uint8_t last = scriptPub[offset+scriptLen];
            if ((uint8_t)libbitcoin::machine::opcode::equal != last) {
                rv = JUBR_ARGUMENTS_BAD;
                break;
            }
            addrScript << (unsigned char)jub::btc::p2shVersion;
            addrScript << scriptPub.read_vector(scriptLen);
        }
        else {
            rv = JUBR_ARGUMENTS_BAD;
            break;
        }

        int scriptLen = (int)addrScript.size();
        uint8_t *script = new uint8_t[scriptLen+1];
        memset(script, 0x00, scriptLen+1);
        std::copy(addrScript.begin(), addrScript.end(), script);

        int strsize = scriptLen*3;
        char *str = new char[strsize];
        strsize = base58_encode_check(script, scriptLen, HasherType::HASHER_SHA2D, str, strsize);
        delete[] script; script = NULL;
        if (strsize <= 0) {
            delete[] str; str = NULL;
            return JUBR_ARGUMENTS_BAD;
        }
        std::string address = std::string(str);
        delete[] str; str = NULL;

        output.stdOutput.address = new JUB_CHAR[address.length()+1];
        memset(output.stdOutput.address, 0x00, address.length()+1);
        memcpy(output.stdOutput.address, address.c_str(), address.length());

        vOutputs.push_back(output);
    }
    if (JUBR_OK != rv) {
        vInputs.clear();
        for (auto& output:vOutputs) {
            if (NULL != output.stdOutput.address) {
                delete[] output.stdOutput.address;
                output.stdOutput.address = NULL;
            }
        }
        vOutputs.clear();
        JUB_VERIFY_RV(rv);
    }

    // [witness]
    if (bWitness) {
        for (auto i=0; i<inputCount; ++i) {
            auto witness = incTx.read_uint8();
            if (0x02 != witness) {
                break;
            }

            // <scriptSig>
            uchar_vector scriptSig;
            incTx >> scriptSig;
            scriptSig.reset_it();
            if (0 == scriptSig.size()) {
                rv = JUBR_ARGUMENTS_BAD;
                break;
            }

            uint8_t scriptSigFirst = scriptSig.read_uint8();
            std::vector<std::string> vInputSignature;
            if ((uint8_t)libbitcoin::machine::opcode::push_size_0 == scriptSigFirst) {
                auto scriptSigIt = scriptSig.begin();
                do {
                    uint32_t itemLen = scriptSig.read_uint8();
                    if ((uint8_t)libbitcoin::machine::opcode::push_one_size == itemLen) {
                        itemLen = scriptSig.read_uint8();
                    }
                    else if ((uint8_t)libbitcoin::machine::opcode::push_two_size == itemLen) {
                        itemLen = scriptSig.read_le_uint16();
                    }
                    else if ((uint8_t)libbitcoin::machine::opcode::push_four_size == itemLen) {
                        itemLen = scriptSig.read_le_uint32();
                    }
                    if (                  0 == itemLen
                        || scriptSig.size()  < itemLen
                        ) {
                        return JUBR_ARGUMENTS_BAD;
                    }
                    uchar_vector item = scriptSig.read_vector(itemLen);
                    uint8_t tag = item[0];
                    if (0x30 == tag) {
                        // signature
                    }
                    else {
                        // redeemScript
                    }

                    scriptSigIt = scriptSig.get_cur_it();
                } while (scriptSigIt != scriptSig.end());
            }
        }
    }

    // [nLockTime]
    *plockTime = incTx.read_le_uint32();

    if (incTx.get_cur_it() != incTx.end()) {
        vInputs.clear();
        for (auto& output:vOutputs) {
            if (NULL != output.stdOutput.address) {
                delete[] output.stdOutput.address;
                output.stdOutput.address = NULL;
            }
        }
        vOutputs.clear();
        *plockTime = 0;
        JUB_VERIFY_RV(JUBR_ARGUMENTS_BAD);
    }

    return JUBR_OK;
}


// !!Not handling witness yet
JUB_RV deserializeTx(bool bWitness,
                     const std::vector<uchar_vector>& vRedeemScript,
                     const std::string hexIncTx,
                     MAP_COSIGNER_SIGNATURE& mapCosignerSignature,
                     uchar_vector& unsignedTx) {

    JUB_RV rv = JUBR_OK;

    if ("" == hexIncTx) {
        return JUBR_MEMORY_NULL_PTR;
    }

    // [nVersion][nInputCount][txInputs][nOutputCount][txOutputs][nLockTime]
    uchar_vector incTx;
    incTx << HexStr2CharPtr(hexIncTx);
    incTx.reset_it();

    // [nVersion]
    auto version = incTx.read_le_uint32();
    if (version1 != version) {
        return JUBR_ARGUMENTS_BAD;
    }
    unsignedTx << version;

    // [nInputCount]
    auto inputCount = incTx.read_uint8();
    unsignedTx << inputCount;
    // [txInputs]
    for (auto inputIndex=0; inputIndex<inputCount; ++inputIndex) {
        uchar_vector preHash = incTx.read_vector(32);
        unsignedTx << preHash;
        preHash.reverse();
        unsignedTx << incTx.read_vector(4);

        // <scriptSig>
        uchar_vector scriptSig;
        incTx >> scriptSig;
        scriptSig.reset_it();
        if (0 == scriptSig.size()) {
            rv = JUBR_ARGUMENTS_BAD;
            break;
        }

        uint8_t scriptSigFirst = scriptSig.read_uint8();
        std::vector<std::string> vInputSignature;
        if ((uint8_t)libbitcoin::machine::opcode::push_size_0 == scriptSigFirst) {
            // SCRIPT_BTC_TYPE::P2SH_MULTISIG;
            auto scriptSigIt = scriptSig.begin();
            do {
                uint32_t itemLen = scriptSig.read_uint8();
                if ((uint8_t)libbitcoin::machine::opcode::push_one_size == itemLen) {
                    itemLen = scriptSig.read_uint8();
                }
                else if ((uint8_t)libbitcoin::machine::opcode::push_two_size == itemLen) {
                    itemLen = scriptSig.read_le_uint16();
                }
                else if ((uint8_t)libbitcoin::machine::opcode::push_four_size == itemLen) {
                    itemLen = scriptSig.read_le_uint32();
                }
                if (                  0 == itemLen
                    || scriptSig.size()  < itemLen
                    ) {
                    rv = JUBR_ARGUMENTS_BAD;
                    break;
                }
                uchar_vector item = scriptSig.read_vector(itemLen);
                uint8_t tag = item[0];
                if (0x30 == tag) {
                    // signature
                    uchar_vector signatureRaw;
                    signatureRaw & item;
                    vInputSignature.push_back(signatureRaw.getHex());
                }
                else {
                    // redeemScript check
                    if (item != vRedeemScript[inputIndex]) {
                        rv = JUBR_ARGUMENTS_BAD;
                        break;
                    }
                }

                scriptSigIt = scriptSig.get_cur_it();
            } while (scriptSigIt != scriptSig.end());
        }
        else {
            unsignedTx << scriptSig;
        }

        mapCosignerSignature.insert({std::to_string(inputIndex), vInputSignature});
        unsignedTx << (JUB_BYTE)0x00;
        unsignedTx << incTx.read_vector(4);
    }
    JUB_VERIFY_RV(rv);

    // [nOutputCount]
    auto outputCount = incTx.read_uint8();
    unsignedTx << outputCount;
    // [txOutputs]
    for (auto i=0; i<outputCount; ++i) {
        unsignedTx << incTx.read_vector(8);

        uchar_vector scriptPub;
        incTx >> scriptPub;
        unsignedTx && scriptPub;
    }

    // [nLockTime]
    unsignedTx << incTx.read_vector(4);

    if (incTx.get_cur_it() != incTx.end()) {
        unsignedTx.clear();
        return JUBR_ARGUMENTS_BAD;
    }

    return JUBR_OK;
}


} // namespace btc end
} // namespace jub end
