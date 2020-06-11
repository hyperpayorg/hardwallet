package example

import (
	"fmt"
	"hardwallet/clientwallet"
	"testing"
)

func Test_TestGenerateMnWallet(t *testing.T) {

	account := clientwallet.GenerateMnWallet(12, "")
	fmt.Println(account)
}
