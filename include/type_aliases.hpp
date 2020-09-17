#ifndef TYPE_ALIASES_H
#define TYPE_ALIASES_H

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <unordered_map>
#include <queue>
#include <map>
#include <array>

#define NUM_BYTES 256

using Codeword = std::deque<bool>;
struct CwHash;
using Byte = std::uint8_t;
using ByteFreqHashMap = std::unordered_map<Byte, long long int>;
using CharPriorQ = std::priority_queue<Byte, std::vector<Byte>, std::greater<Byte>>;
using CodeLenMap = std::map<int, CharPriorQ>;
using EncodingBook = std::unordered_map<Byte, Codeword>;
using DecodingBook = std::unordered_map<Codeword, Byte, CwHash>;
using EncodedMessage = std::deque<bool>;
using ByteVec = std::vector<Byte>;
using ByteDeq = std::deque<Byte>;
using BitQueue = std::deque<bool>;
using FilePath = std::filesystem::path;
using InptBuff = std::array<char, BUFSIZ>;
using OutptBuff = std::vector<char>;
struct Node;

struct CwHash {
    std::size_t operator()(Codeword const& k) const {
        std::size_t output = 0;
        for (auto x : k) {
            output += x;
        }
        return output;
    }
};

#endif
