#include "../include/decompression_interface.hpp"
#include "../include/file_interface.hpp"
#include <cstdio>
#include <fstream>
#include <ios>
#include <iostream>
#include <ostream>

using std::ifstream,
    std::ofstream;

namespace {
    BitQueue convert_buffer_to_bits(Byte * input_buffer, std::streamsize bytes_read);
    void add_byte_to_bitqueue(BitQueue& bitqueue, Byte byte);
    void decode_bitqueue(ofstream& ofs, DecodingBook dcb, BitQueue& bits);
    
}

void print_bitqueue(BitQueue bq) {
    for (bool b : bq) {
        std::cout << b;
    }
    std::cout << std::endl;
}

void Decompress::decompress_file(FilePath input, FilePath output, DecodingBook dcb) {
    auto ifs = FileInterface::open_input_filestream(input);
    auto ofs = FileInterface::open_output_filestream(output);
    // Get the 256th byte, which tells us how many bits of the penultimate byte
    // to ignore
    ifs.seekg(256);
    Byte loose_bits;
    ifs.read(reinterpret_cast<char *>(&loose_bits), 1);
    // std::cout << "Loose bits: " << static_cast<int>(loose_bits) << std::endl;
    
    // Now start reading
    Byte input_buffer[BUFSIZ];
    // BitQueue bits {};
    while (ifs) {
        ifs.read(reinterpret_cast<char *>(input_buffer), BUFSIZ);
        auto bytes_read = ifs.gcount();
        // std::cout << "Bytes read in: " << bytes_read << "\n";
        auto bits = convert_buffer_to_bits(input_buffer, bytes_read);
        // std::cout << "Which was converted to " << bits.size() << " bits\n";
        // print_bitqueue(bits);
        if (!ifs) {             // i.e if it's the last read
            // CAUTION COULD BE A BUG IF THERE IS A MULTIPLE OF 256 BYTES IN FILE
            for (unsigned i = 0; i < loose_bits; i++) {
                bits.pop_back(); // discard those loose bits
            }
        }
        decode_bitqueue(ofs, dcb, bits);
    }
    
}

CodeLenMap Decompress::codeword_lengths_from_file(FilePath input) {
    auto ifs = FileInterface::open_input_filestream(input);
    CodeLenMap clm {};
    char cw_lens[256];
    ifs.read(cw_lens, 256);
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
   
    void decode_bitqueue(std::ofstream& ofs, DecodingBook dcb, BitQueue& bits) {
        Codeword static cw {};
        while (!bits.empty()) {
            cw.push_back(bits.front());
            bits.pop_front();

            if (dcb.contains(cw)) {
                auto output_byte = dcb.at(cw);
                ofs.write(reinterpret_cast<char *>(&output_byte), 1);
                cw.clear();
            }
        }
    }
    
    BitQueue convert_buffer_to_bits(Byte * input_buffer, std::streamsize bytes_read) {
        std::deque<bool> out {};
        for (unsigned i = 0; i < bytes_read; i++) {
            add_byte_to_bitqueue(out, input_buffer[i]);
        }
        
        return out;
        
    }

    void add_byte_to_bitqueue(BitQueue& bitqueue, Byte byte) {
        for (int i = 0; i < 8; i++) {
            bitqueue.push_back(byte & 128);
            byte <<= 1;
        }
    }
}
