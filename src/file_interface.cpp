#include <bitset>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <climits>
#include "../include/file_interface.hpp"

using std::deque,
    std::ofstream,
    std::ifstream;

namespace {
    deque<Byte> get_file_header(EncodingBook& ecb);
    void write_file_header(ofstream& ofs, EncodingBook& ecb);
    FilePath get_output_path(FilePath input_file);
    Byte byte_from_encoding_buffer(deque<bool>& out_buf);
    Byte write_file_end(deque<bool> encoding_buffer, ofstream &ofs);
}

void FileInterface::compress_file(FilePath input, FilePath output, EncodingBook ecb) {
    auto ifs = open_input_filestream(input);
    auto ofs = open_output_filestream(output);
    write_file_header(ofs, ecb);

    char input_buffer[BUFSIZ];
    // bool output_buffer[8 * BUFSIZ + 1];
    deque<bool> encoding_buffer{};
    while (ifs) {
        ifs.read(input_buffer, BUFSIZ);
        auto bytes_read = ifs.gcount();
        // std::cout << "We read " << bytes_read << " bytes\n";
        for (int i = 0; i < bytes_read; i++) {
            Byte c = input_buffer[i];
            auto cw = ecb.at(c);
            // add to output buffer
            encoding_buffer.insert(encoding_buffer.end(), cw.begin(), cw.end());
        }

        // now write out what we can
        while (encoding_buffer.size() >= 8) {
            char byte_out = static_cast<char>
                (byte_from_encoding_buffer(encoding_buffer));
            ofs.write(&byte_out, 1);
        }
    }
    
    // now we either do or don't have some spare bits at the end
    auto spare_bits = write_file_end(encoding_buffer, ofs);
    // std::cout << "Spare bits: " << static_cast<int>(spare_bits) << std::endl;
    // write spare_bits
    ofs.seekp(256);
    ofs.write(reinterpret_cast<char *>(&spare_bits), 1);
}

CharFreqHashMap FileInterface::get_frequencies(FilePath input_file) {
    auto ifs = open_input_filestream(input_file);
    CharFreqHashMap frequencies{};
    char input_buffer[BUFSIZ];
    while (ifs) {
        ifs.read(input_buffer, BUFSIZ);
        auto bytes_read = ifs.gcount();
        for (int i = 0; i < bytes_read; i++) {
            Byte c = input_buffer[i];
            frequencies.try_emplace(c, 0);
            frequencies.at(c)++;
        }
    }
    return frequencies;
}

ofstream FileInterface::open_output_filestream(FilePath output_file) {
    ofstream ofs (output_file, std::ios::binary);
    if ( !ofs ) {
        std::cerr << "Unable to open output file.\n";
        exit(EXIT_FAILURE);
    }
    return ofs;
}
    
ifstream FileInterface::open_input_filestream(FilePath input_file) {
    ifstream ifs (input_file, std::ios::binary);
    if ( !ifs ) {
        std::cerr << "Unable to open input file.\n";
        exit(EXIT_FAILURE);
    }
    return ifs;
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

    Byte byte_from_encoding_buffer(deque<bool>& out_buf) {
        Byte output = 0;
        for (int i = 0; i < 8; i++) {
            output <<= 1;
            output += out_buf.front();
            out_buf.pop_front();
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
    
    FilePath get_output_path(FilePath input_file) {
        FilePath output_path {input_file};
        output_path += ".guac";
        return output_path;
    }
    

    
    deque<Byte> get_file_header(EncodingBook& ecb) {
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
