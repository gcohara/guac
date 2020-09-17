#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "type_aliases.hpp"

namespace Huffman {
    CodeLenMap get_codeword_lengths(ByteFreqHashMap const & char_frequencies);
}

#endif
