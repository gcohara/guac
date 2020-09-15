#ifndef COMPRESS_H
#define COMPRESS_H

#include "file_interface.hpp"
#include "type_aliases.hpp"

namespace Compress {
    void compress_file(FilePath in, FilePath out, EncodingBook encode_book);
}

#endif    
