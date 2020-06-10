package device

import (
	"bytes"
	"compress/zlib"
	"encoding/base64"
	"io/ioutil"
)

func DeviceDataZip(raw string) string {
	b := []byte(raw)
	var out bytes.Buffer
	w := zlib.NewWriter(&out)
	w.Write(b)
	w.Close()
	return base64.StdEncoding.EncodeToString(out.Bytes())
}

func DeviceDataUnzip(encoded string) string {
	decoded, err := base64.StdEncoding.DecodeString(encoded)
	if err != nil {
		return ""
	}
	reader, err := zlib.NewReader(bytes.NewBuffer(decoded))
	if err != nil {
		return ""
	}
	raw, err := ioutil.ReadAll(reader)
	if err != nil {
		return ""
	}
	return string(raw)
}
