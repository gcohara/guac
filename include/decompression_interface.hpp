#ifndef DECOMPRESSION_INTERFACE
#define DECOMPRESSION_INTERFACE
#include <filesystem>
#include "type_aliases.hpp"

namespace Decompress {
    CodeLenMap codeword_lengths_from_file(FilePath compressed_file);
    void decompress_file(FilePath compressed_file, FilePath output, DecodingBook dcb);
}

#endif
