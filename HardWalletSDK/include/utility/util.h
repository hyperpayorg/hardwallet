#pragma once
#ifndef COINPASS_BIO_UTIL_HPP
#define COINPASS_BIO_UTIL_HPP

#include "JUB_SDK.h"

#include <assert.h>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cstring>
#include "bitcoin/crypto/common.h"
#include "mSIGNA/stdutils/uchar_vector.h"
#include "airbitz-core/abcd/util/Data.hpp"
#include "Debug.hpp"

namespace jub {

#define JUB_RET_ERROR(x)                                            \
do {                                                                \
    DEBUG_ERR("Err Code:0x%x, Function: %s, File: %s, Line: %d",    \
                x, __FUNCTION__, __FILE__, __LINE__);               \
    return (x);                                                     \
} while (0)

#define JUB_VERIFY_RV(x)    \
do {                        \
    JUB_RV r = (x);         \
    if (JUBR_OK != r) {     \
        JUB_RET_ERROR(r);   \
    }                       \
} while (0)

#define JUB_CHECK_NULL(x)               \
do {                                    \
    if (!x) {                           \
        assert(false);                  \
        return JUBR_MEMORY_NULL_PTR;    \
    }                                   \
} while (0)

#define JUB_UNREFERENCED_PARAMETER(P) (P)

template <class _Ty, size_t _Size>
constexpr size_t array_size(const _Ty (&)[_Size]) noexcept {
    return (_Size);
}

/*
* String splitting function
*/
std::vector<std::string> Split(std::string str, std::string pattern);
/*
* Merge string function
*//*
std::string Join(std::vector<std::string> v, std::string pattern);
*/
/*
 * Decimal string conversion -> Hexadecimal string
 */
std::string DecStringToHexString(std::string str);

std::string numberToHexString(unsigned int val);

/*
 * char array<uint8_t, 32> -> Hexadecimal string conversion
 *//*
std::string CharArray2HexStr(std::array<uint8_t, 32> v);
*/
/*
 * Hexadecimal string conversion -> char array<uint8_t, 32>
 *//*
std::array<uint8_t, 32> HexStr2CharArray(std::string str);
*/
/*
* std::vector<unsigned char> -> Hexadecimal string conversion
*/
std::string CharPtr2HexStr(std::vector<unsigned char> v);
/*
* Hexadecimal string conversion -> std::vector<unsigned char>
*/
std::vector<unsigned char> HexStr2CharPtr(std::string str);
/*
// Put an array of strings into a vector
std::vector<std::string> CharPtrArr2StrVector(const char* Arr[]);
*/

#define ETH_PRDFIX "0x"
/*
 * std::vector<unsigned char> -> Hexadecimal string conversion
 *//*
std::string ETHCharPtr2HexStr(std::vector<unsigned char> v);
*/
/*
 * Hexadecimal string conversion -> std::vector<unsigned char>
 */
std::vector<unsigned char> ETHHexStr2CharPtr(std::string str);

abcd::DataChunk ToTlv(uint8_t tag, const abcd::DataSlice &data);
abcd::DataChunk Tolv(const abcd::DataSlice &data);
abcd::DataChunkList ParseTlv(const abcd::DataSlice &data);
abcd::DataChunk Tollv(const std::string& strData);

// vector op
std::vector<std::string> getVectorFromMap(const std::string key,
                                          std::map<std::string, std::vector<std::string>> mapString);
} // namespace jub

#endif
