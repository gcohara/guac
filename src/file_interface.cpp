#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <climits>
#include "../include/file_interface.hpp"

namespace {
    std::deque<Byte> get_file_header(EncodingBook& ecb);
    void write_file_header(std::ofstream& ofs, EncodingBook& ecb);
    FilePath get_output_path(FilePath input_file);
    std::ofstream open_output_filestream(FilePath output_file);
    std::ifstream open_input_filestream(FilePath input_file);
    Byte byte_from_encoding_buffer(std::deque<bool>& out_buf);
    void write_file_footer(std::deque<bool> encoding_buffer, std::ofstream& ofs);
    
}

void FileInterface::compress_file(FilePath input, EncodingBook ecb) {
    auto ifs = open_input_filestream(input);
    auto output_file = get_output_path(input);
    auto ofs = open_output_filestream(output_file);
    write_file_header(ofs, ecb);

    char input_buffer[BUFSIZ];
    // bool output_buffer[8 * BUFSIZ + 1];
    std::deque<bool> encoding_buffer{};
    while (ifs) {
        ifs.read(input_buffer, BUFSIZ);
        auto bytes_read = ifs.gcount();
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
    write_file_footer(encoding_buffer, ofs);
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

namespace {
    void write_file_footer(std::deque<bool> encoding_buffer, std::ofstream& ofs) {
        
        if (encoding_buffer.size() != 0) {
            ofs.write(0, 1);
        }
        else {
            Byte unused_bits = 8;
            Byte penultimate_byte = 0;
            while (encoding_buffer.size() != 0) {
                penultimate_byte <<= 1;
                penultimate_byte += encoding_buffer.front();
                encoding_buffer.pop_front();
                unused_bits--;
            }
            penultimate_byte <<= unused_bits;
            char out[2] = {
                static_cast<char>(penultimate_byte),
                static_cast<char>(unused_bits)
            };
            ofs.write(out, 2);
        }
    }
    
    Byte byte_from_encoding_buffer(std::deque<bool>& out_buf) {
        Byte output = 0;
        for (int i = 0; i < 8; i++) {
            output <<= 1;
            output += out_buf.front();
            out_buf.pop_front();
        }
        return output;
    }
    
    void write_file_header(std::ofstream& ofs, EncodingBook& ecb) {
        Byte file_header[256] = {0};
        for (auto& n : ecb) {
            auto byte = n.first;
            file_header[byte] = n.second.size();
        }
        ofs.write(reinterpret_cast<char *>(file_header), 256);
    }
    
    FilePath get_output_path(FilePath input_file) {
        FilePath output_path {input_file};
        output_path += ".guac";
        return output_path;
    }
    
    std::ofstream open_output_filestream(FilePath output_file) {
        std::ofstream ofs (output_file, std::ios::binary);
        if ( !ofs ) {
            std::cerr << "Unable to open output file.\n";
            exit(EXIT_FAILURE);
        }
        return ofs;
    }
    
    std::ifstream open_input_filestream(FilePath input_file) {
        std::ifstream ifs (input_file, std::ios::binary);
        if ( !ifs ) {
            std::cerr << "Unable to open input file.\n";
            exit(EXIT_FAILURE);
        }
        return ifs;
    }
    
    std::deque<Byte> get_file_header(EncodingBook& ecb) {
        std::deque<Byte> file_header {};
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
