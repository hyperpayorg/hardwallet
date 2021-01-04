#ifndef __ApduBuilder__
#define __ApduBuilder__

#include "utility/Apdu.hpp"

namespace jub {

class ApduBuilder {

public:
    /* functions */
    virtual JUB_RV BuildApdu(const APDU *apdu,
                             std::vector<JUB_BYTE> &vSafeApdu) = 0;

    // do something (for example: encrypt data)
    virtual JUB_RV PackData(std::vector<JUB_BYTE> &vDest,
                            std::vector<JUB_BYTE> &vSrc) = 0;

    // do something (for example: decrypt data)
    virtual JUB_RV UnPackData(std::vector<JUB_BYTE> &vDest,
                              std::vector<JUB_BYTE> &vSrc) = 0;
}; // class ApduBuilder end

class JubApudBuiler : public ApduBuilder {

public:
    /* functions */
    virtual JUB_RV BuildApdu(const APDU *apdu, std::vector<JUB_BYTE> &vSafeApdu);

    virtual JUB_RV PackData(std::vector<JUB_BYTE> &vDest,
                            std::vector<JUB_BYTE> &vSrc);

    virtual JUB_RV UnPackData(std::vector<JUB_BYTE> &vDest,
                              std::vector<JUB_BYTE> &vSrc);
}; // class JubApudBuiler end

}  // namespace jub end
#endif  // __ApduBuilder__
