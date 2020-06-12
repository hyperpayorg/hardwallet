package hdwallet

import (
	"bytes"
	"encoding/hex"
	"encoding/json"
	"fmt"

	"github.com/btcsuite/btcd/chaincfg"
	"github.com/btcsuite/btcd/chaincfg/chainhash"
	"github.com/btcsuite/btcd/txscript"
	"github.com/btcsuite/btcd/wire"
	"github.com/btcsuite/btcutil"
)

func init() {
	coins[BTC] = newBTC
}

type btc struct {
	name   string
	symbol string
	key    *Key
}

func newBTC(key *Key) Wallet {
	return &btc{
		name:   "Bitcoin",
		symbol: "BTC",
		key:    key,
	}
}

func (c *btc) GetType() uint32 {
	return c.key.opt.CoinType
}

func (c *btc) GetName() string {
	return c.name
}

func (c *btc) GetSymbol() string {
	return c.symbol
}

func (c *btc) GetKey() *Key {
	return c.key
}

func (c *btc) GetAddress() (string, error) {
	return c.key.AddressBTC()
}

func (c *btc) SignRawTransaction(signIn *SignInput) (*SignResult, error) {
	var vins []OutPutItem
	var vouts []OutPutItem
	json.Unmarshal(signIn.Inputs, &vins)
	fmt.Println("vins : ", vins)
	mtx := wire.NewMsgTx(1)
	net := &chaincfg.MainNetParams
	if signIn.Net == "testnet" {
		net = &chaincfg.TestNet3Params
	}
	c.key.opt.Params = net
	signIn.SrcAddr, _ = c.GetAddress()
	fmt.Println("SrcAddr = ", signIn.SrcAddr)
	///////////////
	dest_addr, err := btcutil.DecodeAddress(signIn.DestAddr, net)
	if err != nil {
		return nil, err
	}
	fmt.Println("dest_addr : ", dest_addr)
	dest_pkScript, err := txscript.PayToAddrScript(dest_addr)
	if err != nil {
		return nil, err
	}

	output := &wire.TxOut{
		Value:    signIn.Amount,
		PkScript: dest_pkScript,
	}
	mtx.AddTxOut(output)
	// Add all outputs as inputs
	var spendValue int64 = 0
	for _, input := range vins {
		txHash, err := chainhash.NewHashFromStr(input.TxHash)
		if err != nil {
			return nil, fmt.Errorf("txid error")
		}
		prevOut := wire.NewOutPoint(txHash, input.Vout)
		txIn := wire.NewTxIn(prevOut, []byte{}, [][]byte{})
		mtx.AddTxIn(txIn)
		vouts = append(vouts, input)
		spendValue = spendValue + input.Value
		if spendValue >= signIn.Change+signIn.Amount+signIn.Fee {
			break
		}
	}

	addrSrc, err := btcutil.DecodeAddress(signIn.SrcAddr, net)
	if err != nil {
		return nil, err
	}

	if signIn.Change > 0 {
		pkScriptSrc, err := txscript.PayToAddrScript(addrSrc)
		if err != nil {
			return nil, err
		}

		output = &wire.TxOut{
			Value:    signIn.Change,
			PkScript: pkScriptSrc,
		}
		mtx.AddTxOut(output)
	}
	for i, input := range vouts {
		pk, _ := hex.DecodeString(input.Pkscript)
		if err != nil {
			return nil, err
		}
		//sigScript, err := txscript.SignatureScript(mtx, i, input.Value, pk, txscript.SigHashAll, a.PrivKey, true)
		sigScript, err := txscript.SignatureScript(mtx, i, pk, txscript.SigHashAll, c.GetKey().Private, true)
		fmt.Println(hex.EncodeToString(sigScript))
		if err != nil {
			return nil, err
		}
		mtx.TxIn[i].SignatureScript = sigScript
	}

	// Serialize the transaction and convert to hex string.
	buf := bytes.NewBuffer(make([]byte, 0, mtx.SerializeSize()))
	if err := mtx.Serialize(buf); err != nil {
		return nil, err
	}
	txHex := hex.EncodeToString(buf.Bytes())
	fmt.Println("txHex :", txHex)
	return &SignResult{
		Res:   1,
		RawTX: txHex,
	}, nil
}

func pubKeyHashToScript(pubKey []byte) []byte {
	pubKeyHash := btcutil.Hash160(pubKey)
	script, err := txscript.NewScriptBuilder().
		AddOp(txscript.OP_0).AddData(pubKeyHash).Script()
	if err != nil {
		panic(err)
	}
	return script
}
func (c *btc) GetWalletAccount() *WalletAccount {
	if c.GetKey().Extended == nil {
		return &WalletAccount{
			Res: 0,
		}
	}
	btcAddress, err := c.GetAddress()
	if err != nil {
		return &WalletAccount{
			Res:    0,
			ErrMsg: err.Error(),
		}
	}
	publicKey := c.GetKey().PublicHex(true)

	return &WalletAccount{
		Res:       1,
		Address:   btcAddress,
		PublicKey: publicKey,
		Seed:      c.GetKey().Seed,
	}
}
