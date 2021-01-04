#include "libETH/ERC20Abi.h"

#include <vector>
#include <array>
#include "mSIGNA/stdutils/uchar_vector.h"

namespace jub {

namespace eth {

const size_t kETHEREUM_SIZE_VARIABLE_FUNCTION_CONTRACT = 32;
const size_t kETH_METHOD_HASH_SIZE = 4;
typedef std::array<uint8_t, kETH_METHOD_HASH_SIZE> EthereumContractMethodHash;

struct EthereumContractPayloadStream {
public:
    EthereumContractPayloadStream()
        : m_data() {
        m_data.reserve(256);
    }

    void write_data(const void* data, size_t size) {
        const uint8_t* d = reinterpret_cast<const uint8_t*>(data);
        m_data.insert(m_data.end(), d, d + size);
    }

    std::vector<std::uint8_t> get_data() {
        return m_data;
    }

    ~EthereumContractPayloadStream() {
    }

protected:
    std::vector<std::uint8_t> m_data;
}; // struct EthereumContractPayloadStream end

EthereumContractPayloadStream& operator<<(EthereumContractPayloadStream& stream, const std::vector<std::uint8_t>& data) {

    static const std::array<uint8_t, kETHEREUM_SIZE_VARIABLE_FUNCTION_CONTRACT> ZEROES = {0,};
    stream.write_data(ZEROES.data(), kETHEREUM_SIZE_VARIABLE_FUNCTION_CONTRACT - data.size());
    stream.write_data(&data[0], data.size());

    return stream;
}

EthereumContractPayloadStream& operator<<(EthereumContractPayloadStream& stream, const EthereumContractMethodHash& method_hash) {

    stream.write_data(method_hash.data(), method_hash.size());

    return stream;
}

std::vector<uint8_t> ERC20Abi::serialize(const std::vector<uint8_t>& address, const std::vector<uint8_t>& value) {

    EthereumContractPayloadStream stream;
    uchar_vector vMethodID(ABI_METHOD_ID_TRANSFER);
    EthereumContractMethodHash hash;
    std::copy_n(vMethodID.begin(), vMethodID.size(), hash.begin());
    stream << hash;
    stream << address;
    stream << value;

    return stream.get_data();
}

//std::vector<uint8_t> ContractAbi::serialize(const std::vector<uint8_t>& methodID, const std::vector<uint8_t>& address) {
//
//    EthereumContractPayloadStream stream;
//    EthereumContractMethodHash hash;
//    std::copy_n(methodID.begin(), methodID.size(), hash.begin());
//    stream << hash;
//    stream << address;
//
//    return stream.get_data();
//}
//
//std::vector<uint8_t> ContractAbi::serialize(const std::vector<uint8_t>& methodID, const std::vector<uint8_t>& address, const std::vector<uint8_t>& amount) {
//
//    EthereumContractPayloadStream stream;
//    EthereumContractMethodHash hash;
//    std::copy_n(methodID.begin(), methodID.size(), hash.begin());
//    stream << hash;
//    stream << address;
//    stream << amount;
//
//    return stream.get_data();
//}
//
std::vector<uint8_t> ContractAbi::serializeWithTxID(const std::vector<uint8_t>& methodID, const std::vector<uint8_t>& transactionID) {

    EthereumContractPayloadStream stream;
    EthereumContractMethodHash hash;
    std::copy_n(methodID.begin(), methodID.size(), hash.begin());
    stream << hash;
    stream << transactionID;

    return stream.get_data();
}

//std::vector<uint8_t> ContractAbi::serializeWithAmt(const std::vector<uint8_t>& methodID, const std::vector<uint8_t>& amount) {
//
//    EthereumContractPayloadStream stream;
//    EthereumContractMethodHash hash;
//    std::copy_n(methodID.begin(), methodID.size(), hash.begin());
//    stream << hash;
//    stream << amount;
//
//    return stream.get_data();
//}
//
std::vector<uint8_t> ContractAbi::serialize(const std::vector<uint8_t>& methodID, const std::vector<uint8_t>& address, const std::vector<uint8_t>& amount, const std::vector<uint8_t>& data) {

    EthereumContractPayloadStream stream;
    EthereumContractMethodHash hash;
    std::copy_n(methodID.begin(), methodID.size(), hash.begin());
    stream << hash;
    stream << address;
    stream << amount;

    std::vector<uint8_t> functionId;
    functionId.push_back(0x60);
    stream << functionId;

    std::vector<uint8_t> dataLen;
    dataLen.push_back(data.size());
    stream << dataLen;
    stream << data;

    return stream.get_data();
}

std::string ContractAbi::parseMethodID(const std::vector<uint8_t>& data) {
    if (0 == data.size()) {
        return "";
    }

    uchar_vector v(data);

    uchar_vector vMethodID;
    vMethodID << v.read_vector(kETH_METHOD_HASH_SIZE);

    return vMethodID.getHex();
}
} // namespace eth end

} // namespace jub end
