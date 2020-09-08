#ifndef CODEBOOKS_H
#define CODEBOOKS_H

#include "type_aliases.hpp"

namespace Codebooks {
    EncodingBook codebook_for_encoding(CodeLenMap& clm);
    DecodingBook codebook_for_decoding(CodeLenMap& clm);
}

#endif
