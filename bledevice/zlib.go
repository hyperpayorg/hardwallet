package device

import (
	"hardwallet/datazip"
	hdwallet "hardwallet/hardwallet"
)

func BLEGenerateWallet(wallet string) bool {
	return hdwallet.GenerateMnWallet(datazip.BLEDeviceDataZip(wallet))
}
