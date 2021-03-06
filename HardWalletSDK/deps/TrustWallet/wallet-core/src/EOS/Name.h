// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include "../Data.h"

namespace TW::EOS {

class Name {
public:
    uint64_t value = 0;

    Name() { }
    Name(const std::string& str);
    uint64_t toSymbol(char c) const noexcept;
    std::string string() const noexcept;

    // JuBiter-defined
    static size_t size();

    void serialize(TW::Data& o) const noexcept;
}; // class Name end

} // namespace TW::EOS end
