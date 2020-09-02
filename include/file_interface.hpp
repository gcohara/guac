#ifndef FILE_INTERFACE_H
#define FILE_INTERFACE_H
#include <filesystem>
#include "type_alises.hpp"

namespace FileInterface {
    CharFreqHashMap get_frequencies(FilePath input_file);
}

#endif
