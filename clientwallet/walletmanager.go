package clientwallet

func GenerateMnWallet(length int, language string) *WalletAccount {

	return &WalletAccount{
		Res:    0,
		ErrMsg: "Coin type is not supported!",
	}
}
