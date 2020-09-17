#include <algorithm>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <deque>
#include <functional>
#include <iterator>
#include <ostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <map>
#include <vector>
#include <cstddef>
#include <iostream>
#include <fstream>

#include "../include/huffmantree.hpp"
#include "../include/codebooks.hpp"
#include "../include/file_interface.hpp"
#include "../include/decompression.hpp"
#include "../include/compress.hpp"

int main(int argc, char ** argv) {
    // logic needs moving out and into a 'run' function
    using namespace std::string_literals;

    if (argv[1] == "--compress"s) {
        FilePath input_file {argv[2]};
        FilePath output_file;
        if (argc < 4) {
            output_file = {argv[2] + ".guac"s};
        }
        else {
            output_file = {argv[3]};
        }
        
        std::cout << "Compressing file: \"" << input_file
                  <<"\" to output file \"" << output_file
                  << "\"" << std::endl;
        
        auto frequencies = FileInterface::character_frequencies(input_file);
        auto code_lens = Huffman::get_codeword_lengths(frequencies);
        auto encoding_codebook = Codebooks::codebook_for_encoding(code_lens);
        
        Compress::compress_file(input_file, output_file, encoding_codebook);
    }
    else if (argv[1] == "--decompress"s) {
        FilePath input_file {argv[2]};
        FilePath output_file;
        if (argc < 4) {
            output_file = input_file;
            output_file = output_file.replace_extension(); // remove the .guac
        }
        else {
            output_file = {argv[3]};
        }
        
        std::cout << "Decompressing file: \"" << input_file
                  <<"\" to output file \"" << output_file
                  << "\"" << std::endl;
        
        auto code_lens = FileInterface::codeword_lengths_from_file(input_file);
        auto decoding_codebook = Codebooks::codebook_for_decoding(code_lens);
        
        Decompress::decompress_file(input_file, output_file, decoding_codebook);
    }
    else {
        std::cerr << "Invalid option! Please provide either '--compress' or\
            '--decompress' as the first argument.\n";
        exit(EXIT_FAILURE);
    }
}
