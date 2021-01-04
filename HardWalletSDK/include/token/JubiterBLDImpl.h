#ifndef __HardwareTokenImpl__
#define __HardwareTokenImpl__

#include "JUB_SDK_DEV_BLE.h"
#include <memory>

#include "utility/util.h"

#include "token/interface/TokenInterface.hpp"
#include <token/interface/HCTokenInterface.hpp>
#include "device/ApduBuilder.hpp"
#include "device/JubiterHidDevice.hpp"

#ifdef HID_MODE // modify later..
#include "device/JubiterHidDevice.hpp"
using DeviceType = jub::JubiterHidDevice;
#else
#include "device/JubiterBLEDevice.hpp"
using DeviceType = jub::JubiterBLEDevice;
#endif

namespace jub {

constexpr JUB_BYTE kPKIAID_FIDO[8] = {
    0xa0, 0x00,	0x00, 0x06, 0x47, 0x2f, 0x00, 0x01
};

constexpr JUB_BYTE kPKIAID_MISC[16] = {
    0xD1, 0x56, 0x00, 0x01, 0x32, 0x03, 0x00, 0x42, 0x4C, 0x44, 0x00, 0x6D, 0x69, 0x73, 0x63, 0x01
};

typedef enum class enumCoinTypeMisc {
    COIN = 0x00,
    COINEOS = 0x01,
    COINXRP = 0x02,
    Default = COIN
} JUB_ENUM_COINTYPE_MISC;

typedef struct _stAppInfos_ {
    abcd::DataChunk appID;
    std::string coinName;
    std::string minimumAppletVersion;
} stAppInfos;

class JubiterBLDImpl : public CommonTokenInterface,
                       public ETHTokenInterface,
                       public HCTokenInterface,
	                   public EOSTokenInterface,
                       public XRPTokenInterface {
public:
    JubiterBLDImpl(std::string path);
    JubiterBLDImpl(DeviceType* device);
    ~JubiterBLDImpl();

public:
    /* functions */
    virtual JUB_RV ConnectToken();
    virtual JUB_RV DisconnectToken();

	//HC functions
	virtual JUB_RV selectApplet_HC();

    //BTC functions
    virtual JUB_RV SelectAppletBTC();
    virtual JUB_RV GetHDNodeBTC(const JUB_BTC_TRANS_TYPE& type, const std::string& path, std::string& xpub);
    virtual JUB_RV GetAddressBTC(const JUB_BYTE addrFmt, const JUB_BTC_TRANS_TYPE& type, const std::string& path, const JUB_UINT16 tag, std::string& address);
    virtual JUB_RV GetAddressMultisigBTC(const JUB_BYTE addrFmt,
                                         const JUB_BTC_TRANS_TYPE& type,
                                         const std::string& path,
                                         const JUB_UINT16 tag,
                                         const uchar_vector& vRedeemScriptTlv,
                                         std::string& address);
    virtual JUB_RV SetUnitBTC(const JUB_BTC_UNIT_TYPE& unit);
    virtual JUB_RV SetCoinTypeBTC(const JUB_ENUM_COINTYPE_BTC& type);
    virtual JUB_RV SignTXBTC(const JUB_BYTE addrFmt,
                             const JUB_BTC_TRANS_TYPE& type,
                             const JUB_UINT16 inputCount,
                             const std::vector<JUB_UINT64>& vInputAmount,
                             const std::vector<std::string>& vInputPath,
                             const std::vector<JUB_UINT16>& vChangeIndex,
                             const std::vector<std::string>& vChangePath,
                             const std::vector<JUB_BYTE>& vUnsigedTrans,
                             std::vector<JUB_BYTE>& vRaw);
    virtual JUB_RV SignTXMultisigBTC(const JUB_BYTE addrFmt,
                                     const JUB_BTC_TRANS_TYPE& type,
                                     const JUB_UINT16 inputCount,
                                     const std::vector<JUB_UINT64>& vInputAmount,
                                     const std::vector<std::string>& vInputPath,
                                     const std::vector<uchar_vector>& vRedeemScriptTlv,
                                     const std::vector<JUB_UINT16>& vChangeIndex,
                                     const std::vector<std::string>& vChangePath,
                                     const std::vector<uchar_vector>& vChangeRedeemScriptTlv,
                                     const std::vector<JUB_BYTE>& vUnsigedTrans,
                                     std::vector<uchar_vector>& vSignatureRaw);

    //ETH functions
    virtual JUB_RV SelectAppletETH();
    virtual JUB_RV GetAppletVersionETH(std::string& version);
    virtual JUB_RV GetAddressETH(const std::string& path, const JUB_UINT16 tag, std::string& address);
    virtual JUB_RV GetHDNodeETH(const JUB_BYTE format, const std::string& path, std::string& pubkey);
    virtual JUB_RV SignTXETH(const bool bERC20,
                             const std::vector<JUB_BYTE>& vNonce,
                             const std::vector<JUB_BYTE>& vGasPrice,
                             const std::vector<JUB_BYTE>& vGasLimit,
                             const std::vector<JUB_BYTE>& vTo,
                             const std::vector<JUB_BYTE>& vValue,
                             const std::vector<JUB_BYTE>& vData,
                             const std::vector<JUB_BYTE>& vPath,
                             const std::vector<JUB_BYTE>& vChainID,
                             std::vector<JUB_BYTE>& vRaw);
    virtual JUB_RV SetERC20ETHToken(const std::string& tokenName,
                                    const JUB_UINT16 unitDP,
                                    const std::string& contractAddress);
    virtual JUB_RV SignContractETH(const JUB_BYTE inputType,
                                   const std::vector<JUB_BYTE>& vNonce,
                                   const std::vector<JUB_BYTE>& vGasPrice,
                                   const std::vector<JUB_BYTE>& vGasLimit,
                                   const std::vector<JUB_BYTE>& vTo,
                                   const std::vector<JUB_BYTE>& vValue,
                                   const std::vector<JUB_BYTE>& vInput,
                                   const std::vector<JUB_BYTE>& vPath,
                                   const std::vector<JUB_BYTE>& vChainID,
                                   std::vector<JUB_BYTE>& signatureRaw);

    virtual JUB_RV SignContractHashETH(const JUB_BYTE inputType,
                                       const std::vector<JUB_BYTE>& vGasLimit,
                                       const std::vector<JUB_BYTE>& vInputHash,
                                       const std::vector<JUB_BYTE>& vUnsignedTxHash,
                                       const std::vector<JUB_BYTE>& vPath,
                                       const std::vector<JUB_BYTE>& vChainID,
                                       std::vector<JUB_BYTE>& signatureRaw);

    //EOS functions
    virtual JUB_RV SelectAppletEOS();
    virtual JUB_RV SetCoinTypeEOS();
    virtual JUB_RV GetAddressEOS(const TW::EOS::Type& type, const std::string& path, const JUB_UINT16 tag, std::string& address);
    virtual JUB_RV GetHDNodeEOS(const JUB_BYTE format, const std::string& path, std::string& pubkey);
    virtual JUB_RV SignTXEOS(const TW::EOS::Type& type,
                             const std::vector<JUB_BYTE>& vPath,
                             const std::vector<JUB_BYTE>& vChainId,
                             const std::vector<JUB_BYTE>& vRaw,
                             std::vector<uchar_vector>& vSignatureRaw,
                             const bool bWithType=false);

    //XRP functions
    virtual JUB_RV SelectAppletXRP();
    virtual JUB_RV SetCoinTypeXRP();
    virtual JUB_RV GetAddressXRP(const std::string& path, const JUB_UINT16 tag, std::string& address);
    virtual JUB_RV GetHDNodeXRP(const JUB_BYTE format, const std::string& path, std::string& pubkey);
    virtual JUB_RV SignTXXRP(const std::vector<JUB_BYTE>& vPath,
                             std::vector<JUB_BYTE>& vUnsignedRaw,
                             std::vector<uchar_vector>& vSignatureRaw);

    //common token functions
    virtual JUB_RV QueryBattery(JUB_BYTE &percent);
    virtual JUB_RV ShowVirtualPwd();
    virtual JUB_RV CancelVirtualPwd();
    virtual bool   IsInitialize();
    virtual bool   IsBootLoader();
    virtual JUB_RV GetSN(JUB_BYTE sn[24]);
    virtual JUB_RV GetLabel(JUB_BYTE label[32]);
    virtual JUB_RV GetPinRetry(JUB_BYTE& retry);
    virtual JUB_RV GetPinMaxRetry(JUB_BYTE& maxRetry);
    virtual JUB_RV GetBleVersion(JUB_BYTE bleVersion[4]);
    virtual JUB_RV GetFwVersion(JUB_BYTE fwVersion[4]);

    virtual JUB_RV EnumApplet(std::string& appletList);
    virtual JUB_RV GetAppletVersion(const std::string& appID, std::string& version);
    virtual JUB_RV EnumSupportCoins(std::string& coinList);
    virtual JUB_RV GetDeviceCert(std::string& cert);
    virtual JUB_RV SendOneApdu(const std::string& apdu, std::string& response);

    virtual JUB_RV VerifyPIN(const std::string &pinMix, OUT JUB_ULONG &retry);

    virtual JUB_RV SetTimeout(const JUB_UINT16 timeout);

    // get function
    std::string getPath() {
        return _path;
    };

    static stAppInfos g_appInfo[];

private:
    JUB_RV _SelectApp(const JUB_BYTE PKIAID[],
                      JUB_BYTE length);

    JUB_RV _SetCoinType(const JUB_BYTE& type);

    JUB_RV _TranPack(const abcd::DataSlice &apduData,
                     const JUB_BYTE highMark,
                     const JUB_BYTE sigType,
                     const JUB_ULONG ulSendOnceLen,
                     int finalData = false,
                     int bOnce = false);
    JUB_RV _TranPackApdu(const JUB_ULONG ncla,
                         const JUB_ULONG nins,
                         const abcd::DataSlice &apduData,
                         const JUB_BYTE highMark,
                         const JUB_BYTE sigType,
                         const JUB_ULONG ulSendOnceLen,
                         JUB_BYTE *retData = nullptr, JUB_ULONG *pulRetDataLen = nullptr,
                         int finalData = false,
                         int bOnce = false);

    JUB_RV _SendApdu(const APDU *apdu, JUB_UINT16 &wRet,
                     JUB_BYTE *retData = nullptr, JUB_ULONG *pulRetDataLen = nullptr,
                     JUB_ULONG ulMiliSecondTimeout = 1200000);

    JUB_BYTE _HighMark(const JUB_ULONG& highMark) {
        return ((highMark&0x0F) << 4);
    }

    //BTC functions
    bool _isSupportLegacyAddress();
    JUB_BYTE _RealAddressFormat(const JUB_ULONG& addrFmt);
    JUB_BYTE _RealAddressFormat(const JUB_ENUM_BTC_ADDRESS_FORMAT& addrFmt);

    std::shared_ptr<ApduBuilder> _apduBuiler;
    std::shared_ptr<DeviceType> _device;
    std::string _path;
    std::string _appletVersion;
}; // class JubiterBLDImpl end

}  // namespace jub end

#endif  // __HardwareTokenImpl__
