
#ifndef __Apdu__
#define __Apdu__

#include "JUB_SDK.h"

#include <vector>

namespace jub {

class APDU {

public:
    APDU() {
        cla = 0;
        ins = 0;
        p1 = 0;
        p2 = 0;
        lc = 0;
        le = 0;
        data.clear();
    }
    APDU(JUB_ULONG ncla,
         JUB_ULONG nins,
         JUB_ULONG np1,
         JUB_ULONG np2,
         JUB_ULONG nlc,
         const JUB_BYTE* pData = nullptr,
         JUB_ULONG nle = 0) :
        cla(ncla),
        ins(nins),
        p1(np1),
        p2(np2),
        lc(nlc),
        le(nle) {
        if (         0 != lc
            && nullptr != pData
            ) {
            SetData(pData, lc);
        }
        else {
            data.clear();
        }
    }
    ~APDU() {}

public:
    JUB_ULONG cla, ins, p1, p2, lc, le;
    std::vector<JUB_BYTE> data;

public:
    void SetApdu(JUB_ULONG ncla,
                 JUB_ULONG nins,
                 JUB_ULONG np1,
                 JUB_ULONG np2,
                 JUB_ULONG nlc,
                 JUB_BYTE* pData = nullptr,
                 JUB_ULONG nle = 0) {
        cla = ncla;
        ins = nins;
        p1 = np1;
        p2 = np2;
        lc = nlc;
        le = nle;
        if (         0 != lc
            && nullptr != pData
            ) {
            SetData(pData, lc);
        }
        else {
            data.clear();
        }
    }
    void SetData(const JUB_BYTE* pData, JUB_ULONG ulDataSize) {
        // assert(ulDataSize == lc);
        data.clear();
        data.resize(ulDataSize);
        data.insert(data.begin(), pData, pData + ulDataSize);
    }
}; // class APDU end

} // namespace jub end

#endif  // __Apdu__
