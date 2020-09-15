#include <bitset>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <climits>
#include "../include/file_interface.hpp"

using std::ofstream,
    std::ifstream;

// Given a compressed input file, reads in the lengths of the codeword for each
// byte.
CodeLenMap FileInterface::codeword_lengths_from_file(FilePath input) {
    auto ifs = FileInterface::open_input_filestream(input);
    CodeLenMap clm {};
    char cw_lens[256];
    
    ifs.read(cw_lens, 256);
    
    for (Byte i = 0; i < UCHAR_MAX; i++) {
        // ignore zeros, these are bytes that aren't present in original file
        if (cw_lens[i] == 0) {
            continue;
        }
        clm.try_emplace(cw_lens[i], CharPriorQ {});
        clm.at(cw_lens[i]).push(i);
    }
    return clm;
}


CharFreqHashMap FileInterface::character_frequencies(FilePath input) {
    auto ifs = open_input_filestream(input);
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

ofstream FileInterface::open_output_filestream(FilePath output_file) {
    ofstream ofs (output_file, std::ios::binary);
    if ( !ofs ) {
        std::cerr << "Unable to open output file.\n";
        exit(EXIT_FAILURE);
    }
    return ofs;
}
    
ifstream FileInterface::open_input_filestream(FilePath input_file) {
    ifstream ifs (input_file, std::ios::binary);
    if ( !ifs ) {
        std::cerr << "Unable to open input file.\n";
        exit(EXIT_FAILURE);
    }
    return ifs;
}


