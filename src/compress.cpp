#include <cstdio>
#include <deque>
#include <fstream>
#include <array>
#include <vector>

#include "../include/compress.hpp"
#include "../include/file_interface.hpp"

using std::ofstream,
    std::ifstream,
    std::array;

using SpareBits = Byte;

namespace {
    // Writes the header of the file.
    // The first 256 bytes contain the length of their own codewords.
    // So, if 00001010 had a codeword length of 6, the 11th byte would be
    // 00000110.
    // Note that it is possible, but astronomically unlikely, for a codeword to
    // have length 256, in which case its value in the header would be 00000000
    // and we would have a bug.
    //
    // The 257th byte tells us how many 'spare' bits there are on the final byte,
    // in case the encoded message is not a multiple of 8 long.
    // This is initialised as 0, and filled in after the rest of the file has
    // been written.
    void write_file_header(ofstream& ofs, EncodingBook& ecb);

    // Returns the output path, if it hasn't been provided.
    // This is simply the input path with a '.guac' extension added.
    FilePath output_path(FilePath const& input_file);

    // Returns a single byte from the encoding buffer.
    // Consumes the first 8 bits in the encoding buffer.
    Byte byte_from_encoding_buffer(BitQueue& out_buf);

    // When the encoding buffer has fewer than 8 bits, writes the last byte out.
    // Returns the amount of trailing bits we have of this final byte.
    SpareBits write_file_end(BitQueue& encoding_buffer, ofstream& ofs);
}

// The main function for performing file compression.
// Reads from the input file to the output file, compressing as we do so.
// The encoding buffer is used to store the codewords before they are translated
// into bytes that can be written out.
void Compress::compress_file(FilePath input, FilePath output, EncodingBook ecb) {
    auto ifs = FileInterface::open_input_filestream(input);
    auto ofs = FileInterface::open_output_filestream(output);
    write_file_header(ofs, ecb);

    array<char, BUFSIZ> input_buffer;
    BitQueue encoding_buffer{};
    std::vector<char> output_buffer {};
    
    while (ifs) {
        ifs.read(input_buffer.data(), BUFSIZ);
        auto bytes_read = ifs.gcount();
        
        for (auto itr = input_buffer.cbegin();
             itr < input_buffer.cbegin() + bytes_read;
             itr++) {
            Byte c = *itr;
            auto codeword = ecb.at(c);
            // append the codeword onto the encoding buffer
            encoding_buffer.insert(encoding_buffer.end(),
                                   codeword.begin(),
                                   codeword.end());
        }

        // translate as many bits from the encoding buffer into bytes as we can,
        // then write them out
        while (encoding_buffer.size() >= 8) {
            Byte byte_out = byte_from_encoding_buffer(encoding_buffer);
            output_buffer.push_back(byte_out);
        }
        ofs.write(output_buffer.data(), output_buffer.size());
        output_buffer.clear();
    }
    
    // write out the final byte, and get the number of spare bits
    auto spare_bits = write_file_end(encoding_buffer, ofs);

    // write spare bits to the 257th byte in the file
    ofs.seekp(256);
    ofs.write(reinterpret_cast<char *>(&spare_bits), 1);
}

namespace {
    // Writes the final byte, and gets the number of trailing bits as it does
    // so.
    // Only writes the byte if there actually is one!
    SpareBits write_file_end(BitQueue& encoding_buffer, ofstream &ofs) {
        Byte unused_bits = 0;
        
        if (encoding_buffer.size() != 0) {
            unused_bits = 8;
            Byte final_byte = 0;
            
            while (encoding_buffer.size() != 0) {
              final_byte <<= 1;
              final_byte += encoding_buffer.front();
              encoding_buffer.pop_front();
              unused_bits--;
            }
            
            final_byte <<= unused_bits;
            ofs.write(reinterpret_cast<char *>(&final_byte), 1);
        }
        
        return unused_bits;
    }

    // Gets a byte from the encoding buffer, consuming 8 bits as it does so.
    // Does this using bitshifts.
    // Assumes the encoding buffer is at least 8 bits long.
    Byte byte_from_encoding_buffer(BitQueue& encoding_buffer) {
        Byte output = 0;
        for (int i = 0; i < 8; i++) {
            output <<= 1;
            output += encoding_buffer.front();
            encoding_buffer.pop_front();
        }
        return output;
    }

    // Writes out 257 bytes - the first 256 are the codeword lengths for
    // each symbol in our alphabet.
    // The 257th bite is the number of trailing bits to ignore at the end of
    // the file.
    void write_file_header(ofstream& ofs, EncodingBook& ecb) {
        Byte file_header[257] = {0};
        
        for (auto& n : ecb) {
            auto byte = n.first;
            file_header[byte] = n.second.size();
        }
        
        ofs.write(reinterpret_cast<char *>(file_header), 257);
    }

    // Gets the output path. Makes a copy of the input path and appends ".guac"
    FilePath output_path(FilePath const& input_file) {
        FilePath output_path {input_file};
        output_path += ".guac";
        return output_path;
    }
}
