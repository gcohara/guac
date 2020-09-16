#include "../include/decompression.hpp"
#include "../include/file_interface.hpp"
#include <cstdio>
#include <fstream>
#include <ios>
#include <iostream>
#include <ostream>
#include <array>

using std::ifstream,
    std::ofstream;

namespace {
    BitQueue convert_buffer_to_bits(InptBuff const& input_buffer, std::streamsize bytes_read);
    void add_byte_to_bitqueue(BitQueue& bitqueue, Byte byte);
    void decode_bitqueue(OutptBuff& ob, DecodingBook const& dcb, BitQueue& bits);
}

void Decompress::decompress_file(FilePath input, FilePath output, DecodingBook dcb) {
    auto ifs = FileInterface::open_input_filestream(input);
    auto ofs = FileInterface::open_output_filestream(output);
    
    // Get the 256th byte, which tells us how many bits of the penultimate byte
    // to ignore
    ifs.seekg(256);
    Byte loose_bits;
    ifs.read(reinterpret_cast<char *>(&loose_bits), 1);
    
    // Now start reading
    InptBuff input_buffer;
    std::vector<char> output_buffer {};

    while (ifs) {
        ifs.read(input_buffer.data(), BUFSIZ);
        auto bytes_read = ifs.gcount();
        auto bits = convert_buffer_to_bits(input_buffer, bytes_read);
        
        if (!ifs) {             // i.e if it's the last read
            // CAUTION COULD BE A BUG IF THERE IS A MULTIPLE OF 256 BYTES IN FILE
            for (unsigned i = 0; i < loose_bits; i++) {
                bits.pop_back(); // discard those loose bits
            }
        }
        
        decode_bitqueue(output_buffer, dcb, bits);
        ofs.write(output_buffer.data(), output_buffer.size());
        output_buffer.clear();
    }
    
}

namespace {
   
    void decode_bitqueue(OutptBuff& ob, DecodingBook const& dcb, BitQueue& bits) {
        Codeword static cw {};
        while (!bits.empty()) {
            cw.push_back(bits.front());
            bits.pop_front();

            if (dcb.contains(cw)) {
                ob.push_back(dcb.at(cw));
                cw.clear();
            }
        }
    }
    
    BitQueue convert_buffer_to_bits(InptBuff const& ib, std::streamsize bytes_read) {
        BitQueue out {};
        
        for (unsigned i = 0; i < bytes_read; i++) {
            add_byte_to_bitqueue(out, ib[i]);
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
