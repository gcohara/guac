#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "type_aliases.hpp"

namespace Huffman {
    // Given the frequencies of each character in the input file, returns a map
    // where the keys are characters, and the value is the length of the character's
    // Huffman codeword.
    CodeLenMap get_codeword_lengths(ByteFreqHashMap const & char_frequencies);
}

#endif
