#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <string>
#include <vector>
#include <bitset>

using namespace std;

// Node structure for Huffman Tree
struct Node {
    char data;
    int freq;
    Node* left;
    Node* right;
    
    Node(char d, int f) : data(d), freq(f), left(nullptr), right(nullptr) {}
    Node(int f) : data('\0'), freq(f), left(nullptr), right(nullptr) {}
};

// Comparator for priority queue (min-heap)
struct Compare {
    bool operator()(Node* a, Node* b) {
        if (a->freq == b->freq) {
            return a->data > b->data; // For consistent ordering
        }
        return a->freq > b->freq;
    }
};

// Function to calculate frequency of characters
unordered_map<char, int> calculateFrequency(const string& text) {
    unordered_map<char, int> freq;
    for (char c : text) {
        freq[c]++;
    }
    return freq;
}

// Function to build Huffman Tree
Node* buildHuffmanTree(const unordered_map<char, int>& freq) {
    priority_queue<Node*, vector<Node*>, Compare> pq;
    
    // Create leaf nodes and add to priority queue
    for (const auto& pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }
    
    // Build the tree
    while (pq.size() > 1) {
        Node* right = pq.top(); pq.pop();
        Node* left = pq.top(); pq.pop();
        
        Node* merged = new Node(left->freq + right->freq);
        merged->left = left;
        merged->right = right;
        
        pq.push(merged);
    }
    
    return pq.top();
}

// Function to generate Huffman codes
void generateCodes(Node* root, string code, unordered_map<char, string>& codes) {
    if (!root) return;
    
    if (root->data != '\0') {
        codes[root->data] = code.empty() ? "0" : code; // Handle single character case
        return;
    }
    
    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

// Function to serialize the tree structure
void serializeTree(Node* root, string& serialized) {
    if (!root) {
        serialized += "0";
        return;
    }
    
    if (root->data != '\0') {
        serialized += "1";
        serialized += root->data;
        return;
    }
    
    serialized += "0";
    serializeTree(root->left, serialized);
    serializeTree(root->right, serialized);
}

// Function to convert binary string to bytes
vector<unsigned char> binaryStringToBytes(const string& binary) {
    vector<unsigned char> bytes;
    int padding = (8 - binary.length() % 8) % 8;
    string paddedBinary = binary + string(padding, '0');
    
    for (size_t i = 0; i < paddedBinary.length(); i += 8) {
        string byte = paddedBinary.substr(i, 8);
        bytes.push_back(static_cast<unsigned char>(bitset<8>(byte).to_ulong()));
    }
    
    return bytes;
}

int main(int argc, char* argv[]) {
    string inputFilePath, outputFilePath;

    if (argc < 2) {
        cout << "Usage: ./encoder <input_file>" << endl;
        return 1;
    }
    inputFilePath = argv[1];
    
    // Read input file
    ifstream inputFile(inputFilePath);
    if (!inputFile.is_open()) {
        cerr << "Error: Could not open input file: " << inputFilePath << endl;
        return 1;
    }
    
    string text, line;
    while (getline(inputFile, line)) {
        text += line + "\n";
    }
    inputFile.close();
    
    if (text.empty()) {
        cerr << "Error: Input file is empty!" << endl;
        return 1;
    }
    
    // Remove the last newline if it exists
    if (!text.empty() && text.back() == '\n') {
        text.pop_back();
    }
    
    cout << "Original text size: " << text.length() << " characters" << endl;
    
    // Calculate frequency
    unordered_map<char, int> freq = calculateFrequency(text);
    
    // Build Huffman Tree
    Node* root = buildHuffmanTree(freq);
    
    // Generate codes
    unordered_map<char, string> codes;
    generateCodes(root, "", codes);
    
    // Display codes
    cout << "\nHuffman Codes:" << endl;
    for (const auto& pair : codes) {
        cout << "'" << (pair.first == '\n' ? "\\n" : string(1, pair.first)) << "' : " << pair.second << endl;
    }
    
    // Encode the text
    string encodedText = "";
    for (char c : text) {
        encodedText += codes[c];
    }
    
    cout << "\nEncoded text size: " << encodedText.length() << " bits" << endl;
    cout << "Compression ratio: " << (double)encodedText.length() / (text.length() * 8) << endl;
    
    // Serialize tree structure
    string serializedTree = "";
    serializeTree(root, serializedTree);
    
    // Generate output file path
    size_t lastDot = inputFilePath.find_last_of('.');
    if (lastDot != string::npos) {
        outputFilePath = inputFilePath.substr(0, lastDot) + "_compressed.huf";
    } else {
        outputFilePath = inputFilePath + "_compressed.huf";
    }
    
    // Write to output file
    ofstream outputFile(outputFilePath, ios::binary);
    if (!outputFile.is_open()) {
        cerr << "Error: Could not create output file: " << outputFilePath << endl;
        return 1;
    }
    
    // Write header information
    uint32_t originalSize = text.length();
    uint32_t treeSize = serializedTree.length();
    uint32_t encodedSize = encodedText.length();
    
    outputFile.write(reinterpret_cast<const char*>(&originalSize), sizeof(originalSize));
    outputFile.write(reinterpret_cast<const char*>(&treeSize), sizeof(treeSize));
    outputFile.write(reinterpret_cast<const char*>(&encodedSize), sizeof(encodedSize));
    
    // Write serialized tree
    outputFile.write(serializedTree.c_str(), treeSize);
    
    // Write encoded data as bytes
    vector<unsigned char> encodedBytes = binaryStringToBytes(encodedText);
    outputFile.write(reinterpret_cast<const char*>(encodedBytes.data()), encodedBytes.size());
    
    outputFile.close();
    
    cout << "\nFile successfully compressed!" << endl;
    cout << "Compressed file saved at: " << outputFilePath << endl;
    cout << "Original file size: " << text.length() << " bytes" << endl;
    cout << "Compressed file size: " << (sizeof(uint32_t) * 3 + treeSize + encodedBytes.size()) << " bytes" << endl;
    
    return 0;
}
