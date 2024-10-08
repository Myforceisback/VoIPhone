#include "Magma.h"
#include <string>

//  id-GostR3411-94-TestParamSet+DiagCornerSwap
const std::array<std::array<Magma::byte_t, 16>, 8> Magma::_s_blocks = {
    0xC, 0xA, 0x9, 0x2, 0xD, 0x8, 0x0, 0xE, 0x6, 0xB, 0x1, 0xC, 0x7, 0xF, 0x5, 0x1,
    0xE, 0xB, 0x4, 0xC, 0x6, 0xD, 0xF, 0xA, 0x2, 0x3, 0x8, 0x1, 0x0, 0x7, 0x5, 0x9,
    0x5, 0x8, 0x1, 0xD, 0xA, 0x3, 0x4, 0x2, 0xE, 0xF, 0xC, 0x7, 0x6, 0x0, 0x9, 0xB,
    0x7, 0xD, 0xA, 0x1, 0x0, 0x8, 0x9, 0xF, 0xE, 0x4, 0x6, 0xC, 0xB, 0x2, 0x5, 0x3,
    0x6, 0xC, 0x7, 0x1, 0x5, 0xF, 0xD, 0x8, 0x4, 0xA, 0x9, 0xE, 0x0, 0x3, 0xB, 0x2,
    0x4, 0xB, 0xA, 0x0, 0x7, 0x2, 0x1, 0xD, 0x3, 0x6, 0x8, 0x5, 0x9, 0xC, 0xF, 0xE,
    0xD, 0xB, 0x4, 0x1, 0x3, 0xF, 0x5, 0x9, 0x0, 0xA, 0xE, 0x7, 0x6, 0x8, 0x2, 0xC,
    0x3, 0xF, 0xD, 0x0, 0x5, 0x7, 0xA, 0x4, 0x9, 0x2, 0x3, 0xE, 0x6, 0xB, 0x8, 0x4,
};

void operator >> (std::basic_istream<char, std::char_traits<char>>& is, Magma::vec_byte_t& v) {
    Magma::byte_t b;
    is >> b;
    v.push_back(b);
};

template<size_t S>
std::array<Magma::byte_t, S> operator ^ (std::array<Magma::byte_t, S> left, std::array<Magma::byte_t, S> right) {
    std::array<Magma::byte_t, S> result;
    for (size_t i = 0; i < S; ++i)
        result[i] = left[i] ^ right[i];
    return result;
};

Magma::Magma(const char* key, const char* iv) {
    for (size_t i = 0; i < 8; ++i) {
        uint32_t part = static_cast<uint32_t>(key[4 * i])
            + static_cast<uint32_t>(key[4 * i + 1])
            + static_cast<uint32_t>(key[4 * i + 2])
            + static_cast<uint32_t>(key[4 * i + 3]);
        _key[i] = part;
        _initialization_vector[i] = static_cast<byte_t>(iv[i]);
    };
}

uint32_t Magma::_f(const std::array<byte_t, 4>& A, const uint32_t& key) {
    std::array<byte_t, 8> four_bit_blocks;
    uint64_t A_bits = (_blockToBits<uint32_t>(A) + key) % (1ull << 32);

    four_bit_blocks[7] = _s_blocks.at(0).at((A_bits & 0xF));
    four_bit_blocks[6] = _s_blocks.at(1).at((A_bits & 0xF0) >> 4);
    four_bit_blocks[5] = _s_blocks.at(2).at((A_bits & 0xF00) >> 8);
    four_bit_blocks[4] = _s_blocks.at(3).at((A_bits & 0xF000) >> 12);
    four_bit_blocks[3] = _s_blocks.at(4).at((A_bits & 0xF0000) >> 16);
    four_bit_blocks[2] = _s_blocks.at(5).at((A_bits & 0xF00000) >> 20);
    four_bit_blocks[1] = _s_blocks.at(6).at((A_bits & 0xF000000) >> 24);
    four_bit_blocks[0] = _s_blocks.at(7).at((A_bits & 0xF0000000) >> 28);

    uint32_t res = _blockToBits<uint32_t>(four_bit_blocks);

    return ((res << 11) | (res >> 21));
};

std::array<Magma::byte_t, 8> Magma::_block_cipher(const std::array<uint32_t, 8>& __key, const std::array<byte_t, 8>& text_block) {
    std::array<byte_t, 4> B = {
        text_block[0],
        text_block[1],
        text_block[2],
        text_block[3],
    };
    std::array<byte_t, 4> A = {
        text_block[4],
        text_block[5],
        text_block[6],
        text_block[7],
    };
    for (size_t key, i = 0; i < 32; ++i) {
        if (_key == __key)
            key = __key.at(i < 24 ? i % 8 : 31 - i);
        else
            key = __key.at(i < 8 ? 7 - i : i % 8);
        uint32_t B_bits = _blockToBits<uint32_t>(B) ^ _f(A, key);
        B = A;
        A = _bitsToBlock<uint32_t, A.size()>(B_bits);
    }
    std::array<byte_t, 8> output = {
        A[0], A[1], A[2], A[3],
        B[0], B[1], B[2], B[3],
    };
    return output;
};

// Utils

void Magma::encrypt(Method method, std::istream& is, std::ostream& os) {
    _stream = &is;
    std::string s = _encrypt(method, _key);
    for (size_t i = 0; i < s.length(); ++i)
        os << s[i];
};

void Magma::decrypt(Method method, std::istream& is, std::ostream& os) {
    _stream = &is;
    std::string open_text;
    std::array<byte_t, 8> block;
    std::array<byte_t, 8> prev;
    std::array<byte_t, 8> cur;
    std::array<byte_t, 8> xored;
    std::array<uint32_t, 8> reversed_key = {
        _key[7], _key[6], _key[5], _key[4],
        _key[3], _key[2], _key[1], _key[0],
    };
    switch (method) {
    case Method::ECB:
        open_text = _encrypt(method, reversed_key);
        break;
    case Method::CBC:
        prev = _read();
        block = _block_cipher(reversed_key, prev);
        xored = block ^ _initialization_vector;
        open_text += _blockToString(xored);
        while (!_stream->eof()) {
            cur = _read();
            block = _block_cipher(reversed_key, cur);
            xored = block ^ prev;
            prev = cur;
            open_text += _blockToString(xored);
        }
        break;
    case Method::CFB:
        prev = _read();
        block = prev ^ _block_cipher(_key, _initialization_vector);
        open_text += _blockToString(block);
        while (!_stream->eof()) {
            block = _read();
            prev = block ^ _block_cipher(_key, prev);
            open_text += _blockToString(prev);
            prev = block;
        };
        break;
    case Method::OFB:
        block = _read();
        prev = _block_cipher(_key, _initialization_vector);
        block = block ^ prev;
        open_text += _blockToString(block);
        while (!_stream->eof()) {
            block = _read();
            prev = _block_cipher(_key, prev);
            block = block ^ prev;
            open_text += _blockToString(block);
        }
        break;
    default:
        break;
    };
    for (size_t i = 0; i < open_text.length(); ++i)
        os << open_text[i];
};

std::string Magma::_encrypt(Method method, const std::array<uint32_t, 8>& __key) {
    std::string cipher_text;
    std::array<byte_t, 8> block;
    std::array<byte_t, 8> prev;
    std::array<byte_t, 8> xored;
    switch (method) {
    case Method::ECB:
        while (!_stream->eof())
            cipher_text += _blockToString(_block_cipher(__key, _read()));
        break;
    case Method::CBC:
        block = _read();
        xored = block ^ _initialization_vector;
        prev = _block_cipher(_key, xored);
        cipher_text += _blockToString(prev);
        while (!_stream->eof()) {
            block = _read();
            xored = block ^ prev;
            prev = _block_cipher(_key, xored);
            cipher_text += _blockToString(prev);
        }
        break;
    case Method::CFB:
        block = _read();
        prev = block ^ _block_cipher(_key, _initialization_vector);
        cipher_text += _blockToString(prev);
        while (!_stream->eof()) {
            block = _read();
            prev = block ^ _block_cipher(_key, prev);
            cipher_text += _blockToString(prev);
        }
        break;
    case Method::OFB:
        block = _read();
        prev = _block_cipher(_key, _initialization_vector);
        block = block ^ prev;
        cipher_text += _blockToString(block);
        while (!_stream->eof()) {
            block = _read();
            prev = _block_cipher(_key, prev);
            block = block ^ prev;
            cipher_text += _blockToString(block);
        }
        break;
    default:
        break;
    }
    return cipher_text;
};

std::array<Magma::byte_t, 8> Magma::_read() {
    vec_byte_t v;
    while (!_stream->eof() && v.size() < 8) {
        *_stream >> std::noskipws >> v;
        _stream->peek();
    }
    while (v.size() < 8)
        v.push_back(0);
    return std::array<byte_t, 8> {
        v[0], v[1], v[2], v[3],
            v[4], v[5], v[6], v[7],
    };
}

template<size_t S>
std::string Magma::_blockToString(const std::array<byte_t, S>& text_block) {
    std::string output;
    for (size_t i = 0; i < S; ++i)
        output += text_block[i];
    return output;
};

template<typename T, size_t S>
T Magma::_blockToBits(const std::array<byte_t, S>& block) {
    T bits = 0;
    for (size_t i = 0; i < block.size(); ++i)
        bits |= static_cast<T>(block[i]) << (S - i - 1) * (sizeof(T) + S % 8);
    return bits;
};

template<typename T, size_t S>
std::array<Magma::byte_t, S> Magma::_bitsToBlock(const T& bits) {
    std::array<byte_t, S> result;
    for (size_t i = 0; i < S; ++i)
        result[i] = (bits >> 8 * (S - i - 1)) & 0xFF;
    return result;
};