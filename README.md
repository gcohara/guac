# guac

_George's ~~Utterly Abysmal~~ Ultra Amazing Compressor_

Personal project to implement a file compressor in C++.
Builds a Huffman tree over the alphabet of all 256 bytes. The length of each symbol's codeword in this tree is then used to get a canonical Huffman encoding. The canonical encoding is then used to compress the file.
