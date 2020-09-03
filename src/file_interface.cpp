#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include "../include/file_interface.hpp"

CharFreqHashMap FileInterface::get_frequencies(FilePath input_file) {
    std::ifstream ifs (input_file, std::ios::binary);
    if ( !ifs ) {
        std::cerr << "Unable to open input file.\n";
        exit(EXIT_FAILURE);
    }
    CharFreqHashMap frequencies{};
    char input_buffer[BUFSIZ];
    while (ifs) {
        ifs.read(input_buffer, BUFSIZ);
        auto bytes_read = ifs.gcount();
        for (int i = 0; i < bytes_read; i++) {
            Byte c = input_buffer[i];
            frequencies.try_emplace(c, 0);
            frequencies.at(c)++;
        }
    }
    return frequencies;
}
