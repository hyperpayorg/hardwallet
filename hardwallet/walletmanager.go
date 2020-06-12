package hdwallet

import (
	"encoding/json"
	"fmt"
	"hardwallet/datazip"
	"hardwallet/keystore"
	"io/ioutil"
	"strings"
)

var (
	//	mnemonic = "boy video model trap novel debris clip father art era edit salute"
	// ## 已经支持的 BTC ETH ONT BCH LTC QTUM Libra EOS HC
	// ## 暂未支持的    *TRX
	coinMap = map[string]uint32{
		"btc": BTC,
	}
)

func GenerateMnWallet(acczip string) bool {

	json := datazip.BLEDeviceDataUnzip(acczip)
	if len(json) == 0 {
		return false
	}
	acc := JsonToCreateAccount(json)
	mnemonic, err := NewMnemonic(acc.Length, acc.Language)
	if err != nil {
		return false
	}
	//fmt.Println("mnemonic = ", mnemonic)
	keystoreJson, err := keystore.EncryptKey(mnemonic, acc.Password, "")
	if err != nil {
		return false
	}
	//fmt.Println(keystoreJson)
	err = writeKeyStoreFile("../hardwallet/keystore.txt", keystoreJson)
	if err != nil {
		return false
	}
	return true
}
func HDSignRawTransaction(zip string) string {
	result := datazip.BLEDeviceDataUnzip(zip)
	signIn := JsonToSignInput(result)
	fmt.Println(signIn)
	if coinType, ok := coinMap[strings.ToLower(signIn.Coin)]; ok {
		switch coinType {
		case BTC:
			break
		default:
			break
		}
		signResult := &SignResult{
			Res: 0,
		}
		keystoreJson, _ := readKeyStoreFile("../hardwallet/keystore.txt")
		mnemonic, _ := keystore.DecryptKey(keystoreJson, signIn.Password, "")
		master, _ := NewKey(
			Mnemonic(mnemonic), CoinType(coinType),
		)
		master.Net = signIn.Net
		wallet, _ := master.GetImportWallet(coinType)
		fmt.Println(wallet)

		return datazip.BLEDeviceDataZip(SignResultToJson(signResult))
	}

	signResult := &SignResult{
		Res: 0,
	}
	return datazip.BLEDeviceDataZip(SignResultToJson(signResult))
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

func SignResultToJson(signIn *SignResult) string {
	jsonInputs, err := json.Marshal(signIn)
	if err == nil {
		return string(jsonInputs)
	}
	return ""
}

func JsonToSignInput(jsonString string) *SignInput {
	var signIn *SignInput
	rawByte := []byte(jsonString)
	json.Unmarshal(rawByte, &signIn)
	return signIn
}

func JsonToCreateAccount(jsonString string) *CreateAccount {
	var account *CreateAccount
	rawByte := []byte(jsonString)
	json.Unmarshal(rawByte, &account)
	return account
}

// func JsonToCreateAccount(jsonString string) *CreateAccount {
// 	var account *CreateAccount
// 	rawByte := []byte(jsonString)
// 	json.Unmarshal(rawByte, &account)
// 	return account
// }
func JsonToSignResult(jsonString string) *SignResult {
	var signResult *SignResult
	rawByte := []byte(jsonString)
	json.Unmarshal(rawByte, &signResult)
	return signResult
}

func SignInputToJson(signIn *SignInput) string {
	jsonInputs, err := json.Marshal(signIn)
	if err == nil {
		return string(jsonInputs)
	}
	return ""
}
