#pragma once
#ifndef __HCTokenInterface__
#define __HCTokenInterface__

#include <JUB_SDK.h>
#include <vector>
#include <token/interface/BTCTokenInterface.hpp>

namespace jub {

class HCTokenInterface : public BTCTokenInterface {

public:
    virtual JUB_RV selectApplet_HC() = 0;
};

} // namespace jub end

#endif
