#ifndef HEADER_H
#define HEADER_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <vector>
#include <bitset>


// Struktura węzła drzewa Huffmana
struct HuffmanNode {
    char ch;             // symbol
    int freq;            // częstotliwość
    HuffmanNode* left;   // lewe dziecko
    HuffmanNode* right;  // prawe dziecko

    HuffmanNode(char c, int f);
};

// Porównywarka do kolejki priorytetowej (min-heap)
struct Compare { bool operator()(HuffmanNode* l, HuffmanNode* r);};

class Huffman {
public:
    // Rekurencyjnie generuje mapę kodów Huffmana (znak -> kod bitowy)
    void generateCodes(HuffmanNode* root, const std::string& prefix, std::unordered_map<char, std::string>& codes);
    // Zwalnia pamięć całego drzewa Huffmana
    void freeTree(HuffmanNode* root);


    // Koduje tekst na podstawie mapy kodów Huffmana
    std::string encode(const std::string& text, const std::unordered_map<char, std::string>& codes);

    // Dekoduje zakodowany tekst na podstawie drzewa Huffmana
    std::string decode(const std::string& encoded, HuffmanNode* root);


};

#endif // HEADER_H

