//
//  test_trezor_crypto.mm
//  JubSDKCore
//
//  Created by Pan Min on 2019/8/21.
//  Copyright © 2019 JuBiter. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "JubSDKCore.h"
#import "JubSDKCore+DEV_HID.h"
#import "JubSDKCore+COIN_BTC.h"

#include <iostream>     // std::cout
#include <sstream>      // std::istringstream
#include <string>       // std::string
#include <vector>
#include <map>
#include "mSIGNA/stdutils/uchar_vector.h"
#include "HDKey/HDKey.hpp"
#include "libbitcoin/machine/opcode.hpp"
#include "bitcoin/hash.h"
//#include "bitcoin/bech32.h"
#include "bitcoin/segwit_addr.h"

//#include <TrezorCrypto/address.h>
//#include <TrezorCrypto/aes.h>
//#include <TrezorCrypto/base32.h>
#include <TrezorCrypto/base58.h>
//#include <TrezorCrypto/ripple/base58.h>
//#include <TrezorCrypto/bignum.h>
//#include <TrezorCrypto/bip32.h>
#include <TrezorCrypto/bip39.h>
//#include <TrezorCrypto/blake2b.h>
//#include <TrezorCrypto/blake256.h>
//#include <TrezorCrypto/cash_addr.h>
#include <TrezorCrypto/curves.h>
//#include <TrezorCrypto/ed25519.h>
//#include <TrezorCrypto/hasher.h>
//#include <TrezorCrypto/hmac.h>
//#include <TrezorCrypto/memzero.h>
//#include <TrezorCrypto/monero/monero.h>
//#include <TrezorCrypto/nem.h>
//#include <TrezorCrypto/nist256p1.h>
//#include <TrezorCrypto/pbkdf2.h>
//#include <TrezorCrypto/rand.h>
//#include <TrezorCrypto/ripemd160.h>
#include <TrezorCrypto/secp256k1.h>
//#include <TrezorCrypto/sha2.h>
//#include <TrezorCrypto/sha3.h>
#include "TrezorCrypto/ecdsa.h"

using namespace std;

//JubSDKCore* g_sdk;

void address_code_test() {
    //LTC: pqeqx8dedtzpr4umchvhnmnnxv5rpqmf6v7at9sjmp
    //legacy: 1JxkR2xGeroewqiWC7shuXAsSrAuFoPonJ
    //        32EbjhDXXF7pVzVExRaN3ZGXbR1UQ9YZmr
    std::string address =     "pqeqx8dedtzpr4umchvhnmnnxv5rpqmf6v7at9sjmp";//"7jWbzFMvmNPZN6aNK54X3pKbRnBhx7XvtD";//"M8Sk3adVUMyFJVm94JZhsCWvv7bvRyHXri";//"Xrxf9nCM8AWPs2CYhkYpep1d7ZU4Yb1PAA";
    
    int datalen = (int)address.length();
    uint8_t* data = new uint8_t[datalen];
    memset(data, 0x00, datalen);
    strcpy((char*)data, address.c_str());

    char str[50] = {0,};
    int strsize = sizeof(str)/sizeof(uint8_t);
    if (0 >= base58_decode_check(address.c_str(), HasherType::HASHER_SHA2D, (uint8_t*)str, strsize)) {
//    if (0 >= base58_encode_check(data, datalen, HasherType::HASHER_SHA2D, str, strsize)) {
        delete[] data; data = NULL;
        return;
    }
    delete[] data; data = NULL;

    uchar_vector vchRet(str);
    vchRet.reset_it();
    size_t vchLen = vchRet.size();
    std::cout << "[" << vchLen << "]" << vchRet.getHex() << std::endl;
    uint8_t mainnet = vchRet.read_uint8();
    std::cout << "minnet[" << to_string(mainnet) << "]" << std::endl;
    uchar_vector hash = vchRet.read_vector(vchLen-1);
    std::cout << "[" << hash.size() << "]" << hash.getHex() << std::endl;
    
    //qtum: QeZjXLEyqKgUNyoXcTCC2nxVx77cSnmpcy
    //      M8Sk3adVUMyFJVm94JZhsCWvv7bvRyHXri
    uchar_vector scriptAddress;
    scriptAddress << (unsigned char)0x05;//mainnet;//(unsigned char)0x3a;
    scriptAddress << hash;
    datalen = (int)scriptAddress.size();
    data = new uint8_t[datalen];
    memset(data, 0x00, datalen);
    std::copy(scriptAddress.begin(), scriptAddress.end(), data);
    strsize = sizeof(str)/sizeof(uint8_t);
    memset(str, 0x00, strsize);
    if (0 >= base58_encode_check(data, datalen, HasherType::HASHER_SHA2D, str, strsize)) {
        delete[] data; data = NULL;
        return;
    }
    std::string addr = std::string(str);
    std::cout << "Qtum base58 address: " << addr << std::endl;
    delete[] data; data = NULL;
    /*
    //qtum witness: qc1qc5z5rwp0fndz7cwthsset9w8nl7mfuh656anjn
    //              qc1qqhu3a4xhkz67zl0y9dxhkz8fxs2ywfj3aj0hq3
    int witver = 0x00;
    std::string bechAddr = segwit_addr::encode("qc", witver, hash);
    std::cout << "Qtum bech32 address: " << bechAddr << std::endl;*/
}

#define VERSION_PUBLIC  0x0488b21e
#define VERSION_PRIVATE 0x0488ade4

void hdFromXpub() {
    int rv = 0;
    
    // m/45'/0/0/0: xpub    std::__1::string &    "xpub6Ee2MvBN8nyti9PnL8D2jTke63MKSDK6ykvVedt3Ztjp3Z69uU3teSfHx2Ee1jvSpAkapnDX5FkKLQBoQYMx4CMnBiwwDWcTUAVu8DeHKcN"    0x00007ffeefbfec78
    HDNode hdkey;
    uint32_t parentFingerprint;
    string deriveRootXpub = "xpub69fZU73sYR4gxkmD7nLTgAWtNyCzJ52yJUD93JjQmzUouvofiTrPtu36keJJaLUSxZRYdiFGG64hFapgUVBvVyNeN6bY1GohX3tPV3gPYGE";
    rv = hdnode_deserialize(deriveRootXpub.c_str(),
                            VERSION_PUBLIC, VERSION_PRIVATE,
                            SECP256K1_NAME,
                            &hdkey, &parentFingerprint);
    uchar_vector publickey;
    for (int i=0; i<sizeof(hdkey.public_key)/sizeof(uint8_t); ++i) {
        publickey.push_back(hdkey.public_key[i]);
    }
    NSLog(@"public_key: %s\n", publickey.getHex().c_str());
    uchar_vector chaincode;
    for (int i=0; i<sizeof(hdkey.chain_code)/sizeof(uint8_t); ++i) {
        chaincode.push_back(hdkey.chain_code[i]);
    }
    parentFingerprint = hdnode_fingerprint(&hdkey);
    NSLog(@"chain_code: %s\n", chaincode.getHex().c_str());
    NSLog(@"fingerprint: 0x%xl\n", parentFingerprint);
    
    char xpub[MAX_ADDR_SIZE] = {0,};
    // m/45'
    //xpub    std::__1::string &   "xpub68PwQJExtoyySq2uW3dUm9rptFdvuRanXnqx6ukXoGpQU9bpyoZyYK5uDbMr3kpWpXgT7Vmp9mE822YQYenk7HXn2zLEzaZnSADRVwtpdGA"
    rv = hdnode_serialize_public(&hdkey, parentFingerprint, VERSION_PUBLIC, xpub, MAX_ADDR_SIZE);
    publickey.clear();
    for (int i=0; i<sizeof(hdkey.public_key)/sizeof(uint8_t); ++i) {
        publickey.push_back(hdkey.public_key[i]);
    }
    NSLog(@"public_key: %s\n", publickey.getHex().c_str());
    chaincode.clear();
    for (int i=0; i<sizeof(hdkey.chain_code)/sizeof(uint8_t); ++i) {
        chaincode.push_back(hdkey.chain_code[i]);
    }
    parentFingerprint = hdnode_fingerprint(&hdkey);
    NSLog(@"chain_code: %s\n", chaincode.getHex().c_str());
    NSLog(@"fingerprint: 0x%xl\n", parentFingerprint);
    NSLog(@"[%i] hdnode_serialize_public return: %s\n", rv, xpub);
    
    // m/45'/0
    //xpub    std::__1::string &    "xpub6ADSVAkxaUyVJq1hLnfVX1zePWQtdWDzkK6QSj4raaLBEeRJzJZRoquq85z9RmCkPm3JefcwNWYujw8391Y3QeZcsFiG2ZQv75ycaBgvwoP"
//    rv = hdnode_private_ckd(&hdkey, 0);
//    hdnode_fill_public_key(&hdkey);
    rv = hdnode_public_ckd(&hdkey, 0);
    memset(xpub, 0x00, MAX_ADDR_SIZE);
    rv = hdnode_serialize_public(&hdkey, parentFingerprint, VERSION_PUBLIC, xpub, MAX_ADDR_SIZE);
    publickey.clear();
    for (int i=0; i<sizeof(hdkey.public_key)/sizeof(uint8_t); ++i) {
        publickey.push_back(hdkey.public_key[i]);
    }
    NSLog(@"public_key: %s\n", publickey.getHex().c_str());
    chaincode.clear();
    for (int i=0; i<sizeof(hdkey.chain_code)/sizeof(uint8_t); ++i) {
        chaincode.push_back(hdkey.chain_code[i]);
    }
    parentFingerprint = hdnode_fingerprint(&hdkey);
    NSLog(@"chain_code: %s\n", chaincode.getHex().c_str());
    NSLog(@"fingerprint: 0x%xl\n", parentFingerprint);
    NSLog(@"[%i] hdnode_serialize_public return: %s\n", rv, xpub);
    
    // m/45'/0/0
    //xpub    std::__1::string &    "xpub6Bzxt2qXAf8KE69LWr87TSD62bh8oYNGFb7hKcGd2o4WQNp1K7nohihM5aKsmk8iwrV3dhMvqaBZx6aR945NUkhZSzrcNctmSJLLAfySBkz"
//    rv = hdnode_private_ckd(&hdkey, 0);
//    hdnode_fill_public_key(&hdkey);
    rv = hdnode_public_ckd(&hdkey, 0);
    memset(xpub, 0x00, MAX_ADDR_SIZE);
    rv = hdnode_serialize_public(&hdkey, parentFingerprint, VERSION_PUBLIC, xpub, MAX_ADDR_SIZE);
    publickey.clear();
    for (int i=0; i<sizeof(hdkey.public_key)/sizeof(uint8_t); ++i) {
        publickey.push_back(hdkey.public_key[i]);
    }
    NSLog(@"public_key: %s\n", publickey.getHex().c_str());
    chaincode.clear();
    for (int i=0; i<sizeof(hdkey.chain_code)/sizeof(uint8_t); ++i) {
        chaincode.push_back(hdkey.chain_code[i]);
    }
    parentFingerprint = hdnode_fingerprint(&hdkey);
    NSLog(@"chain_code: %s\n", chaincode.getHex().c_str());
    NSLog(@"fingerprint: 0x%xl\n", parentFingerprint);
    NSLog(@"[%i] hdnode_serialize_public return: %s\n", rv, xpub);
    
    // m/45'/0/0/0
    //xpub    std::__1::string &    "xpub6Ee2MvBN8nyti9PnL8D2jTke63MKSDK6ykvVedt3Ztjp3Z69uU3teSfHx2Ee1jvSpAkapnDX5FkKLQBoQYMx4CMnBiwwDWcTUAVu8DeHKcN"
//    rv = hdnode_private_ckd(&hdkey, 0);
//    hdnode_fill_public_key(&hdkey);
    rv = hdnode_public_ckd(&hdkey, 0);
    memset(xpub, 0x00, MAX_ADDR_SIZE);
    rv = hdnode_serialize_public(&hdkey, parentFingerprint, VERSION_PUBLIC, xpub, MAX_ADDR_SIZE);
    publickey.clear();
    for (int i=0; i<sizeof(hdkey.public_key)/sizeof(uint8_t); ++i) {
        publickey.push_back(hdkey.public_key[i]);
    }
    NSLog(@"public_key: %s\n", publickey.getHex().c_str());
    chaincode.clear();
    for (int i=0; i<sizeof(hdkey.chain_code)/sizeof(uint8_t); ++i) {
        chaincode.push_back(hdkey.chain_code[i]);
    }
    parentFingerprint = hdnode_fingerprint(&hdkey);
    NSLog(@"chain_code: %s\n", chaincode.getHex().c_str());
    NSLog(@"fingerprint: 0x%xl\n", parentFingerprint);
    NSLog(@"[%i] hdnode_serialize_public return: %s\n", rv, xpub);
}

void redeemScript() {
    unsigned long rv = 0;
    
//    std::vector<std::string> vCosignerMainXpub = {
//    "xpub68pgp4T4h7MTbghzLviPnT5WdLtpduBDrbt9PMa3ueGEWLqjrHh3XnBZJZ46EP8LPFkrebyV6AR2EvAU9YPBFmccpVvhgTugKn4AcoiNTAK",
//    "xpub68W63r3gBL5gZFKwA4Q2fHwweiPAP7UDaa7Sgy8uBF4Q6AN9gD97Y95ci4hVwB76CBJx19C4uheKn3ndmPSaZEfAtsFF6s6hcbUfQVvnbha"
//    };
    std::vector<std::string> vCosignerMainXpub = {
    "xpub68y1awDNteR6Etb6cPN9PE8VUYZEmL9G8JCUpu7vyQTKzQL27dNCcVc2H3Y6Qs9U6nKHtfxhwYrNpvJ75aVNFuWRaSm5xXoin5Yk1yVR5X3",
    "xpub6Cor86MbqK8Hmnt6W4rin5QiHWyLT7iWNTRLnnYYviRHP6eJU3eeR7Afbmp1kUw2MiAfsKfJwNFpUvSk4f5L42AeuxX7w3PFxskn1yF3ozR",
    "xpub68pgp4T4h7MTbghzLviPnT5WdLtpduBDrbt9PMa3ueGEWLqjrHh3XnBZJZ46EP8LPFkrebyV6AR2EvAU9YPBFmccpVvhgTugKn4AcoiNTAK"
    };
//    std::vector<std::string> vCosignerMainXpub = {
//    "xpub69fZU73sYR4gxkmD7nLTgAWtNyCzJ52yJUD93JjQmzUouvofiTrPtu36keJJaLUSxZRYdiFGG64hFapgUVBvVyNeN6bY1GohX3tPV3gPYGE",
//    "xpub6913UHqdFEA64vWd24kaUJqQzYbTSGi2hE3iaWTSDbPXCPnmYLqndQQimUWdFwMLabrdCRguKpxHYV7f34ZVjTfWnpdW9MGkKZMxRmvxAPN",
//    "xpub685ZS5kT4EoUhFPnMyPNd5j17YVR1UNFKznkwrPrRVctuSdDZc4SmdW2MMfQV8cSks4zwmnb4N6QAoLTK9KfiGZbhER1FdbSX8dH3mPHpvC"
//    };
//    std::vector<std::string> vCosignerMainXpub = {
//    "xpub695tGgJixYUEdYWyh623JB3vyzQHQnvy8FUBCe5Fccz86TUPbatyokpxQYNbG4UNyyxAN5LMejHmZbxSUteZYYToXncmP4Dxv6yfsqK1jo9",
//    "xpub69N1VfUArAzHNWPaDm46nF3zR3J8hZUGBTMy7LTVR2MsQTpnFw6iZdNea6tP1MmPmgXmFdjCcFZ2rW6GgyxMdn3brAHpw7eBLyPJfbhgkyb"
//    };
    
    uint8_t m = 2;
    uint8_t n = vCosignerMainXpub.size();
    
    std::vector<std::string> vCosignerMainPubKey;
    //hex main key, xpub
    std::map<std::string, std::string> mapCosignerMainKey;
    for (const auto& cosignerMainXpub:vCosignerMainXpub) {
        HDNode hdkey;
        uint32_t fingerprint = 0;
        rv = hdnode_deserialize(cosignerMainXpub.c_str(),
                                VERSION_PUBLIC, VERSION_PRIVATE,
                                SECP256K1_NAME,
                                &hdkey, &fingerprint);
        NSLog(@"[%lu] hdnode_deserialize(...): \n", rv);
        uchar_vector mainPublickey(hdkey.public_key, sizeof(hdkey.public_key)/sizeof(uint8_t));
        std::string hexMainPubKey = mainPublickey.getHex();
        vCosignerMainPubKey.push_back(hexMainPubKey);
        mapCosignerMainKey.insert({hexMainPubKey, cosignerMainXpub});
    }
    
    //sort
    std::sort(vCosignerMainPubKey.begin(), vCosignerMainPubKey.end());
    
    std::string basePath = "m/45'";
    long change = 0;
    long addressIndex = 0;
    std::vector<std::string> vCosignerPubKeys;
    // derive cosigner
    for (const auto& cosignerMainPubKey:vCosignerMainPubKey) {
        // m/45'/cosignerIndex/change/index
        long cosignerIndex = std::find(vCosignerMainPubKey.begin(), vCosignerMainPubKey.end(), cosignerMainPubKey) - vCosignerMainPubKey.begin();
        std::string path1 = basePath + "/" + std::to_string(cosignerIndex)
        + "/" + std::to_string(change)
        + "/" + std::to_string(addressIndex);
        
        auto it = mapCosignerMainKey.find(cosignerMainPubKey);
        if (mapCosignerMainKey.end() == it) {
            break;
        }
        
        HDNode hdkey;
        uint32_t fingerprint = 0;
        rv = jub::hdnode_pub_ckd(it->second, path1, SECP256K1_NAME, bitcoinXPUB, bitcoinXPRV, &hdkey, &fingerprint);
        NSLog(@"[%lu] jub::hdnode_pub_ckd(...): %s\n", rv, path1.c_str());
        char cosignerXpub[130] = {0,};
        int cosignerXpubSize = sizeof(cosignerXpub)/sizeof(char);
        rv = hdnode_serialize_public(&hdkey, fingerprint, VERSION_PUBLIC, cosignerXpub, cosignerXpubSize);
        NSLog(@"[%lu] hdnode_serialize_public(...): %s\n", rv, cosignerXpub);
        curve_point pub;
        if (1 != ecdsa_read_pubkey(&secp256k1, hdkey.public_key, &pub)) {
            return ;
        }
        uint8_t compressPubKey[33] = {0,};
        compress_coords(&pub, compressPubKey);
        uchar_vector compressed(compressPubKey, sizeof(compressPubKey)/sizeof(uint8_t));
        vCosignerPubKeys.push_back(compressed.getHex());
        NSLog(@"[%lu] compress_coords(...): %s\n", rv, compressed.getHex().c_str());
    }
    
    // order
    std::sort(vCosignerPubKeys.begin(), vCosignerPubKeys.end());
    
    //redeemScript
    uchar_vector redeemScript;
    redeemScript << (uint8_t)((uint8_t)libbitcoin::machine::opcode::reserved_80+m);
    for (const auto& cosignerPubKeys:vCosignerPubKeys) {
        uchar_vector cosignerPubKey(cosignerPubKeys);
        redeemScript << (uint8_t)cosignerPubKey.size();
        redeemScript << cosignerPubKey;
    }
    redeemScript << (uint8_t)((uint8_t)libbitcoin::machine::opcode::reserved_80+n);
    redeemScript << libbitcoin::machine::opcode::checkmultisig;
    
    std::cout << "redeemScript: " << redeemScript.getHex() << std::endl;
    
    ///
//    std::string hexRedeemScript = "5221025638ad8a262f9de463880adddf2e7d045409ee2194427567f29bc9c52384ea012102d8531fe1ebd5b5211dffdb53c82b90cfcf4510bc8e7b0305f71319ce2fb9f6362103f4b3e17fe0489767d671b03ca7802df365f8d4eb47693ba767426cfd20b44b7c53ae";
//    uchar_vector redeemScript(hexRedeemScript);
    
    std::vector<unsigned char> hRedeemScript;
    size_t hRedeemScriptSz = Hash160(redeemScript, hRedeemScript);
    if (0 == hRedeemScriptSz) {
        return ;
    }
    uchar_vector hash(hRedeemScript);
    NSLog(@"[%lu] Hash160(...): %s\n", hRedeemScriptSz, hash.getHex().c_str());
    
    uchar_vector script;
    script << (unsigned char)0x05;//0x00;//0x10;
    script << hRedeemScript;
    //std::string address = base58::EncodeBase58Check(script);
    int scpLen = (int)script.size();
    uint8_t *scp = new uint8_t[scpLen+1];
    memset(scp, 0x00, scpLen+1);
    std::copy(script.begin(), script.end(), scp);

    int strsize = scpLen * 3;
    char* str = new char[strsize];
    memset(str, 0x00, strsize);
    strsize = base58_encode_check(scp, scpLen, HasherType::HASHER_SHA2D, str, strsize);
    delete[] scp; scp = NULL;
    std::string address = std::string(str);

    NSLog(@"[%lu] base58::EncodeBase58Check(...): %s\n", rv, address.c_str());
}
