#ifndef CODEBOOKS_H
#define CODEBOOKS_H

#include "type_alises.hpp"

using Codeword = std::deque<bool>;
using EncodingBook = std::unordered_map<Byte, Codeword>;
using DecodingBook = std::unordered_map<Codeword, Byte>;

namespace Codebooks {
    EncodingBook get_codebook_for_encoding(CodeLenMap& clm);
}

#endif
