package clientwallet

// Wallet interface

type WalletAccount struct {
	Res       int    // 0 失败 1 成功
	Address   string // 成功必定包含地址
	PublicKey string // 公钥
	Coin      string //币种
	ErrMsg    string // 失败原因(便于排查问题,不是必定返回)
}

type TransferResult struct {
	Res    int    // 0 失败 1 成功
	ErrMsg string //  失败原因(便于排查问题,不是必定返回)
	TxID   string // 成功: 返回TxID
	Symbol string // 币种
	Params []byte //预留字段
}

type OutPutItem struct {
	TxHash   string
	Vout     uint32
	Value    int64
	Pkscript string
}

type SignInput struct {
	Coin         string // 主链币
	Symbol       string // symbol
	Amount       int64  //转账数量
	Change       int64  //找零数量
	Fee          int64  //交易费用
	SrcAddr      string //转账地址
	DestAddr     string //接受地址
	ContractAddr string //合约地址
	Net          string
	Inputs       []byte //Vin构造
	Params       []byte //预留字段
	// GasLimit     int64  // gas数量
	// GasPrice     int64  // gas价格
}

type SignResult struct {
	Res    int    // 0 失败 1 成功
	Coin   string // 主链币
	Symbol string // symbol币种
	RawTX  string //签名后的数据
	TxHash string // 交易Hash
	ErrMsg string // 失败原因(便于排查问题,不是必定返回)
	Params []byte //预留字段
}
