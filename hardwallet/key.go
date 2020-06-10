package hdwallet

import (
	"crypto/ecdsa"
	"encoding/hex"
	"regexp"
	"strings"

	"github.com/btcsuite/btcd/btcec"
	"github.com/btcsuite/btcd/txscript"
	"github.com/btcsuite/btcutil"
	"github.com/btcsuite/btcutil/hdkeychain"
)

// Key struct
type Key struct {
	opt      *Options
	Extended *hdkeychain.ExtendedKey

	Wif      string //for import
	Mnemonic string //助记词
	Seed     string //根种子
	Net      string
	// for btc
	Private *btcec.PrivateKey
	Public  *btcec.PublicKey
	// for eth
	PrivateECDSA *ecdsa.PrivateKey
	PublicECDSA  *ecdsa.PublicKey
}

func NewKey(opts ...Option) (*Key, error) {

	var (
		err error
		o   = newOptions(opts...)
	)
	if len(o.Mnemonic) > 0 {
		mm := strings.Replace(o.Mnemonic, " ", "", -1)
		//mm := o.Mnemonic

		if ok, _ := regexp.MatchString(`[\p{Hangul}]`, mm); ok { //韩文
			o.Language = "korean"
		}
		if ok, _ := regexp.MatchString(`[a-zA-Z]`, mm); ok { //英语
			o.Language = "english"
		}
		if ok, _ := regexp.MatchString(`[\p{Han}]`, mm); ok { //中文
			o.Language = "chinese_simplified"
		}
	}
	//fmt.Println("o.Language = ", o.Language)
	if len(o.Seed) <= 0 {
		o.Seed, err = NewSeed(o.Mnemonic, o.Password, o.Language)
	}

	if err != nil {
		return nil, err
	}
	switch o.CoinType {
	case BTC:
		o.AddressIndex = 0
		break
	default:
		break
	}

	extended, err := hdkeychain.NewMaster(o.Seed, o.Params)
	if err != nil {
		return nil, err
	}

	key := &Key{
		opt:      o,
		Extended: extended,
		Mnemonic: o.Mnemonic,
		Seed:     hex.EncodeToString(o.Seed),
	}

	err = key.init()
	if err != nil {
		return nil, err
	}

	return key, nil
}
func (k *Key) init() error {
	var err error

	k.Private, err = k.Extended.ECPrivKey()
	if err != nil {
		return err
	}

	k.Public, err = k.Extended.ECPubKey()
	if err != nil {
		return err
	}

	k.PrivateECDSA = k.Private.ToECDSA()
	k.PublicECDSA = &k.PrivateECDSA.PublicKey
	return nil
}

// GetChildKey return a key from master key
// params: [Purpose], [CoinType], [Account], [Change], [AddressIndex], [Path]
func (k *Key) GetChildKey(opts ...Option) (*Key, error) {
	var (
		err error
		o   = newOptions(opts...)
		no  = o
	)

	extended := k.Extended
	for _, i := range no.GetPath() {
		extended, err = extended.Child(i)
		if err != nil {
			return nil, err
		}
	}

	key := &Key{
		opt:      o,
		Extended: extended,
		Mnemonic: k.Mnemonic,
		Seed:     hex.EncodeToString(o.Seed),
	}

	err = key.init()
	if err != nil {
		return nil, err
	}

	return key, nil
}

// GetWallet return wallet from master key
// params: [Purpose], [CoinType], [Account], [Change], [AddressIndex], [Path]
func (k *Key) GetWallet(opts ...Option) (Wallet, error) {
	var (
		err error
		//o   = newOptions(opts...)
	)
	key, err := k.GetChildKey(opts...)
	if err != nil {
		return nil, err
	}

	coin, ok := coins[key.opt.CoinType]
	if !ok {
		return nil, ErrCoinTypeUnknow
	}

	return coin(key), nil
}

// GetWallet return wallet from master key
// params: [Purpose], [CoinType], [Account], [Change], [AddressIndex], [Path]
func (k *Key) GetImportWallet(CoinType uint32) (Wallet, error) {

	coin, ok := coins[CoinType]
	if !ok {
		return nil, ErrCoinTypeUnknow
	}

	return coin(k), nil
}

// PrivateHex generate private key to string by hex
func (k *Key) PrivateHex() string {
	return hex.EncodeToString(k.Private.Serialize())
}

// PrivateWIF generate private key to string by wif
func (k *Key) PrivateWIF(compress bool) (string, error) {
	wif, err := btcutil.NewWIF(k.Private, k.opt.Params, compress)
	if err != nil {
		return "", err
	}

	return wif.String(), nil
}

// PublicHex generate public key to string by hex
func (k *Key) PublicHex(compress bool) string {
	if compress {
		return hex.EncodeToString(k.Public.SerializeCompressed())
	}

	return hex.EncodeToString(k.Public.SerializeUncompressed())
}

// PublicHash generate public key by hash160
func (k *Key) PublicHash() ([]byte, error) {
	address, err := k.Extended.Address(k.opt.Params)
	if err != nil {
		return nil, err
	}

	return address.ScriptAddress(), nil
}

// AddressBTC generate public key to btc style address
func (k *Key) AddressBTC() (string, error) {
	address, err := k.Extended.Address(k.opt.Params)
	if err != nil {
		return "", err
	}

	return address.EncodeAddress(), nil
}

// AddressBCH generate public key to bch style address
// func (k *Key) AddressBCH() (string, error) {
//  address, err := k.Extended.Address(k.opt.Params)
//  if err != nil {
// 	 return "", err
//  }

//  addr, err := bchutil.NewCashAddressPubKeyHash(address.ScriptAddress(), k.opt.Params)
//  if err != nil {
// 	 return "", err
//  }

//  data := addr.EncodeAddress()
//  prefix := bchutil.Prefixes[k.opt.Params.Name]
//  return prefix + ":" + data, nil
// }

// AddressP2WPKH generate public key to p2wpkh style address
func (k *Key) AddressP2WPKH() (string, error) {
	pubHash, err := k.PublicHash()
	if err != nil {
		return "", err
	}

	addr, err := btcutil.NewAddressWitnessPubKeyHash(pubHash, k.opt.Params)
	if err != nil {
		return "", err
	}

	return addr.EncodeAddress(), nil
}

// AddressP2WPKHInP2SH generate public key to p2wpkh nested within p2sh style address
func (k *Key) AddressP2WPKHInP2SH() (string, error) {
	pubHash, err := k.PublicHash()
	if err != nil {
		return "", err
	}

	addr, err := btcutil.NewAddressWitnessPubKeyHash(pubHash, k.opt.Params)
	if err != nil {
		return "", err
	}

	script, err := txscript.PayToAddrScript(addr)
	if err != nil {
		return "", err
	}

	addr1, err := btcutil.NewAddressScriptHash(script, k.opt.Params)
	if err != nil {
		return "", err
	}

	return addr1.EncodeAddress(), nil
}
