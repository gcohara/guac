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

template<typename T> void print_pq(T pq);

int main(int argc, char ** argv) {
    using namespace std::string_literals;

    FilePath input_file {argv[1]};
    FilePath output_file {argv[1] + ".guac"s};
    auto frequencies = FileInterface::get_frequencies(input_file);
    // Print them out for checking!
    for (auto n : frequencies) {
        std::cout << "Key:[" << n.first << "] Value:[" << n.second << "]\n";
    }
    auto code_lens = Huffman::get_codeword_lengths(frequencies);
    auto encoding_codebook = Codebooks::get_codebook_for_encoding(code_lens);

    //segfault after this
    FileInterface::compress_file(input_file, encoding_codebook);

}
