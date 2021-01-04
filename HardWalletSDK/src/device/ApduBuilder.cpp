#include "device/ApduBuilder.hpp"

namespace jub {

JUB_RV JubApudBuiler::BuildApdu(const APDU *apdu,
                                std::vector<JUB_BYTE> &vSafeApdu) {
    // copy header
    vSafeApdu.push_back((JUB_BYTE)apdu->cla);
    vSafeApdu.push_back((JUB_BYTE)apdu->ins);
    vSafeApdu.push_back((JUB_BYTE)apdu->p1);
    vSafeApdu.push_back((JUB_BYTE)apdu->p2);

    if (0 != apdu->lc) {
        // get lc,data
        if (0xFF < apdu->lc) {
            vSafeApdu.push_back((JUB_BYTE)( (apdu->lc)/0x10000));
            vSafeApdu.push_back((JUB_BYTE)(((apdu->lc)/0x100)%0x100));
            vSafeApdu.push_back((JUB_BYTE)( (apdu->lc)%0x100));
            vSafeApdu.insert(vSafeApdu.end(), apdu->data.begin(),
                             apdu->data.begin() + apdu->lc);
        }
        else {
            vSafeApdu.push_back((JUB_BYTE)(apdu->lc));
            vSafeApdu.insert(vSafeApdu.end(), apdu->data.begin(),
                             apdu->data.begin() + apdu->lc);
        }
    }

    if (0 != apdu->le) {
        if (0xFF < apdu->le) {
            vSafeApdu.push_back((JUB_BYTE)( (apdu->le)/0x10000));
            vSafeApdu.push_back((JUB_BYTE)(((apdu->le)/0x100)%0x100));
            vSafeApdu.push_back((JUB_BYTE)( (apdu->le)%0x100));
        }
        else {
            vSafeApdu.push_back((JUB_BYTE)(apdu->le));
        }
    }

    return JUBR_OK;
}

JUB_RV JubApudBuiler::PackData(std::vector<JUB_BYTE> &vDest,
                               std::vector<JUB_BYTE> &vSrc) {

	vDest = vSrc;
    return JUBR_OK;
}

JUB_RV JubApudBuiler::UnPackData(std::vector<JUB_BYTE> &vDest,
                                 std::vector<JUB_BYTE> &vSrc) {
	vDest = vSrc;
    return JUBR_OK;
}

}  // namespace jub end
