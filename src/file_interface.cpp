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
    // why can't this be unsigned?
    char input_buffer[BUFSIZ];
    ifs.read(input_buffer, BUFSIZ);
    
}
