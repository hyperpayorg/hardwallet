////////////////////////////////////////////////////////////////////////////////
//
// uchar_vector.h
//
// Copyright (c) 2011-2012 Eric Lombrozo
// Copyright (c) 2011-2016 Ciphrex Corp.
//
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.
//

#ifndef UCHAR_VECTOR_H__
#define UCHAR_VECTOR_H__

#include <stdio.h>
#include <stdint.h>

#include <iostream>
#include <cstring>

#include <vector>
#include <string>
#include <algorithm>

#include "machine/opcode.hpp"
#include "bitcoin/crypto/common.h"

const char g_hexBytes[][3] = {
    "00","01","02","03","04","05","06","07","08","09","0a","0b","0c","0d","0e","0f",
    "10","11","12","13","14","15","16","17","18","19","1a","1b","1c","1d","1e","1f",
    "20","21","22","23","24","25","26","27","28","29","2a","2b","2c","2d","2e","2f",
    "30","31","32","33","34","35","36","37","38","39","3a","3b","3c","3d","3e","3f",
    "40","41","42","43","44","45","46","47","48","49","4a","4b","4c","4d","4e","4f",
    "50","51","52","53","54","55","56","57","58","59","5a","5b","5c","5d","5e","5f",
    "60","61","62","63","64","65","66","67","68","69","6a","6b","6c","6d","6e","6f",
    "70","71","72","73","74","75","76","77","78","79","7a","7b","7c","7d","7e","7f",
    "80","81","82","83","84","85","86","87","88","89","8a","8b","8c","8d","8e","8f",
    "90","91","92","93","94","95","96","97","98","99","9a","9b","9c","9d","9e","9f",
    "a0","a1","a2","a3","a4","a5","a6","a7","a8","a9","aa","ab","ac","ad","ae","af",
    "b0","b1","b2","b3","b4","b5","b6","b7","b8","b9","ba","bb","bc","bd","be","bf",
    "c0","c1","c2","c3","c4","c5","c6","c7","c8","c9","ca","cb","cc","cd","ce","cf",
    "d0","d1","d2","d3","d4","d5","d6","d7","d8","d9","da","db","dc","dd","de","df",
    "e0","e1","e2","e3","e4","e5","e6","e7","e8","e9","ea","eb","ec","ed","ee","ef",
    "f0","f1","f2","f3","f4","f5","f6","f7","f8","f9","fa","fb","fc","fd","fe","ff"
};

const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

typedef unsigned int uint;

/// TODO: Add secure allocator capabilities
//template < class Allocator = allocator<unsigned char> >
class uchar_vector : public std::vector<unsigned char>//, Allocator>
{
public:
    uchar_vector() : std::vector<unsigned char>() {
        m_cur_it = begin(); m_last_op_it = begin();
    }
    uchar_vector(size_type n, const unsigned char& value = 0) : std::vector<unsigned char>(n, value) {
        m_cur_it = begin(); m_last_op_it = begin();
    }
    template <class InputIterator> uchar_vector(InputIterator first, InputIterator last) : std::vector<unsigned char>(first, last) {
        m_cur_it = begin(); m_last_op_it = begin();
    }
    uchar_vector(const std::vector<unsigned char>& vec) : std::vector<unsigned char>(vec) {
        m_cur_it = begin(); m_last_op_it = begin();
    }
    uchar_vector(const unsigned char* array, unsigned int size) : std::vector<unsigned char>(array, array + size) {
        m_cur_it = begin(); m_last_op_it = begin();
    }
    uchar_vector(const std::string& hex) {
        this->setHex(hex);
        m_cur_it = begin(); m_last_op_it = begin();
    }

    uchar_vector& operator+=(const std::vector<unsigned char>& rhs)
    {
        this->insert(this->end(), rhs.begin(), rhs.end());
        return *this;
    }

    uchar_vector& operator<<(const std::vector<unsigned char>& rhs)
    {
        this->insert(this->end(), rhs.begin(), rhs.end());
        return *this;
    }

    uchar_vector& operator<<(unsigned char byte)
    {
        this->push_back(byte);
        return *this;
    }

    // https://en.bitcoin.it/wiki/Script
    // JuBiter-defined
    uchar_vector& operator<<(uint16_t data)
    {
        uint8_t _data[2] = { 0x00, };
        WriteLE16(_data, (uint16_t)data);
        insert(end(), _data, _data + sizeof(_data));
        return *this;
    }

    // JuBiter-defined
    uchar_vector& operator<<(uint32_t data)
    {
        uint8_t _data[4] = { 0x00, };
        WriteLE32(_data, (uint32_t)data);
        insert(end(), _data, _data + sizeof(_data));
        return *this;
    }

    // JuBiter-defined
    uchar_vector& operator<<(uint64_t data)
    {
        uint8_t _data[8] = { 0x00, };
        WriteLE64(_data, (uint64_t)data);
        insert(end(), _data, _data + sizeof(_data));
        return *this;
    }

    // JuBiter-defined
    uchar_vector& operator<<(const std::string& str)
    {
        this->insert(this->end(), str.begin(), str.end());
        return *this;
    }

    // JuBiter-defined
    uchar_vector& operator<<(libbitcoin::machine::opcode byte)
    {
        this->push_back((unsigned char)byte);
        return *this;
    }

    // JuBiter-defined
    // handling OP_PUSHDATA
    uchar_vector& operator &(const std::vector<unsigned char>& rhs)
    {
        if (rhs.size() < (unsigned char)libbitcoin::machine::opcode::push_one_size) {
            insert(end(), (unsigned char)rhs.size());
        }
        else if (rhs.size() <= 0xff) {
            insert(end(), (unsigned char)libbitcoin::machine::opcode::push_one_size);
            insert(end(), (unsigned char)rhs.size());
        }
        else if (rhs.size() <= 0xffff) {
            insert(end(), (unsigned char)libbitcoin::machine::opcode::push_two_size);
            uint8_t _data[2] = { 0x00, };
            WriteLE16(_data, (uint16_t)rhs.size());
            insert(end(), _data, _data + sizeof(_data));
        }
        else {
            insert(end(), (unsigned char)libbitcoin::machine::opcode::push_four_size);
            uint8_t _data[4] = { 0x00, };
            WriteLE32(_data, (uint16_t)rhs.size());
            insert(end(), _data, _data + sizeof(_data));
        }
        insert(end(), rhs.begin(), rhs.end());
        return *this;
    }

    // JuBiter-defined
    uchar_vector build_compact_size(uint64_t size)
    {
        uchar_vector v;
        if (size < 0xfd) {
            v.insert(v.end(), (unsigned char)size);
        }
        else if (size <= 0xffff) {
            v.insert(v.end(), 0xfd);
            uint8_t _hash[2] = { 0x00, };
            WriteLE16(_hash, (uint16_t)size);
            v.insert(v.end(), _hash, _hash + sizeof(_hash));
        }
        else if (size <= 0xffff) {
            v.insert(v.end(), 0xfe);
            uint8_t _hash[4] = { 0x00, };
            WriteLE32(_hash, (uint32_t)size);
            v.insert(v.end(), _hash, _hash + sizeof(_hash));
        }
        else {
            v.insert(v.end(), 0xff);
            uint8_t _hash[8] = { 0x00, };
            WriteLE64(_hash, size);
            v.insert(v.end(), _hash, _hash + sizeof(_hash));
        }
        return v;
    }

    // JuBiter-defined
    // script_code
    uchar_vector& operator &&(const std::vector<unsigned char>& rhs)
    {
        auto v_size = build_compact_size(rhs.size());
        insert(end(), v_size.begin(), v_size.end());
        insert(end(), rhs.begin(), rhs.end());
        return *this;
    }

    // JuBiter-defined
    uchar_vector& operator &&(const std::string& str)
    {
        auto v_size = build_compact_size(str.length());
        insert(end(), v_size.begin(), v_size.end());
        insert(end(), str.begin(), str.end());
        return *this;
    }

    const uchar_vector operator+(const std::vector<unsigned char>& rightOperand) const { return uchar_vector(*this) += rightOperand; }

    uchar_vector& operator=(const std::string& hex) { this->setHex(hex); return *this; }

    void copyToArray(unsigned char* array) { std::copy(this->begin(),this->end(), array); }

    void padLeft(unsigned char pad, uint total_length)
    {
        this->reverse();
        this->padRight(pad, total_length);
        this->reverse();
    }

    void padRight(unsigned char pad, uint total_length)
    {
        for (uint i = (uint)this->size(); i < total_length; i++)
            this->push_back(pad);
    }

    std::string getHex(bool spaceBytes = false) const
    {
        std::string hex;
        hex.reserve(this->size() * 2);
        for (uint i = 0; i < this->size(); i++) {
            if (spaceBytes && (i > 0)) hex += " ";
            hex += g_hexBytes[(*this)[i]];
        }
        return hex;
    }

    void setHex(std::string hex)
    {
        this->clear();

        // pad on the left if hex contains an odd number of digits.
        if (hex.size() % 2 == 1)
            hex = "0" + hex;

        this->reserve(hex.size() / 2);

        for (uint i = 0; i < hex.size(); i+=2) {
            uint byte;
            sscanf(hex.substr(i, 2).c_str(), "%x", &byte);
            this->push_back(byte);
        }
    }

    void reverse() { std::reverse(this->begin(), this->end()); }

    uchar_vector getReverse() const
    {
        uchar_vector rval(*this);
        rval.reverse();
        return rval;
    }

    std::string getCharsAsString() const
    {
        std::string chars;
        chars.reserve(this->size());
        for (uint i = 0; i < this->size(); i++)
            chars += (*this)[i];
        return chars;
    }

    void setCharsFromString(const std::string& chars)
    {
        this->clear();
        this->reserve(chars.size());
        for (uint i = 0; i < chars.size(); i++)
            this->push_back(chars[i]);
    }

    std::string getBase64() const
    {
        unsigned int padding = (3 - (this->size() % 3)) % 3;
        std::string base64;

        uchar_vector paddedBytes = *this;
        for (unsigned int i = 1; i <= padding; i++)
            paddedBytes.push_back(0);

        base64.reserve(4*(paddedBytes.size()) / 3);

        for (unsigned int i = 0; i < paddedBytes.size(); i += 3) {
            uint32_t triple = ((uint32_t)paddedBytes[i] << 16) | ((uint32_t)paddedBytes[i+1] << 8) | (uint32_t)paddedBytes[i+2];
            base64 += base64chars[(triple & 0x00fc0000) >> 18];
            base64 += base64chars[(triple & 0x0003f000) >> 12];
            base64 += base64chars[(triple & 0x00000fc0) >> 6];
            base64 += base64chars[triple & 0x0000003f];
        }

        for (unsigned int i = 1; i <= padding; i++)
            base64[base64.size() - i] = '=';

        return base64;
    }

    void setBase64(std::string base64)
    {
        unsigned int padding = (4 - (base64.size() % 4)) % 4;

        std::string paddedBase64;
        paddedBase64.reserve(base64.size() + padding);
        paddedBase64 = base64;
        paddedBase64.append(padding, '=');
        padding = 0; // we'll count them again in the loop so we also get any that were already there.

        this->clear();
        this->reserve(3*paddedBase64.size() / 4);

        bool bEnd = false;
        for (unsigned int i = 0; (i < paddedBase64.size()) && (!bEnd); i+=4) {
            uint32_t digits[4];
            for (unsigned int j = 0; j < 4; j++) {
                const char* pPos = strchr(base64chars, paddedBase64[i+j]);
                if (!pPos) bEnd = true;
                if (bEnd) {
                    digits[j] = 0;
                    padding++;
                }
                else
                    digits[j] = (uint32_t)(pPos - base64chars);
            }

            uint32_t quadruple = (digits[0] << 18) | (digits[1] << 12) | (digits[2] << 6) | digits[3];

            this->push_back((quadruple & 0x00ff0000) >> 16);
            this->push_back((quadruple & 0x0000ff00) >> 8);
            this->push_back(quadruple & 0x000000ff);
        }

        for (unsigned int i = 0; i < padding; i++)
            this->pop_back();
    }

    // for deserialize
    // JuBiter-defined
    uint64_t read_compact_size() {
        if (m_cur_it == end()) {
            std::string _err = "error in data index: ";
            _err += std::to_string(m_last_op_it - begin());
            throw std::runtime_error(_err);
        }
        uint64_t len = 0;
        if (*m_cur_it == 0xfd) {
            m_last_op_it = m_cur_it;
            m_cur_it++;
            len = read_le_uint16();
        }
        else if (*m_cur_it == 0xfe) {
            m_last_op_it = m_cur_it;
            m_cur_it++;
            len = read_le_uint32();
        }
        else if (*m_cur_it == 0xff) {
            m_last_op_it = m_cur_it;
            m_cur_it++;
            len = read_le_uint64();
        }
        else {
            len = read_uint8();
        }

        return len;
    }
    
    uchar_vector& operator | (uchar_vector&  v)
    {

        if (m_cur_it == end())
        {
                std::string _err = "error in data index: ";
                _err += std::to_string(m_last_op_it - begin());
                throw std::runtime_error(_err);
        }

        uint32_t len = 0;
        if (*m_cur_it == (unsigned char)libbitcoin::machine::opcode::push_one_size) {
                m_last_op_it = m_cur_it;
                m_cur_it++;
                len = read_uint8();
        }
        else if (*m_cur_it == (unsigned char)libbitcoin::machine::opcode::push_two_size) {
                m_last_op_it = m_cur_it;
                m_cur_it ++;
                len = read_le_uint16();
        }
        else if (*m_cur_it == (unsigned char)libbitcoin::machine::opcode::push_four_size) {
                m_last_op_it = m_cur_it;
                m_cur_it++;
                len = read_le_uint32();
        }
        else
        {
                len = read_uint8();
        }
        if (end()- m_cur_it < len)
        {
                std::string _err = "error in data index: ";
                _err += std::to_string(m_last_op_it - begin());
                throw std::runtime_error(_err);
        }
        v.clear();
        v.insert(v.end(), m_cur_it, m_cur_it + len);
        v.reset_it();
        m_last_op_it = m_cur_it;
        m_cur_it += len;
        return *this;
    }
    uchar_vector& operator || (uchar_vector&  v)
    {
        uint64_t len = read_compact_size();

        if (end() - m_cur_it < len)
        {
                std::string _err = "error in data index: ";
                _err += std::to_string(m_last_op_it - begin());
                throw std::runtime_error(_err);
        }
        v.clear();
        v.insert(v.end(), m_cur_it, m_cur_it + len);
        v.reset_it();
        m_last_op_it = m_cur_it;
        m_cur_it += len;
        return *this;
    }

    // JuBiter-defined
    uchar_vector& operator >> (uchar_vector& v) {
        uint64_t len = read_compact_size();

        if (end() - m_cur_it < len) {
            std::string _err = "error in data index: ";
            _err += std::to_string(m_last_op_it - begin());
            throw std::runtime_error(_err);
        }
        v.clear();
        v.insert(v.end(), m_cur_it, m_cur_it + len);
        v.reset_it();
        m_last_op_it = m_cur_it;
        m_cur_it += len;
        return *this;
    }

    // JuBiter-defined
    uint8_t read_uint8() {
        if (end() - m_cur_it < 1) {
            std::string _err = "error in data index: ";
            _err += std::to_string(m_last_op_it - begin());
            throw std::runtime_error(_err);
        }
        m_last_op_it = m_cur_it;
        return *(m_cur_it++);
    };

    // JuBiter-defined
    uint16_t read_le_uint16() {
        if (end() - m_cur_it < 2) {
            std::string _err = "error in data index: ";
            _err += std::to_string(m_last_op_it - begin());
            throw std::runtime_error(_err);
        }
        uint16_t d = ReadLE16(&(*m_cur_it));
        m_last_op_it = m_cur_it;
        m_cur_it += 2;
        return d;
    };

    // JuBiter-defined
    uint16_t read_be_uint16() {
        if (end() - m_cur_it < 2) {
            std::string _err = "error in data index: ";
            _err += std::to_string(m_last_op_it - begin());
            throw std::runtime_error(_err);
        }
        uint16_t d = ReadBE16(&(*m_cur_it));
        m_last_op_it = m_cur_it;
        m_cur_it += 2;
        return d;
    };

    // JuBiter-defined
    uint32_t read_le_uint32() {
        if (end()- m_cur_it < 4) {
            std::string _err = "error in data index: ";
            _err += std::to_string(m_last_op_it - begin());
            throw std::runtime_error(_err);
        }
        uint32_t d = ReadLE32(&(*m_cur_it));
        m_last_op_it = m_cur_it;
        m_cur_it += 4;
        return d;
    }

    // JuBiter-defined
    uint32_t read_be_uint32() {
        if (end()- m_cur_it < 4) {
            std::string _err = "error in data index: ";
            _err += std::to_string(m_last_op_it - begin());
            throw std::runtime_error(_err);
        }
        uint32_t d = ReadBE32(&(*m_cur_it));
        m_last_op_it = m_cur_it;
        m_cur_it += 4;
        return d;
    }

    // JuBiter-defined
    uint64_t read_le_uint64() {
        if (end() - m_cur_it < 8) {
            std::string _err = "error in data index: ";
            _err += std::to_string(m_last_op_it - begin());
            throw std::runtime_error(_err);
        }
        uint64_t d = ReadLE64(&(*m_cur_it));
        m_last_op_it = m_cur_it;
        m_cur_it += 8;
        return d;
    }

    // JuBiter-defined
    uint64_t read_be_uint64() {
        if (end() - m_cur_it < 8) {
            std::string _err = "error in data index: ";
            _err += std::to_string(m_last_op_it - begin());
            throw std::runtime_error(_err);
        }
        uint64_t d = ReadBE64(&(*m_cur_it));
        m_last_op_it = m_cur_it;
        m_cur_it += 8;
        return d;
    }

    // JuBiter-defined
    uchar_vector read_vector(size_t len) {
        if (end() - m_cur_it < len) {
            std::string _err = "error in data index: ";
            _err += std::to_string(m_last_op_it - begin());
            throw std::runtime_error(_err);
        }
        uchar_vector v;
        v.insert(v.begin(), m_cur_it, m_cur_it + len);
        v.reset_it();
        m_last_op_it = m_cur_it;
        m_cur_it = m_cur_it + len;
        return v;
    }

    void reset_it() { m_cur_it = begin(); m_last_op_it = begin();}
    void reset_it(int offset) { m_cur_it = m_cur_it + offset; };
    std::vector<unsigned char>::iterator get_cur_it() {
        return m_cur_it;
    }

private:
    std::vector<unsigned char>::iterator m_cur_it;
    std::vector<unsigned char>::iterator m_last_op_it;
};

typedef std::string string_secure;
typedef uchar_vector uchar_vector_secure; // not really :p at least not yet!
/*
//
// Allocator that locks its contents from being paged
// out of memory and clears its contents before deletion.
//
template<typename T>
struct secure_allocator : public std::allocator<T>
{
  // MSVC8 default copy constructor is broken
  typedef std::allocator<T> base;
  typedef typename base::size_type size_type;
  typedef typename base::difference_type  difference_type;
  typedef typename base::pointer pointer;
  typedef typename base::const_pointer const_pointer;
  typedef typename base::reference reference;
  typedef typename base::const_reference const_reference;
  typedef typename base::value_type value_type;
  secure_allocator() throw() {}
  secure_allocator(const secure_allocator& a) throw() : base(a) {}
  template <typename U>
  secure_allocator(const secure_allocator<U>& a) throw() : base(a) {}
  ~secure_allocator() throw() {}
  template<typename _Other> struct rebind
  { typedef secure_allocator<_Other> other; };

  T* allocate(std::size_t n, const void *hint = 0)
  {
      T *p;
      p = std::allocator<T>::allocate(n, hint);
      if (p != NULL)
          mlock(p, sizeof(T) * n);
      return p;
  }

  void deallocate(T* p, std::size_t n)
  {
      if (p != NULL)
      {
          memset(p, 0, sizeof(T) * n);
          munlock(p, sizeof(T) * n);
      }
      std::allocator<T>::deallocate(p, n);
  }
};
*/
#endif
