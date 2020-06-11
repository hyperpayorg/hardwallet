package hdwallet

import (
	"encoding/json"
	"fmt"
	"hardwallet/datazip"
	"hardwallet/keystore"
	"io/ioutil"
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
	fmt.Println("mnemonic = ", mnemonic)
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
	//var signIn *SignInput
	result := datazip.BLEDeviceDataUnzip(zip)
	signIn := JsonToSignInput(result)
	fmt.Println(signIn)
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
