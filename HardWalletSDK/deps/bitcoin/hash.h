// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_HASH_H
#define BITCOIN_HASH_H

#include <vector>

//#include <openssl/ripemd.h>
//#include <openssl/sha.h>
#include "TrezorCrypto/ripemd160.h"
#include "TrezorCrypto/sha2.h"

#define SIZE_MD5		16
#define SIZE_RIPEMD160	20
#define SIZE_SHA160		SIZE_RIPEMD160
#define SIZE_SHA256		32

template<typename T1>
inline unsigned char* Hash(const T1 pbegin, const T1 pend)
{
	static unsigned char pblank[1] = { 0, };
	unsigned char hash1[SIZE_SHA256];
	size_t hashlen = sizeof(hash1) / sizeof(unsigned char);
	memset(hash1, 0, hashlen);

//	SHA256((pbegin == pend ? pblank : (unsigned char*)&pbegin[0]), (pend - pbegin) * sizeof(pbegin[0]), hash1);
	SHA256_CTX ctx;
	memset(&ctx, 0, sizeof(ctx) / sizeof(SHA256_CTX));
	sha256_Init(&ctx);
	if (pbegin == pend) {
		sha256_Update(&ctx, pblank, (pend - pbegin) * sizeof(pbegin[0]));
	}
	else {
		sha256_Update(&ctx, (unsigned char*)&pbegin[0], (pend - pbegin) * sizeof(pbegin[0]));
	}
	sha256_Final(&ctx, hash1);

	unsigned char *hash2 = new unsigned char[hashlen];
	memset(hash2, 0, hashlen);
//    SHA256(hash1, sizeof(hash1), hash2);
	memset(&ctx, 0, sizeof(ctx) / sizeof(SHA256_CTX));
	sha256_Init(&ctx);
	sha256_Update(&ctx, hash1, hashlen);
	sha256_Final(&ctx, hash2);

    return hash2;
}

template<typename T1>
inline unsigned char* Hash160(const T1 pbegin, const T1 pend)
{
	static unsigned char pblank[1] = { 0, };

	// SHA256
	unsigned char hash1[SIZE_SHA256];
	size_t hashlen1 = sizeof(hash1)/sizeof(unsigned char);
	memset(hash1, 0, hashlen1);

//    SHA256((pbegin == pend ? pblank : (unsigned char*)&pbegin[0]), (pend - pbegin) * sizeof(pbegin[0]), hash1);
	SHA256_CTX sha256Ctx;
	memset(&sha256Ctx, 0, sizeof(sha256Ctx) / sizeof(SHA256_CTX));
	sha256_Init(&sha256Ctx);
	if (pbegin == pend) {
		sha256_Update(&sha256Ctx, pblank, (pend - pbegin) * sizeof(pbegin[0]));
	}
	else {
		sha256_Update(&sha256Ctx, (unsigned char*)&pbegin[0], (pend - pbegin) * sizeof(pbegin[0]));
	}
	sha256_Final(&sha256Ctx, hash1);

	// RIPEMD160
    unsigned char *hash2 = new unsigned char[SIZE_RIPEMD160+1];
	memset(hash2, 0, SIZE_RIPEMD160+1);

//    RIPEMD160(hash1, sizeof(hash1), hash2);
	RIPEMD160_CTX ripemdCtx;
	ripemd160_Init(&ripemdCtx);
	ripemd160_Update(&ripemdCtx, hash1, hashlen1);
	ripemd160_Final(&ripemdCtx, hash2);

    return hash2;
}

template<typename T1>
inline unsigned char* Hash256(const T1 pbegin, const T1 pend)
{
	static unsigned char pblank[1] = { 0, };

	// SHA256
	size_t hashlen = SIZE_SHA256;
	unsigned char *hash = new unsigned char[hashlen+1];
	memset(hash, 0, hashlen+1);

	SHA256_CTX sha256Ctx;
	memset(&sha256Ctx, 0, sizeof(sha256Ctx) / sizeof(SHA256_CTX));
	sha256_Init(&sha256Ctx);
	if (pbegin == pend) {
		sha256_Update(&sha256Ctx, pblank, (pend - pbegin) * sizeof(pbegin[0]));
	}
	else {
		sha256_Update(&sha256Ctx, (unsigned char*)&pbegin[0], (pend - pbegin) * sizeof(pbegin[0]));
	}
	sha256_Final(&sha256Ctx, hash);

	return hash;
}

//template<typename T1>
//inline unsigned char* MD5(const T1 pbegin, const T1 pend)
//{
//	static unsigned char pblank[1] = { 0, };
//
//	// MD5
//	size_t hashlen = SIZE_MD5;
//	unsigned char *hash = new unsigned char[hashlen+1];
//	memset(hash, 0, hashlen+1);
//
//	md5_context md5Ctx;
//	memset(&md5Ctx, 0, sizeof(md5Ctx) / sizeof(md5_context));
//	md5_starts(&md5Ctx);
//	if (pbegin == pend) {
//		md5_update(&md5Ctx, pblank, (pend - pbegin) * sizeof(pbegin[0]));
//	}
//	else {
//		md5_update(&md5Ctx, (unsigned char*)&pbegin[0], (pend - pbegin) * sizeof(pbegin[0]));
//	}
//	md5_finish(&md5Ctx, hash);
//
//	return hash;
//}
//
/*
 * Hash256 + RIPEMD160
*/
inline unsigned char* Hash160(const std::vector<unsigned char>& vch)
{
    return Hash160(vch.begin(), vch.end());
}
inline size_t Hash160(const std::vector<unsigned char>& vch, std::vector<unsigned char>& vhash)
{
	size_t szHash = 0;
	unsigned char* pHash = Hash160(vch.begin(), vch.end());
	if (pHash) {
		szHash = SIZE_SHA160;
		vhash.assign(pHash, pHash + szHash);
		delete[] pHash; pHash = nullptr;
	}
	return szHash;
}

/*
* Single Hash256
*/
inline unsigned char* Hash256(const std::vector<unsigned char>& vch)
{
	return Hash256(vch.begin(), vch.end());
}
inline size_t Hash256(const std::vector<unsigned char>& vch, std::vector<unsigned char>& vhash)
{
	size_t szHash = 0;
	unsigned char* pHash = Hash256(vch.begin(), vch.end());
	if (pHash) {
		szHash = SIZE_SHA256;
		vhash.assign(pHash, pHash + szHash);
		delete[] pHash; pHash = nullptr;
	}
	return szHash;
}

/*
* Double Hash256
*/
inline unsigned char* dHash256(const std::vector<unsigned char>& vch)
{
	return Hash(vch.begin(), vch.end());
}
inline size_t dHash256(const std::vector<unsigned char>& vch, std::vector<unsigned char>& vhash)
{
	size_t szHash = 0;
	unsigned char* pHash = dHash256(vch);
	if (pHash) {
		szHash = SIZE_SHA256;
		vhash.assign(pHash, pHash + szHash);
		delete[] pHash; pHash = nullptr;
	}
	return szHash;
}

/*
 * MD5
*/
//inline unsigned char* MD5(const std::vector<unsigned char>& vch)
//{
//	return MD5(vch.begin(), vch.end());
//}
//inline size_t MD5(const std::vector<unsigned char>& vch, std::vector<unsigned char>& vhash)
//{
//	size_t szHash = 0;
//	unsigned char* pHash = MD5(vch.begin(), vch.end());
//	if (pHash) {
//		szHash = SIZE_MD5;
//		vhash.assign(pHash, pHash + szHash);
//		delete[] pHash; pHash = nullptr;
//	}
//	return szHash;
//}
//inline size_t MD5(const unsigned char*pch, std::vector<unsigned char>& vhash)
//{
//	size_t szHash = 0;
//	unsigned char* pHash = MD5(pch, pch+strlen((const char*)pch));
//	if (pHash) {
//		szHash = SIZE_MD5;
//		vhash.assign(pHash, pHash + szHash);
//		delete[] pHash; pHash = nullptr;
//	}
//	return szHash;
//}
#endif
