#ifndef DECOMPRESS_H
#define DECOMPRESS_H
#include <filesystem>
#include "type_aliases.hpp"

namespace Decompress {

    void decompress_file(FilePath compressed_file, FilePath output, DecodingBook dcb);
}

#endif
