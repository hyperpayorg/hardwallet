#include "utility/util.h"

#include "context/Context.h"
#include "token/interface/TokenInterface.hpp"

namespace jub {

JUB_RV Context::ShowVirtualPwd() {

    auto token = jub::TokenManager::GetInstance()->GetOne(_deviceID);
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->ShowVirtualPwd());

    return JUBR_OK;
}

JUB_RV Context::CancelVirtualPwd() {

    auto token = jub::TokenManager::GetInstance()->GetOne(_deviceID);
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->CancelVirtualPwd());

    return JUBR_OK;
}

JUB_RV Context::VerifyPIN(JUB_CHAR_PTR pinMix, OUT JUB_ULONG &retry) {

    auto token = jub::TokenManager::GetInstance()->GetOne(_deviceID);
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->VerifyPIN(pinMix, retry));

    return JUBR_OK;
}

JUB_RV Context::SetTimeout(const JUB_UINT16 timeout) {

    _timeout = timeout;

    auto token = jub::TokenManager::GetInstance()->GetOne(_deviceID);
    JUB_CHECK_NULL(token);

    JUB_VERIFY_RV(token->SetTimeout(_timeout));

    return JUBR_OK;
}

std::string Context::_FullBip32Path(const BIP32_Path& path) {

    return _mainPath
            + "/" + std::to_string(path.change)
            + "/" + std::to_string(path.addressIndex);
}

} // namespace jub end
