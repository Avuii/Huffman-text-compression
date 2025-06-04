#include <iostream>
#include "header.h"
#include <queue>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <bitset>
#include <cstdint>
#include <limits>

using namespace std;

// ─────────────────────────────────────────────────────────────
//  // DEFINITIONS FROM HEADER.H
// ─────────────────────────────────────────────────────────────


HuffmanNode::HuffmanNode(char c, int f)
    : ch(c), freq(f), left(nullptr), right(nullptr) {}

// Deterministic comparison operator in the priority queue
bool Compare::operator()(HuffmanNode* l, HuffmanNode* r) {
    if (l->freq == r->freq) {
        // If the frequencies are equal, we compare the ASCII characters.
        // Internal nodes ('\0') are treated as "greater" (code 127).
        char lch = (l->ch == '\0') ? 127 : l->ch;
        char rch = (r->ch == '\0') ? 127 : r->ch;
        return lch > rch;
    }
    return l->freq > r->freq;
}

// ─────────────────────────────────────────────────────────────
//     IMPLEMENTATIONS OF THE HUFFMAN CLASS METHODS
// ─────────────────────────────────────────────────────────────

void Huffman::generateCodes(HuffmanNode* root,
                            const std::string& prefix,
                            std::unordered_map<char, std::string>& codes)
{
    if (!root) return;

    // If the node is a leaf, we save the character → prefix mapping
    if (!root->left && !root->right) {
        codes[root->ch] = prefix;
    }

    generateCodes(root->left, prefix + "0", codes);
    generateCodes(root->right, prefix + "1", codes);
}

void Huffman::freeTree(HuffmanNode* root)
{
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

std::string Huffman::encode(const std::string& text,
                            const std::unordered_map<char, std::string>& codes)
{
    std::string encoded;
    encoded.reserve(text.size() * 2);

    for (char c : text) {
        auto it = codes.find(c);
        if (it == codes.end()) {
            // Brak kodu dla danego znaku → błąd
            std::cerr << "Encoding error: no code for character '"
                      << c << "' (ASCII=" << static_cast<int>(static_cast<unsigned char>(c)) << ")\n";
            return "";
        }
        encoded += it->second;
    }
    return encoded;
}

std::string Huffman::decode(const std::string& encoded, HuffmanNode* root)
{
    if (!root) {
        std::cerr << "Decode error: null Huffman tree\n";
        return "";
    }

    std::string decoded;
    decoded.reserve(encoded.size() / 2);

    HuffmanNode* current = root;
    for (char bit : encoded) {
        if (bit == '0') {
            if (!current->left) {
                std::cerr << "Decode error: invalid path for bit '0'\n";
                return "";
            }
            current = current->left;
        }
        else if (bit == '1') {
            if (!current->right) {
                std::cerr << "Decode error: invalid path for bit '1'\n";
                return "";
            }
            current = current->right;
        }
        else {
            std::cerr << "Decode error: invalid bit character '" << bit << "'; expected '0' or '1'\n";
            return "";
        }

        // If we have reached a leaf – we read the character and return to the root
        if (!current->left && !current->right) {
            decoded += current->ch;
            current = root;
        }
    }

    return decoded;
}

// ─────────────────────────────────────────────────────────────
//     HELPER FUNCTION PROTOTYPES (outside the Huffman class)
// ─────────────────────────────────────────────────────────────

// Loads the entire file (any type; even binary) into a string.
string readFileToString(const string& filename);

// Writes any string to a file in binary mode.
void writeBinaryToFile(const string& filename, const string& data);

// Saves the map (char → bit code) to a text file.
// First line: number of entries. Subsequent lines: \<ASCII\_character> <code>.
bool writeAlphabetToFile(const string& filename,
                         const unordered_map<char, string>& codes);

// Loads the "alphabet" file and builds a map from char to bitstring.
// Returns false if a format or I/O error occurs.
bool readAlphabetFromFile(const string& filename,
                          unordered_map<char, string>& codes);

// Builds the Huffman tree based on the map (char → bitstring).
// Returns nullptr if a conflict is detected in the codes (e.g., two characters with the same code).
HuffmanNode* buildTreeFromCodes(const unordered_map<char, string>& codes);

// ─────────────────────────────────────────────────────────────
//     IMPLEMENTATIONS OF HELPER FUNCTIONS
// ─────────────────────────────────────────────────────────────

string readFileToString(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Cannot open file: " << filename << "\n";
        return "";
    }

    file.seekg(0, ios::end);
    size_t size = file.tellg();
    file.seekg(0, ios::beg);

    if (size == 0) {
        file.close();
        return "";  // pusty plik
    }

    string buffer(size, '\0');
    file.read(&buffer[0], size);
    if (!file) {
        cerr << "Error while reading file: " << filename << "\n";
        file.close();
        return "";
    }

    file.close();
    return buffer;
}

void writeBinaryToFile(const string& filename, const string& data) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Cannot open file to write: " << filename << "\n";
        return;
    }
    file.write(data.data(), data.size());
    if (!file) {
        cerr << "Error while writing to file: " << filename << "\n";
    }
    file.close();
}

bool writeAlphabetToFile(const string& filename,
                         const unordered_map<char, string>& codes)
{
    ofstream out(filename);
    if (!out.is_open()) {
        cerr << "Cannot open alphabet file to write: " << filename << "\n";
        return false;
    }

    out << codes.size() << "\n";
    for (auto& pair : codes) {
        unsigned char uc = static_cast<unsigned char>(pair.first);
        out << static_cast<int>(uc) << " " << pair.second << "\n";
        if (!out) {
            cerr << "Error while writing to alphabet file: " << filename << "\n";
            out.close();
            return false;
        }
    }

    out.close();
    return true;
}

bool readAlphabetFromFile(const string& filename,
                          unordered_map<char, string>& codes)
{
    ifstream in(filename);
    if (!in.is_open()) {
        cerr << "Cannot open alphabet file to read: " << filename << "\n";
        return false;
    }

    size_t lines;
    if (!(in >> lines)) {
        cerr << "Alphabet file format error (cannot read count): " << filename << "\n";
        in.close();
        return false;
    }

    for (size_t i = 0; i < lines; i++) {
        int asciiCode;
        string bitstr;
        if (!(in >> asciiCode >> bitstr)) {
            cerr << "Alphabet file format error at line " << (i + 2)
                 << " in file: " << filename << "\n";
            in.close();
            return false;
        }

        // Checks if the bit code consists only of '0' and '1' characters
        for (char b : bitstr) {
            if (b != '0' && b != '1') {
                cerr << "Alphabet file error: invalid bit '" << b
                     << "' for ASCII=" << asciiCode << " in " << filename << "\n";
                in.close();
                return false;
            }
        }

        if (asciiCode < 0 || asciiCode > 255) {
            cerr << "Alphabet file error: invalid ASCII code " << asciiCode
                 << " in " << filename << "\n";
            in.close();
            return false;
        }

        codes[static_cast<char>(asciiCode)] = bitstr;
    }

    in.close();
    return true;
}

HuffmanNode* buildTreeFromCodes(const unordered_map<char, string>& codes) {
    HuffmanNode* root = new HuffmanNode('\0', 0);

    for (auto& pair : codes) {
        char c = pair.first;
        const string& bitstr = pair.second;

        HuffmanNode* current = root;
        for (char bit : bitstr) {
            if (bit == '0') {
                if (!current->left)
                    current->left = new HuffmanNode('\0', 0);
                current = current->left;
            }
            else { // bit == '1'
                if (!current->right)
                    current->right = new HuffmanNode('\0', 0);
                current = current->right;
            }
        }
        // If a character (leaf) already existed at this position, then there is a conflict
        if (current->ch != '\0') {
            cerr << "Alphabet build error: duplicate code for character '"
                 << c << "'\n";
            delete root;
            return nullptr;
        }
        current->ch = c;
    }

    return root;
}

// ─────────────────────────────────────────────────────────────
//
// ─────────────────────────────────────────────────────────────

int main() {
    cout << "Enter input filename: ";
    string inputFile;
    getline(cin, inputFile);
    if (inputFile.empty()) {
        cerr << "No input filename provided.\n";
        return 1;
    }

    // Wczytanie zawartości pliku (bajtowo)
    string text = readFileToString(inputFile);
    if (text.empty()) {
        cerr << "Input file is empty or cannot be read: " << inputFile << "\n";
        return 1;
    }

    // Poprawny wybór trybu
    string answer;
    while (true) {
        cout << "Do you want encode or decode? [encode|decode]: ";
        if (!getline(cin, answer)) {
            cerr << "Error reading mode from stdin.\n";
            return 1;
        }
        if (answer == "encode" || answer == "Encode" || answer == "ENCODE" ||
            answer == "decode" || answer == "Decode" || answer == "DECODE") {
            break;
        }
        cout << "Invalid choice, please type \"encode\" or \"decode\".\n";
    }

    Huffman hf; // An object of the class used to call the methods generateCodes, freeTree, encode, decode

    if (answer == "encode" || answer == "Encode" || answer == "ENCODE") {
        // ────────────── ENCODE MODE ───────────────

        // 1) count the frequencies of characters
        unordered_map<char, int> freq;
        for (unsigned char c : text) {
            freq[static_cast<char>(c)]++;
        }

        if (freq.empty()) {
            cerr << "Nothing to encode (empty frequency map).\n";
            return 1;
        }

        // 2)  build the priority queue of leaf nodes
        priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;
        for (auto& p : freq) {
            pq.push(new HuffmanNode(p.first, p.second));
        }

        // 3)  merge nodes until only one root remains.
        while (pq.size() > 1) {
            HuffmanNode* left = pq.top(); pq.pop();
            HuffmanNode* right = pq.top(); pq.pop();

            HuffmanNode* parent = new HuffmanNode('\0', left->freq + right->freq);
            parent->left = left;
            parent->right = right;
            pq.push(parent);
        }

        HuffmanNode* root = pq.top();
        pq.pop();

        if (!root) {
            cerr << "Error building Huffman tree.\n";
            return 1;
        }

        // 4) generating the map (char → bitstring)
        unordered_map<char, string> codes;
        hf.generateCodes(root, "", codes);

        if (codes.empty()) {
            cerr << "Error: generated empty code map.\n";
            hf.freeTree(root);
            return 1;
        }

        // 5) encoding
        string encoded = hf.encode(text, codes);
        if (encoded.empty()) {
            cerr << "Encoding failed (empty result).\n";
            hf.freeTree(root);
            return 1;
        }

        // 6) write to file
        cout << "Enter output filename for encoded data: ";
        string outputFile;
        if (!getline(cin, outputFile) || outputFile.empty()) {
            cerr << "No output filename provided.\n";
            hf.freeTree(root);
            return 1;
        }
        writeBinaryToFile(outputFile, encoded);
        cout << "Encoding complete. Encoded data saved to: " << outputFile << "\n";

        // 7) write alphabet to file
        cout << "Enter filename to save alphabet (char → bitstring): ";
        string alphabetFile;
        if (!getline(cin, alphabetFile) || alphabetFile.empty()) {
            cerr << "No alphabet filename provided.\n";
            hf.freeTree(root);
            return 1;
        }
        if (!writeAlphabetToFile(alphabetFile, codes)) {
            cerr << "Failed to write alphabet to: " << alphabetFile << "\n";
            hf.freeTree(root);
            return 1;
        }
        cout << "Alphabet saved to: " << alphabetFile << "\n";

        hf.freeTree(root);
    }
    else {
        // ────────────── DECODE MODE ───────────────

        // 1) We check if "text" is a string containing only '0' and '1' characters.
        bool validBits = true;
        for (char b : text) {
            if (b != '0' && b != '1') {
                validBits = false;
                break;
            }
        }
        if (!validBits) {
            cerr << "Decode error: input file does not consist solely of bits '0'/'1'.\n";
            return 1;
        }

        // 2) We read the name of the alphabet file.
        cout << "Enter alphabet filename (char → bitstring): ";
        string alphabetFile;
        if (!getline(cin, alphabetFile) || alphabetFile.empty()) {
            cerr << "No alphabet filename provided.\n";
            return 1;
        }

        unordered_map<char, string> codes;
        if (!readAlphabetFromFile(alphabetFile, codes) || codes.empty()) {
            cerr << "Failed to read or empty alphabet from: " << alphabetFile << "\n";
            return 1;
        }

        // 3) We reconstruct the tree from the code map.
        HuffmanNode* root = buildTreeFromCodes(codes);
        if (!root) {
            cerr << "Failed to build Huffman tree from alphabet.\n";
            return 1;
        }

        // 4) decoding
        string decoded = hf.decode(text, root);
        if (decoded.empty() && !text.empty()) {
            cerr << "Decoding failed (empty result).\n";
            hf.freeTree(root);
            return 1;
        }

        cout << "Decoded string:\n" << decoded << "\n";

        // 5) write to file
        cout << "Enter output filename for decoded data: ";
        string outputFile;
        if (!getline(cin, outputFile) || outputFile.empty()) {
            cerr << "No output filename provided.\n";
            hf.freeTree(root);
            return 1;
        }
        writeBinaryToFile(outputFile, decoded);
        cout << "Decoding complete. Decoded data saved to: " << outputFile << "\n";

        hf.freeTree(root);
    }

    return 0;
}
