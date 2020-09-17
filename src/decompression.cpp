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
    // Converts the bytes in the input buffer into bits and places them in a
    // bitqueue.
    BitQueue convert_buffer_to_bits(InptBuff const& input_buffer, std::streamsize bytes_read);
    // Converts a byte into bits, and appends it to a bitqueue.
    void add_byte_to_bitqueue(BitQueue& bitqueue, Byte byte);
    // Decodes as much of the bitqueue as it can, and adds the decoded characters
    // to the output buffer. Consumes the bitqueue as it does so.
    // Uses a static Codeword variable that persists between calls, since codes
    // can cross over different reads into the input buffer.
    void decode_bitqueue(OutptBuff& ob, DecodingBook const& dcb, BitQueue& bits);
}

// Main function for performing file decompression.
// First gets the number of bits of the final byte that should be ignored, then
// reads from input to output, decompressing as it goes.
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
    OutptBuff output_buffer {};
    

    while (ifs) {
        ifs.read(input_buffer.data(), BUFSIZ);
        auto bytes_read = ifs.gcount();
        auto bits = convert_buffer_to_bits(input_buffer, bytes_read);
        
        if (!ifs) {             // i.e if it's the last read
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
