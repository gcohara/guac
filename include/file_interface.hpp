#ifndef FILE_INTERFACE_H
#define FILE_INTERFACE_H
#include <filesystem>
#include "type_aliases.hpp"

namespace FileInterface {
    CharFreqHashMap get_frequencies(FilePath input_file);
    void compress_file(FilePath input_file, EncodingBook ecb);
    std::ofstream open_output_filestream(FilePath output_file);
    std::ifstream open_input_filestream(FilePath input_file);
}

#endif
