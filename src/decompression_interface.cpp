#include "../include/decompression_interface.hpp"
#include "../include/file_interface.hpp"
#include <cstdio>
#include <fstream>
#include <ios>
#include <iostream>
#include <ostream>

using std::ifstream;

namespace {
    std::deque<bool> convert_buffer_to_bits(Byte * input_buffer, std::streamsize bytes_read);
    void add_byte_to_bitqueue(std::deque<bool>& bitqueue, Byte byte);
    void decode_bitqueue(std::ofstream& ofs, DecodingBook dcb, std::deque<bool> bits);
    
}

void Decompress::decompress_file(FilePath input, DecodingBook dcb) {
    auto ifs = FileInterface::open_input_filestream(input);
    auto output_file = input.replace_extension(); //remove the ".guac"
    std::cout << output_file << "\n";
    auto ofs = FileInterface::open_output_filestream(output_file);
    // Get the 255th byte, which tells us how many bits of the penultimate byte
    // to ignore
    ifs.seekg(255);
    Byte loose_bits;
    ifs.read(reinterpret_cast<char *>(&loose_bits), 1);
    // Now start reading
    Byte input_buffer[BUFSIZ];
    while (ifs) {
        ifs.read(reinterpret_cast<char *>(input_buffer), BUFSIZ);
        auto bytes_read = ifs.gcount();
        std::deque<bool> bits = convert_buffer_to_bits(input_buffer, bytes_read);
        decode_bitqueue(ofs, dcb, bits);
    }
    
}

CodeLenMap Decompress::codeword_lengths_from_file(FilePath input) {
    auto ifs = FileInterface::open_input_filestream(input);
    CodeLenMap clm {};
    char cw_lens[255];
    ifs.read(cw_lens, 255);
    for (Byte i = 0; i < 255; i++) {
        // ignore codewords with zero length
        if (cw_lens[i] == 0) {
            continue;
        }
        clm.try_emplace(cw_lens[i], CharPriorQ {});
        clm.at(cw_lens[i]).push(i);
    }
    return clm;
}

namespace {
   
    void decode_bitqueue(std::ofstream& ofs, DecodingBook dcb, std::deque<bool> bits) {
        for (auto x : bits) {
            std::cout << x;
        }
        std::cout << std::endl;
        Codeword cw {};
        while (!bits.empty()) {
            cw.push_back(bits.front());
            bits.pop_front();
            
            std::cout << "Current cw is: ";
            for (auto x : cw) {
                std::cout << x;
            }
            std::cout << std::endl;
            
            if (dcb.contains(cw)) {
                auto output_byte = dcb.at(cw);
                
                std::cout << "Writing " << static_cast<int>(output_byte);
                std::cout << " from codeword ";
                for (auto x : cw) {
                    std::cout << x;
                }
                std::cout << std::endl;
                
                ofs.write(reinterpret_cast<char *>(&output_byte), 1);
                cw.clear();
            }
            else {
                std::cout << "No match...\n";
            }
        }
    }
    
    std::deque<bool>
    convert_buffer_to_bits
    (Byte * input_buffer, std::streamsize bytes_read) {
        std::deque<bool> out {};
        for (unsigned i = 0; i < bytes_read; i++) {
            add_byte_to_bitqueue(out, input_buffer[i]);
        }
        
        return out;
        
    }

    void add_byte_to_bitqueue(std::deque<bool>& bitqueue, Byte byte) {
        for (int i = 0; i < 8; i++) {
            bitqueue.push_back(byte & 128);
            byte <<= 1;
        }
    }
}
