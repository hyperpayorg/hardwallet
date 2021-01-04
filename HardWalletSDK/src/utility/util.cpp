#include "bigint/BigIntegerLibrary.hh"
#include "utility/util.h"

namespace jub {

/*
 * String splitting function
*/
std::vector<std::string> Split(std::string str, std::string pattern) {

	std::string::size_type pos;
	std::vector<std::string> vResult;
	str += pattern; // extend the string for convenience
	size_t size = str.size();

	for (size_t i = 0; i < size; i++) {
		pos = str.find(pattern, i);
		if (pos < size) {
			std::string s = str.substr(i, pos - i);
            if (0 < s.length()) {
                vResult.push_back(s);
            }
			i = pos + pattern.size() - 1;
		}
	}

	return vResult;
}

/*
 * Merge string function
*//*
std::string Join(std::vector<std::string> v, std::string pattern) {

	std::string strJoin;
	int i = 0;
	auto sz = v.size();
	for (auto it : v) {
		i++;
		strJoin += it;
		if (i != sz) {
			strJoin += pattern;
		}
	}

	return strJoin;
}
*/
/*
 * Decimal string conversion -> Hexadecimal string
 */
std::string DecStringToHexString(std::string str) {

    return bigUnsignedToString(stringToBigUnsigned(str.c_str()), 16);
}

std::string numberToHexString(unsigned int val) {

    return bigUnsignedToString(val, 16);
}

/*
 * char array<uint8_t, 32> -> Hexadecimal string conversion
 *//*
std::string CharArray2HexStr(std::array<uint8_t, 32> v) {

    std::stringstream ss;
    ss << std::hex;

    size_t len = v.size();
    for (size_t i(0); i < len; ++i) {
        if (0x0F >= (int)v[i]) {
            ss << 0;
        }
        ss << (int)v[i];
    }

    return ss.str();
}
*/
/*
 * Hexadecimal string conversion -> char array<uint8_t, 32>
 *//*
std::array<uint8_t, 32> HexStr2CharArray(std::string str) {

    std::array<uint8_t, 32> v;

    auto l = str.size();
	unsigned int c;
    std::string sub;
	int j = 0;
	for (int i = 0; i < l; i += 2, j++) {
        sub = str.substr(i, 2);
        std::istringstream hex_chars_stream(sub.c_str());
        hex_chars_stream >> std::hex >> c;
		v[j] = c;
    }

    return v;
}
*/
/*
 * std::vector<unsigned char> -> Hexadecimal string conversion
*/
std::string CharPtr2HexStr(std::vector<unsigned char> v) {

	std::stringstream ss;
	ss << std::hex;

	size_t len = v.size();
	for (size_t i(0); i < len; ++i) {
		if (0x0F >= (int)v[i]) {
			ss << 0;
		}
		ss << (int)v[i];
	}

	return ss.str();
}

/*
* Hexadecimal string conversion -> std::vector<unsigned char>
*/
std::vector<unsigned char> HexStr2CharPtr(std::string str) {

    std::vector<unsigned char> v;

    std::string strHex;
    auto l = str.size();
    if (l%2) {
        strHex = "0";
    }
    else {
        strHex = "";
    }
    strHex += str;
    unsigned int c;
    std::string sub;
    for (int i = 0; i < l; i += 2) {
        sub = strHex.substr(i, 2);
        std::istringstream hex_chars_stream(sub.c_str());
        hex_chars_stream >> std::hex >> c;
        v.push_back(c);
    }

    return v;
}

/*
 * std::vector<unsigned char> -> Hexadecimal string conversion
 *//*
std::string ETHCharPtr2HexStr(std::vector<unsigned char> v) {

    std::string str(ETH_PRDFIX);
    str += CharPtr2HexStr(v);
    return str;
}*/

/*
 * Hexadecimal string conversion -> std::vector<unsigned char>
 */
std::vector<unsigned char> ETHHexStr2CharPtr(std::string str) {
    if (str.empty()) {
        return std::vector<unsigned char>();
    }

    std::string _str = str;
    size_t pos = _str.find(ETH_PRDFIX);
    if (std::string::npos == pos) {
        return std::vector<unsigned char>();
    }
    _str.erase(pos, strlen(ETH_PRDFIX));
    return HexStr2CharPtr(_str);
}

/*
// Put an array of strings into a vector
std::vector<std::string> CharPtrArr2StrVector(const char* Arr[]) {

	std::vector<std::string> v;
	if (nullptr == Arr) {
		return v;
	}

	int l = 0;
	const char* p = Arr[l];
	while (p) {
		v.push_back(p);
		l++;
		p = Arr[l];
	}

	return v;
}
*/
/*
bool AddPKCSPadding(abcd::DataChunk &data, unsigned int block_size) {

	if (data.empty()) {
		return false;
	}

	auto padLen = block_size - (data.size() % block_size);
    for (auto i = 0; i < padLen; i++) {
        data.push_back((unsigned char)padLen);
    }

	return true;
}
*/
/*
bool StripPKCSPadding(abcd::DataChunk &data, unsigned long block_size) {

    if (data.empty()) {
        return false;
    }

    if (data.size() < block_size) {
        return false;
    }

    auto padValue = data.back();
    if (   (block_size < padValue)
        || (        0 == padValue)
        ) {
        return false;
    }

    for (auto i = 0; i < padValue; i++) {
        data.pop_back();
    }

    return true;
}
*/
abcd::DataChunkList ParseTlv(const abcd::DataSlice &data) {
    abcd::DataChunkList dataList;

    for (auto pos = data.begin(); pos < data.end(); ) {
        ++pos; // tag, one byte

        unsigned int len = 0;

        if (0x82 == (*pos)) {
            ++pos; // 0x82
            len = *pos << 8; // high

            ++pos; // low
            len |= *pos & 0xff;

            ++pos; // value
        }
        else if (0x81 == (*pos)) {
            ++pos; // 0x81
            len = *pos;

            ++pos; // value
        }
        else {
            len = *pos;
            ++pos; // value
        }

        dataList.push_back(abcd::DataChunk(pos, pos + len));
        pos += len;
    }

    return dataList;
}

abcd::DataChunk ToTlv(uint8_t tag, const abcd::DataSlice &data) {

    abcd::DataChunk tlvData;

    unsigned int len = static_cast<unsigned int>(data.size());

    tlvData.push_back(tag);
    if (0x100 <= len) {   // max size: two bytes 2^16
        tlvData.push_back(0x82);
        tlvData.push_back(len >> 8);
        tlvData.push_back(len & 0xFF);
    }
    else if (0x80 < len) {
        tlvData.push_back(0x81);
        tlvData.push_back(len);
    }
    else {
        tlvData.push_back(len);
    }

    tlvData.insert(tlvData.end(), data.begin(), data.end());

    return tlvData;
}

abcd::DataChunk Tolv(const abcd::DataSlice &data) {

    abcd::DataChunk lvData;

    unsigned int len = static_cast<unsigned int>(data.size());

    if (0x100 <= len) {   // max size: two bytes 2^16
        lvData.push_back(0x82);
        lvData.push_back(len >> 8);
        lvData.push_back(len & 0xFF);
    }
    else if (0x80 < len) {
        lvData.push_back(0x81);
        lvData.push_back(len);
    }
    else {
        lvData.push_back(len);
    }

    lvData.insert(lvData.end(), data.begin(), data.end());

    return lvData;
}

abcd::DataChunk Tollv(const std::string& strData) {

    abcd::DataChunk vData;
    vData.push_back(' ');
    uchar_vector data;
    data << strData;
    for (size_t i=0; i<data.size(); ++i) {
        vData.push_back(data[i]);
    }
    vData.push_back(0x00);

    abcd::DataChunk llvData;
    unsigned int len = static_cast<unsigned int>(vData.size());

    llvData.push_back(len);
    llvData.insert(llvData.end(), vData.begin(), vData.end());

    return llvData;
}

std::vector<std::string> getVectorFromMap(const std::string key,
                                          std::map<std::string, std::vector<std::string>> mapString) {
    std::vector<std::string> vValue;
    auto search = mapString.find(key);
    if (search != mapString.end()) {
        vValue = search->second;
    }
    return vValue;
}
} // namespace jub end
