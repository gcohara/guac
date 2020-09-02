#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "type_alises.hpp"

namespace Huffman {
    CodeLenMap get_codeword_lengths(CharFreqHashMap const & char_frequencies);
}

#endif
