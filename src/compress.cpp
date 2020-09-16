#include <cstdio>
#include <deque>
#include <fstream>
#include <array>
#include <vector>

#include "../include/compress.hpp"
#include "../include/file_interface.hpp"

using std::ofstream,
    std::ifstream,
    std::deque,
    std::array;

namespace {
    deque<Byte> file_header(EncodingBook& ecb);
    void write_file_header(ofstream& ofs, EncodingBook& ecb);
    FilePath output_path(FilePath input_file);
    Byte byte_from_encoding_buffer(deque<bool>& out_buf);
    Byte write_file_end(deque<bool> encoding_buffer, ofstream &ofs);
}

void Compress::compress_file(FilePath input, FilePath output, EncodingBook ecb) {
    auto ifs = FileInterface::open_input_filestream(input);
    auto ofs = FileInterface::open_output_filestream(output);
    write_file_header(ofs, ecb);

    array<char, BUFSIZ> input_buffer;
    deque<bool> encoding_buffer{};
    // needs to be large, maximum cw size is 256 bits
    // array<char, BUFSIZ * NUM_BYTES> output_buffer;
    std::vector<char> output_buffer {};
    
    // definitely need an output buffer in this loop, would speed it up hugely
    while (ifs) {
        ifs.read(input_buffer.data(), BUFSIZ);
        auto bytes_read = ifs.gcount();
        
        for (auto iter = input_buffer.cbegin();
             iter < input_buffer.cbegin() + bytes_read;
             iter++) {
            Byte c = *iter;
            auto cw = ecb.at(c);
            // add to output buffer
            encoding_buffer.insert(encoding_buffer.end(), cw.begin(), cw.end());
        }

        // now write out what we can
        while (encoding_buffer.size() >= 8) {
            char byte_out = static_cast<char>
                (byte_from_encoding_buffer(encoding_buffer));
            output_buffer.push_back(byte_out);
            // ofs.write(&byte_out, 1);
        }
        ofs.write(output_buffer.data(), output_buffer.size());
        output_buffer.clear();
    }
    
    // now we either do or don't have some spare bits at the end
    auto spare_bits = write_file_end(encoding_buffer, ofs);

    // write spare_bits
    ofs.seekp(256);
    ofs.write(reinterpret_cast<char *>(&spare_bits), 1);
}

namespace {
    Byte write_file_end(deque<bool> encoding_buffer, ofstream &ofs) {
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

    // Gets the next byte to write out from our encoding buffer.
    Byte byte_from_encoding_buffer(deque<bool> &encoding_buffer) {
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
    
    FilePath output_path(FilePath input_file) {
        FilePath output_path {input_file};
        output_path += ".guac";
        return output_path;
    }
    
    deque<Byte> file_header(EncodingBook& ecb) {
        deque<Byte> file_header {};
        for (Byte b = 0; b < UCHAR_MAX; b++) {
            if (ecb.contains(b)) {
                file_header.push_back(ecb.at(b).size());
            }
            else {
                file_header.push_back(0);
            }
        }
        return file_header;
    }
}
