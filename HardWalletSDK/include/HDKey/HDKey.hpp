/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HDKey.hpp
 * Author: zxc_work
 *
 * Created on 2019年7月19日, 下午2:02
 */

#ifndef HDKEY_HPP
#define HDKEY_HPP

#include "JUB_SDK.h"
#include <TrezorCrypto/bip32.h>
#include <string>
#include <vector>
#include "constant.hpp"

namespace jub {
std::vector<JUB_UINT32> parsePath(const std::string& path);

JUB_RV hdnode_pub_ckd(std::string xpub, std::string path, std::string curve, JUB_UINT32 xpubPrefix ,JUB_UINT32 xprvPrefix,HDNode* node, JUB_UINT32* parentFingerprint);

JUB_RV hdnode_priv_ckd(std::string xprv, std::string path, std::string curve,JUB_UINT32 xpubPrefix ,JUB_UINT32 xprvPrefix, HDNode* node, JUB_UINT32* parentFingerprint);
}

#endif /* HDKEY_HPP */
