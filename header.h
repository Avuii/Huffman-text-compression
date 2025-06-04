#ifndef HEADER_H
#define HEADER_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <vector>
#include <bitset>


// Huffman tree node structure
struct HuffmanNode {
    char ch;             // symbol
    int freq;            // frequency
    HuffmanNode* left;   // left child
    HuffmanNode* right;  // right child

    HuffmanNode(char c, int f);
};

// Comparator for the priority queue (min-heap)
struct Compare { bool operator()(HuffmanNode* l, HuffmanNode* r);};

class Huffman {
public:
    // Recursively generates the Huffman code map (character -> bit code)
    void generateCodes(HuffmanNode* root, const std::string& prefix, std::unordered_map<char, std::string>& codes);

    void freeTree(HuffmanNode* root); // Frees all nodes in the Huffman tree starting from root

    std::string encode(const std::string& text, const std::unordered_map<char, std::string>& codes);
    std::string decode(const std::string& encoded, HuffmanNode* root);


};

#endif // HEADER_H

