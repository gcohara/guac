#ifndef TYPE_ALIASES_H
#define TYPE_ALIASES_H

#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <queue>
#include <map>


using Byte = std::uint8_t;
using CharFreqHashMap = std::unordered_map<Byte, long long int>;
using CharPriorQ = std::priority_queue<Byte, std::vector<Byte>, std::greater<Byte>>;
using CodeLenMap = std::map<int, CharPriorQ>;
using Codeword = std::deque<bool>;
using EncodingBook = std::unordered_map<Byte, Codeword>;
using DecodingBook = std::unordered_map<Codeword, Byte>;
using EncodedMessage = std::deque<bool>;
using ByteVec = std::vector<Byte>;
using ByteDeq = std::deque<Byte>;
using FilePath = std::filesystem::path;
struct Node;



#endif
