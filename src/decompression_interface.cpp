#include "../include/decompression_interface.hpp"
#include "../include/file_interface.hpp"
#include <fstream>
#include <ios>
#include <iostream>

using std::ifstream;

namespace {

}

void Decompress::decompress_file(FilePath input, DecodingBook dcb) {
    auto ifs = FileInterface::open_input_filestream(input);
    auto output_file = input.replace_extension(); //remove the ".guac"
    auto ofs = FileInterface::open_output_filestream(output_file);
    // Get the last byte, which tells us how many bits of the penultimate byte
    // to ignore
    ifs.seekg(-1, std::ios::end);
    Byte loose_bits;
    ifs.read(reinterpret_cast<char *>(&loose_bits), 1);

    
}

CodeLenMap Decompress::codeword_lengths_from_file(FilePath input) {
    auto ifs = FileInterface::open_input_filestream(input);
    CodeLenMap clm {};
    char cw_lens[255];
    ifs.read(cw_lens, 255);
    for (Byte i = 0; i < 255; i++) {
        clm.try_emplace(cw_lens[i], CharPriorQ {});
        clm.at(cw_lens[i]).push(i);
    }
    return clm;
}

namespace {

}
