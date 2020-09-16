#include <iostream>

#include "../include/codebooks.hpp"

namespace {
    // Increments a codeword by 1.
    void increment_codeword(Codeword& cw);
    
    // Print functions used for debugging.
    void print_dcb(DecodingBook const& dcb);
    void print_encoding_codebook(EncodingBook const& ecb);
}

// Gets a codebook for encoding - that is, one indexed by byte and having
// codewords as associated values.
// As parameter, takes a map of codeword lengths for each byte.
EncodingBook Codebooks::codebook_for_encoding(CodeLenMap& clm) {
    EncodingBook ecb {};
    Codeword codeword {};
    
    for (auto& n : clm) {
        auto codeword_length = n.first;
        auto symbols = n.second;
        
        while (codeword.size() < codeword_length) {
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

// Gets a codebook for decoding - that is, one indexed by codeword, and having
// the byte this codeword decodes to as an associated value.
// As a parameter, takes a map of codeword lengths for each byte.
DecodingBook Codebooks::codebook_for_decoding(CodeLenMap& clm) {
    DecodingBook dcb {};
    Codeword codeword {};
    
    for (auto& n : clm) {
        auto codeword_length = n.first;
        auto symbols = n.second;
        
        while ( codeword.size() < codeword_length ) {
            codeword.push_back(0);
        }
        
        while (!symbols.empty()) {
            auto current_symbol = symbols.top();
            symbols.pop();
            dcb.insert({codeword, current_symbol});
            increment_codeword(codeword);
        }
    }
    
    return dcb;
}



namespace {
    // Helper function for codebook creation, increments a codeword by 1.
    // So, if cw is 1001, returns 1010.
    // If cw is 111, returns 1000.
    void increment_codeword(Codeword& cw) {
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

    // Function to print out codebooks, used for debugging.
    void print_dcb(DecodingBook const& dcb) {
        for (auto& n : dcb) {
            for (auto x : n.first) {
                std::cout << x;
            }
            std::cout << " is key for " << n.second << std::endl;
        }
    }

    // Function to print out codebooks, used for debugging.
    void print_encoding_codebook(EncodingBook const& ecb) {
        for (auto & n : ecb) {
            std::cout << n.first << " - has key ";
            for (auto x : n.second) {
                std::cout << x;
            }
            std::cout << std::endl;
        }
    }
}
