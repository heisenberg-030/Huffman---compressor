#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>

using namespace std;

// Node structure for Huffman Tree
struct Node {
    char data;
    Node* left;
    Node* right;
    
    Node(char d = '\0') : data(d), left(nullptr), right(nullptr) {}
};

// Function to deserialize the tree structure
Node* deserializeTree(const string& serialized, int& index) {
    if (index >= serialized.length()) return nullptr;
    
    if (serialized[index] == '1') {
        // Leaf node
        index++;
        if (index >= serialized.length()) return nullptr;
        Node* node = new Node(serialized[index]);
        index++;
        return node;
    } else {
        // Internal node
        index++;
        Node* node = new Node();
        node->left = deserializeTree(serialized, index);
        node->right = deserializeTree(serialized, index);
        return node;
    }
}

// Function to convert bytes to binary string
string bytesToBinaryString(const vector<unsigned char>& bytes, int actualBits) {
    string binary = "";
    for (unsigned char byte : bytes) {
        binary += bitset<8>(byte).to_string();
    }
    // Return only the actual encoded bits
    return binary.substr(0, actualBits);
}

// Function to decode the encoded text using Huffman tree
string decodeText(Node* root, const string& encodedText) {
    if (!root) return "";
    
    string decodedText = "";
    Node* current = root;
    
    // Handle single character case
    if (!root->left && !root->right) {
        for (int i = 0; i < encodedText.length(); i++) {
            decodedText += root->data;
        }
        return decodedText;
    }
    
    for (char bit : encodedText) {
        if (bit == '0') {
            current = current->left;
        } else {
            current = current->right;
        }
        
        if (!current->left && !current->right) {
            // Reached a leaf node
            decodedText += current->data;
            current = root;
        }
    }
    
    return decodedText;
}

int main(int argc, char* argv[]) {
    string inputFilePath, outputFilePath;

if (argc < 2) {
    cout << "Usage: ./decoder <compressed_file (.huf) >" << endl;
    return 1;
}
inputFilePath = argv[1];
    
    // Open compressed file
    ifstream inputFile(inputFilePath, ios::binary);
    if (!inputFile.is_open()) {
        cerr << "Error: Could not open compressed file: " << inputFilePath << endl;
        return 1;
    }
    
    // Read header information
    uint32_t originalSize, treeSize, encodedSize;
    inputFile.read(reinterpret_cast<char*>(&originalSize), sizeof(originalSize));
    inputFile.read(reinterpret_cast<char*>(&treeSize), sizeof(treeSize));
    inputFile.read(reinterpret_cast<char*>(&encodedSize), sizeof(encodedSize));
    
    cout << "Reading compressed file..." << endl;
    cout << "Original size: " << originalSize << " characters" << endl;
    cout << "Tree size: " << treeSize << " characters" << endl;
    cout << "Encoded size: " << encodedSize << " bits" << endl;
    
    // Read serialized tree
    string serializedTree(treeSize, '\0');
    inputFile.read(&serializedTree[0], treeSize);
    
    // Read encoded data
    int bytesToRead = (encodedSize + 7) / 8; // Round up to nearest byte
    vector<unsigned char> encodedBytes(bytesToRead);
    inputFile.read(reinterpret_cast<char*>(encodedBytes.data()), bytesToRead);
    
    inputFile.close();
    
    // Deserialize the tree
    int index = 0;
    Node* root = deserializeTree(serializedTree, index);
    
    if (!root) {
        cerr << "Error: Failed to deserialize Huffman tree!" << endl;
        return 1;
    }
    
    // Convert bytes to binary string
    string encodedText = bytesToBinaryString(encodedBytes, encodedSize);
    
    // Decode the text
    string decodedText = decodeText(root, encodedText);
    
    if (decodedText.length() != originalSize) {
        cerr << "Warning: Decoded text size (" << decodedText.length() 
             << ") doesn't match original size (" << originalSize << ")" << endl;
    }
    
    // Generate output file path
    size_t lastDot = inputFilePath.find_last_of('.');
    if (lastDot != string::npos) {
        outputFilePath = inputFilePath.substr(0, lastDot) + "_decompressed.txt";
    } else {
        outputFilePath = inputFilePath + "_decompressed.txt";
    }
    
    // Write decoded text to output file
    ofstream outputFile(outputFilePath);
    if (!outputFile.is_open()) {
        cerr << "Error: Could not create output file: " << outputFilePath << endl;
        return 1;
    }
    
    outputFile << decodedText;
    outputFile.close();
    
    cout << "\nFile successfully decompressed!" << endl;
    cout << "Decompressed file saved at: " << outputFilePath << endl;
    cout << "Decoded text size: " << decodedText.length() << " characters" << endl;
    
    // Display first 200 characters of decoded text for verification
    cout << "\nFirst 200 characters of decoded text:" << endl;
    cout << "\"" << decodedText.substr(0, min(200, (int)decodedText.length())) << "\"" << endl;
    if (decodedText.length() > 200) {
        cout << "... (text truncated for display)" << endl;
    }
    
    return 0;
}
