package example

import (
	"encoding/json"
	"fmt"
	"hardwallet/clientwallet"
	"testing"
)

func Test_GenerateMnWallet(t *testing.T) {
	ceateAccount := &clientwallet.CreateAccount{
		Language: "english",
		Length:   12,
		Password: "123456",
	}
	fmt.Println(ceateAccount)
	isCreated := clientwallet.GenerateMnWallet(ceateAccount)
	fmt.Println(isCreated)
}

func Test_SignRawTransation(t *testing.T) {

	item1 := clientwallet.OutPutItem{

		TxHash:   "fe6c982c3b8bcf50139a364a698395dbfa12093eb59c981ed11749d04f21aa1b",
		Value:    25000000,
		Vout:     1,
		Pkscript: "76a914ce3208cbad0926f4110a1b9b3db444c269b90ca088ac",
	}

	outputs := []clientwallet.OutPutItem{item1}
	// fmt.Println("ltc outputs: ", outputs)

	jsonInputs, err := json.Marshal(outputs)
	if err != nil {
		//log.Fatal("Cannot encode to JSON ", err)
		fmt.Println("outputs err: ", err.Error())

	}

	signIn := &clientwallet.SignInput{
		Coin:     "btc",
		Symbol:   "btc",
		Amount:   10000,
		Change:   25000000 - 10000 - 10000,
		Fee:      10000,
		SrcAddr:  "",
		DestAddr: "",
		Net:      "testnet",
		Password: "123456",
		Inputs:   jsonInputs,
	}
	signResult := clientwallet.SignRawTransaction(signIn)
	fmt.Println(signResult)
}
