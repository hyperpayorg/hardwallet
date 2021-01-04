#include "JUB_SDK.h"

#include <set>
#include <string.h>
#include <algorithm>

#include "utility/util.h"
#include "utility/Singleton.h"

#include "context/Context.h"

static constexpr char const *kVersionFormat = "1.1.0.%02d%02d%02d";
static char Version[20] = {0x00,};

stContextCfg::stContextCfg() {
     mainPath = nullptr;
}

stBip32Path::stBip32Path() {
     change = JUB_ENUM_BOOL::BOOL_FALSE;
     addressIndex = 0;
}

static std::set<JUB_CHAR_CPTR> memPtrs;
JUB_RV _allocMem(JUB_CHAR_PTR_PTR memPtr, const std::string &strBuf) {

    *memPtr = (new char[strBuf.size() + 1]{0,});
    if (nullptr == *memPtr) {
        return JUBR_HOST_MEMORY;
    }

    if (0 == strBuf.size()) {
        return JUBR_OK;
    }

    memcpy(*memPtr, strBuf.data(), strBuf.size());

    if (!memPtrs.insert(*memPtr).second) {
        return JUBR_REPEAT_MEMORY_PTR;
    }

    return JUBR_OK;
}

/*****************************************************************************
* @function name : JUB_FreeMemory
* @in  param : memPtr
* @out param :
* @last change :
*****************************************************************************/
JUB_RV JUB_FreeMemory(IN JUB_CHAR_CPTR memPtr) {

    JUB_CHECK_NULL(memPtr);

    auto pos = memPtrs.find(memPtr);
    if (pos == memPtrs.end()) {
        return JUBR_INVALID_MEMORY_PTR;
    }

    delete[] memPtr; memPtr = nullptr;

    memPtrs.erase(pos);

    return JUBR_OK;
}

/*****************************************************************************
* @function name : JUB_ClearContext
* @in  param : contextID - context ID
* @out param :
* @last change :
*****************************************************************************/
JUB_RV JUB_ClearContext(IN JUB_UINT16 contextID) {

    jub::ContextManager::GetInstance()->ClearOne(contextID);

    return JUBR_OK;
}

/*****************************************************************************
* @function name : JUB_ShowVirtualPwd
* @in  param : contextID - context ID
* @out param :
* @last change :
*****************************************************************************/
JUB_RV JUB_ShowVirtualPwd(IN JUB_UINT16 contextID) {

    auto context = jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    JUB_VERIFY_RV(context->ShowVirtualPwd());

    return JUBR_OK;
}

/*****************************************************************************
* @function name : JUB_CancelVirtualPwd
* @in  param : contextID - context ID
* @out param :
* @last change :
*****************************************************************************/
JUB_RV JUB_CancelVirtualPwd(IN JUB_UINT16 contextID) {

	auto context = jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    JUB_VERIFY_RV(context->CancelVirtualPwd());

    return JUBR_OK;
}

/*****************************************************************************
* @function name : JUB_VerifyPIN
* @in  param : contextID - context ID
*            : pinMix
* @out param : retry
* @last change :
*****************************************************************************/
JUB_RV JUB_VerifyPIN(IN JUB_UINT16 contextID,
                     IN JUB_CHAR_PTR pinMix,
                     OUT JUB_ULONG_PTR pretry) {

    auto context = jub::ContextManager::GetInstance()->GetOne(contextID);
    JUB_CHECK_NULL(context);

    JUB_VERIFY_RV(context->VerifyPIN(pinMix, *pretry));

    return JUBR_OK;
}

/*****************************************************************************
* @function name : JUB_GetVersion
* @in  param :
* @out param :
* @last change :
*****************************************************************************/
JUB_CHAR_PTR JUB_GetVersion(void) {

	std::string monthes[] = {
		"Jan",
		"Feb",
		"Mar",
		"Apr",
		"May",
		"Jun",
		"Jul",
		"Aug",
		"Sep",
		"Oct",
		"Nov",
		"Dec",
	};

	std::string dateStr = __DATE__;

	int year = atoi(dateStr.substr(dateStr.length() - 2).c_str());
	int month = 0;
	for (int i = 0; i < 12; i++) {
		if (std::string::npos != dateStr.find(monthes[i])) {
			month = i + 1;
			break;
		}
	}

	std::string dayStr = dateStr.substr(4, 2);
	int day = atoi(dayStr.c_str());

	sprintf(Version, kVersionFormat, year, month, day);

	return Version;
}
