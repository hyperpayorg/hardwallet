#pragma once

#include <vector>
#include <stdint.h>

// Allowance is a free data retrieval call binding the contract method 0xdd62ed3e.
// Solidity: function allowance(address, address) constant returns(uint256)
#define ABI_METHOD_ID_ALLOWANCE         "dd62ed3e"

/**
 * @dev Sets `amount` as the allowance of `spender` over the caller's tokens.
 *
 * Returns a boolean value indicating whether the operation succeeded.
 *
 * IMPORTANT: Beware that changing an allowance with this method brings the risk
 * that someone may use both the old and the new allowance by unfortunate
 * transaction ordering. One possible solution to mitigate this race
 * condition is to first reduce the spender's allowance to 0 and set the
 * desired value afterwards:
 * https://github.com/ethereum/EIPs/issues/20#issuecomment-263524729
 *
 * Emits an {Approval} event.
 */
//Solidity: function approve(address spender, uint256 amount) external returns (bool);
#define ABI_METHOD_ID_APPROVE           "095ea7b3"

// BalanceOf is a free data retrieval call binding the contract method 0x70a08231.
// Solidity: function balanceOf(address) constant returns(uint256)
#define ABI_METHOD_ID_BALANCE_OF        "70a08231"//    *

// Decimals is a free data retrieval call binding the contract method 0x313ce567.
// Solidity: function decimals() constant returns(uint8)
#define ABI_METHOD_ID_DECIMALS          "313ce567"

// Name is a free data retrieval call binding the contract method 0x06fdde03.
// Solidity: function name() constant returns(string)
#define ABI_METHOD_ID_NAME              "06fdde03"

// SpentAllowance is a free data retrieval call binding the contract method 0xdc3080f2.
// Solidity: function spentAllowance(address,  address) constant returns(uint256)
#define ABI_METHOD_ID_SPENT_ALLOWANCE   "dc3080f2"

// Symbol is a free data retrieval call binding the contract method 0x95d89b41.
// Solidity: function symbol() constant returns(string)
#define ABI_METHOD_ID_SYMBOL            "95d89b41"

// Returns the amount of tokens in existence.
// Solidity: function totalSupply() external view returns (uint256);
#define ABI_METHOD_ID_TOTAL_SUPPLY      "18160ddd"

// ApproveAndCall is a paid mutator transaction binding the contract method 0xcae9ca51.
// Solidity: function approveAndCall(_spender address, _value uint256, _extraData bytes) returns(success bool)
#define ABI_METHOD_ID_APPROVE_AND_CALL  "cae9ca51"

// Transfer is a paid mutator transaction binding the contract method 0xa9059cbb.
// Solidity: function transfer(_to address, _value uint256) returns()
#define ABI_METHOD_ID_TRANSFER          "a9059cbb"//    *

// TransferFrom is a paid mutator transaction binding the contract method 0x23b872dd.
// Solidity: function transferFrom(_from address, _to address, _value uint256) returns(success bool)
#define ABI_METHOD_ID_TRANSFER_FROM     "23b872dd"

// Solidity: function version()
#define ABI_METHOD_ID_VERSION           "54fd4d50"

namespace jub
{
namespace eth
{
class ERC20Abi
{
public:
    virtual ~ERC20Abi();

    static std::vector<uint8_t> serialize(const std::vector<uint8_t>& address, const std::vector<uint8_t>& value);
}; // class ERC20Abi end

typedef enum {
    CREATE_CONTRACT = 0,
    WITH_ADDRESS = 1,           // abandon
    WITH_ADDRESS_AMOUNT = 2,    // abandon
    WITH_TXID = 3,
    WITH_AMOUNT = 4,            // abandon
    WITH_ADDRESS_AMOUNT_DATA = 5,
    NS_ITEM
} ENUM_CONTRACT_ABI;

class ContractAbi
{
public:
    virtual ~ContractAbi();

//    static std::vector<uint8_t> serialize(const std::vector<uint8_t>& methodID, const std::vector<uint8_t>& address);
//    static std::vector<uint8_t> serialize(const std::vector<uint8_t>& methodID, const std::vector<uint8_t>& address, const std::vector<uint8_t>& amount);
    static std::vector<uint8_t> serializeWithTxID(const std::vector<uint8_t>& methodID, const std::vector<uint8_t>& transactionID);
//    static std::vector<uint8_t> serializeWithAmt(const std::vector<uint8_t>& methodID, const std::vector<uint8_t>& amount);
    static std::vector<uint8_t> serialize(const std::vector<uint8_t>& methodID, const std::vector<uint8_t>& address, const std::vector<uint8_t>& amount, const std::vector<uint8_t>& data);

    static std::string parseMethodID(const std::vector<uint8_t>& data);
}; // class ContractAbi end
} // namespace eth end

} // namespace jub end
