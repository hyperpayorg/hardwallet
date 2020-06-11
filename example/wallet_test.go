package example

import (
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

}
