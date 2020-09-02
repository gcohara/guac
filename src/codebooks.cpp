#include <iostream>

#include "../include/codebooks.hpp"

namespace {
    void print_encoding_codebook(EncodingBook ecb);
    void increment_codeword(Codeword & cw);
}

EncodingBook Codebooks::get_codebook_for_encoding(CodeLenMap& clm) {
    EncodingBook ecb {};
    Codeword codeword {};
    for ( auto& n : clm ) {
        auto codeword_length = n.first;
        auto symbols = n.second;
        while ( codeword.size() < codeword_length ) {
            codeword.push_back(0);
        }
        while (!symbols.empty()) {
            auto current_symbol = symbols.top();
            symbols.pop();
            ecb.insert({current_symbol, codeword});
            increment_codeword(codeword);
        }
    }
    return ecb;
}

namespace {
    void print_encoding_codebook(EncodingBook ecb) {
        for (auto & n : ecb) {
            std::cout << n.first << " - ";
            for (auto x : n.second) {
                std::cout << x;
            }
            std::cout << std::endl;
        }
    }
    

    void increment_codeword(Codeword & cw) {
        if (cw.size() == 0) {
            cw.push_back(0);
            return;
        }
        for (auto itr = cw.rbegin(); itr != cw.rend(); ++itr) {
            if (*itr == 0) {
                *itr = 1;
                return;
            }
            else {
                *itr = 0;
            }
        }
        cw.push_front(1);
    }

}
