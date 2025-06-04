# Huffman Encoder/Decoder

A C++ project implementing lossless data compression and decompression using the Huffman coding algorithm. This command-line tool reads user input from the terminal to determine the operation mode and file names, then processes the data accordingly.

## Overview

The program supports two interactive modes:

- **encode** – Compresses input text by building a Huffman tree, generating bitstring encodings, saving the encoded output, and writing the code map (alphabet) to a file.
- **decode** – Decompresses a binary-encoded bitstring using a saved alphabet file to reconstruct the original content.

### All interactions are performed via terminal prompts:
- You enter:
  - The input filename
  - Whether to `encode` or `decode`
  - Output filename
  - (and in encode mode) a filename to save the generated alphabet

  ## Example – Encoding Mode
  Enter input filename: input.txt
  Do you want encode or decode? [encode|decode]: encode
  Enter output filename for encoded data: encoded.bin
  Enter filename to save alphabet (char→bitstring): alphabet.txt
  
  ## Example – Decoding Mode
  Enter input filename: encoded.bin
  Do you want encode or decode? [encode|decode]: decode
  Enter alphabet filename (char→bitstring): alphabet.txt
  Enter output filename for decoded data: decoded.txt


## Prerequisites
A C++17 (or later) compatible compiler
Standard C++ libraries
Building
Compile the decoder using your preferred compiler. For example, using g++:

---
### Author
Created for educational purposes by Avui.
