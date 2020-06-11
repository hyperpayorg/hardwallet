package clientwallet

import (
	"encoding/json"
	"fmt"
	device "hardwallet/bledevice"
)

func GenerateMnWallet(account *CreateAccount) bool {
	//func GenerateMnWallet(length int, language, password string) *WalletAccount {
	acc := device.BLEGenerateWallet(accountToJson(account))
	return acc

}
func SignRawTransaction(signIn *SignInput) *SignResult {

	json := signInputToJson(signIn)
	fmt.Println(json)
	// acc, err := hdwallet.SignRawTransaction(signInput, coinType)
	// if err != nil {
	// 	return &SignResult{
	// 		Res:    0,
	// 		Coin:   signIn.Coin,
	// 		Symbol: signIn.Symbol,
	// 		ErrMsg: err.Error(),
	// 	}
	// }
	// return &SignResult{
	// 	Res:    acc.Res,
	// 	Coin:   signIn.Coin,
	// 	Symbol: signIn.Symbol,
	// 	RawTX:  acc.RawTX,
	// 	ErrMsg: acc.ErrMsg,
	// 	Params: acc.Params,
	// }
	return &SignResult{}
}
func jsonToSignResult(jsonString string) *SignResult {
	var signResult *SignResult
	rawByte := []byte(jsonString)
	json.Unmarshal(rawByte, &signResult)
	return signResult
}

func accountToJson(account *CreateAccount) string {
	acc, err := json.Marshal(account)
	if err == nil {
		return string(acc)
	}
	return ""
}

func signInputToJson(signIn *SignInput) string {
	jsonInputs, err := json.Marshal(signIn)
	if err == nil {
		return string(jsonInputs)
	}
	return ""
}
