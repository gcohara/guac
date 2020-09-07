#ifndef CODEBOOKS_H
#define CODEBOOKS_H

#include "type_aliases.hpp"

using Codeword = std::deque<bool>;
using EncodingBook = std::unordered_map<Byte, Codeword>;
using DecodingBook = std::unordered_map<Codeword, Byte>;

namespace Codebooks {
    EncodingBook codebook_for_encoding(CodeLenMap& clm);
    DecodingBook codebook_for_decoding(CodeLenMap& clm);
}

#endif
