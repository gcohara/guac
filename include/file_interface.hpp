#ifndef FILE_INTERFACE_H
#define FILE_INTERFACE_H
#include <filesystem>
#include "type_aliases.hpp"

namespace FileInterface {
    ByteFreqHashMap character_frequencies(FilePath input_file);
    CodeLenMap codeword_lengths_from_file(FilePath compressed_file);
    std::ofstream open_output_filestream(FilePath output_file);
    std::ifstream open_input_filestream(FilePath input_file);
}

#endif
