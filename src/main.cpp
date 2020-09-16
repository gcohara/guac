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

void print_pq(CharPriorQ pq);
void print_cw_lens(CodeLenMap clm);

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
        // Print them out for checking!

        auto code_lens = Huffman::get_codeword_lengths(frequencies);
        // print_cw_lens(code_lens);
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
}

void print_cw_lens(CodeLenMap clm) {
    for (auto& n : clm) {
        std::cout << "Of length " << n.first << ":\n";
        print_pq(n.second);
    }
}

void print_pq(CharPriorQ pq){
    while (!pq.empty()) {
        std::cout << static_cast<int>(pq.top()) << ", ";
        pq.pop();
    }
    std::cout << "\n";
}

void print_freqs(CharFreqHashMap cfm) {
    for (auto n : cfm) {
        std::cout << "Key:[" << static_cast<int>(n.first) << "] Value:["
                  << n.second << "]\n";
    }
}
