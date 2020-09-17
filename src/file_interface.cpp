#include <bitset>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <climits>
#include <array>
#include <iterator>
#include "../include/file_interface.hpp"

using std::ofstream,
    std::ifstream,
    std::array;

// Given a compressed input file, reads in the lengths of the codeword for each
// byte - that is, the values of the first 256 bytes.
CodeLenMap FileInterface::codeword_lengths_from_file(FilePath input) {
    auto ifs = FileInterface::open_input_filestream(input);
    CodeLenMap clm {};
    array<char, NUM_BYTES> cw_lens;
    
    ifs.read(cw_lens.data(), NUM_BYTES);
    
    for (auto itr = cw_lens.cbegin(); itr < cw_lens.cend(); itr++) {
        // ignore zeros, these are bytes that aren't present in original file
        Byte byte = itr - cw_lens.cbegin();
        auto cw_len_for_byte = *itr;
        
        if (cw_len_for_byte == 0) {
            continue;
        }

        clm.try_emplace(cw_len_for_byte, CharPriorQ {});
        clm.at(cw_len_for_byte).push(byte);
    }
    
    return clm;
}

// Given a file we want to compress, reads it in and counts the frequencies of
// each byte. 
ByteFreqHashMap FileInterface::character_frequencies(FilePath input) {
    auto ifs = open_input_filestream(input);
    ByteFreqHashMap frequencies{};
    InptBuff input_buffer;
    
    while (ifs) {
        ifs.read(input_buffer.data(), BUFSIZ);
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
    if (!ofs) {
        std::cerr << "Unable to open output file.\n";
        std::cerr << "Error:" << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    return ofs;
}
    
ifstream FileInterface::open_input_filestream(FilePath input_file) {
    ifstream ifs (input_file, std::ios::binary);
    if (!ifs) {
        std::cerr << "Unable to open input file.\n";
        std::cerr << "Error:" << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    return ifs;
}


