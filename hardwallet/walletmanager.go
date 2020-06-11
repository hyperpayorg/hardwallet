package hdwallet

import (
	"encoding/json"
	"fmt"
	device "hardwallet/bledevice"
	"hardwallet/keystore"
	"io/ioutil"
)

func GenerateMnWallet(length int, language, password string) *WalletAccount {
	mnemonic, err := NewMnemonic(length, language)
	fmt.Println("mnemonic = ", mnemonic)
	keystoreJson, err := keystore.EncryptKey(mnemonic, password, "")
	if err != nil {
		return &WalletAccount{
			Res:    0,
			ErrMsg: err.Error(),
		}
	}
	//fmt.Println(keystoreJson)
	err = writeKeyStoreFile("keystore.txt", keystoreJson)
	if err != nil {
		return &WalletAccount{
			Res:    0,
			ErrMsg: err.Error(),
		}
	}
	return &WalletAccount{
		Res: 1,
	}
}
func BLESignRawTransaction(zip string) string {
	var signIn *SignInput
	var signResult *SignResult
	result := device.DeviceDataUnzip(zip)
	if err := json.Unmarshal(result, &signIn); err != nil {
		return ""
	}
	return result
}
func readKeyStoreFile(fileName string) (string, error) {
	//b, err := ioutil.ReadFile("test.log")

	b, err := ioutil.ReadFile(fileName)
	if err != nil {
		return "", err
	}
	fmt.Println(b)
	str := string(b)
	fmt.Println(str)
	return str, nil
}

func writeKeyStoreFile(fileName string, JsonFile string) error {
	json := []byte(JsonFile)
	err := ioutil.WriteFile(fileName, json, 0644)
	return err
}

// // EnKeystore 用于将私钥和 `密码 + udid` 加密得到 keystore json 数据。
// func enKeystore(privateKey, password, udid string) *KeystoreResult {

// 	result, err := keystore.EncryptKey(privateKey, password, udid)
// 	if err != nil {
// 		return &KeystoreResult{ResCode: 0, ErrMsg: err.Error()}
// 	}
// 	return &KeystoreResult{ResCode: 1, Result: result}
// }

// // DeKeystore 用于将 keystore json 数据通过 `密码 + udid` 解密得到私钥。
// func deKeystore(json, password, udid string) *KeystoreResult {
// 	result, err := keystore.DecryptKey(json, password, udid)
// 	if err != nil {
// 		return &KeystoreResult{ResCode: 0, ErrMsg: err.Error()}
// 	}
// 	return &KeystoreResult{ResCode: 1, Result: result}
// }
