package device

import (
	"hardwallet/datazip"
	hdwallet "hardwallet/hardwallet"
)

func BLEGenerateWallet(wallet string) bool {
	return hdwallet.GenerateMnWallet(datazip.BLEDeviceDataZip(wallet))
}

func BLESignRawTransaction(sign string) string {
	return hdwallet.HDSignRawTransaction(datazip.BLEDeviceDataZip(sign))
}
