#pragma once

#ifndef __libETH__
#define __libETH__

#include "JUB_SDK.h"

#include <vector>

namespace jub {
namespace eth {
JUB_RV serializePreimage(const std::vector<JUB_BYTE>& vNonce,
                         const std::vector<JUB_BYTE>& vGasPrice,
                         const std::vector<JUB_BYTE>& vGasLimit,
                         const std::vector<JUB_BYTE>& vTo,
                         const std::vector<JUB_BYTE>& vAmount,
                         const std::vector<JUB_BYTE>& vInput,
                         const std::vector<JUB_BYTE>& vChainID,
                         std::vector<JUB_BYTE>& raw);

JUB_RV preimageHash(const std::vector<JUB_BYTE>& vNonce,
                    const std::vector<JUB_BYTE>& vGasPrice,
                    const std::vector<JUB_BYTE>& vGasLimit,
                    const std::vector<JUB_BYTE>& vTo,
                    const std::vector<JUB_BYTE>& vAmount,
                    const std::vector<JUB_BYTE>& vInput,
                    const std::vector<JUB_BYTE>& vChainID,
                    std::vector<JUB_BYTE>& hash);

JUB_RV serializeTx(const std::vector<JUB_BYTE>& vNonce,
                   const std::vector<JUB_BYTE>& vGasPrice,
                   const std::vector<JUB_BYTE>& vGasLimit,
                   const std::vector<JUB_BYTE>& vTo,
                   const std::vector<JUB_BYTE>& vAmount,
                   const std::vector<JUB_BYTE>& vInput,
                   const std::vector<JUB_BYTE>& vSignature,
                   std::vector<JUB_BYTE>& raw);
} // namespace eth end

} // namespace jub end

#endif
