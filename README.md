# Huffman File Compressor

A full-stack file compression web application powered by a C++ Huffman encoding engine, wrapped in a modern Next.js frontend.

## 🔗 Links
- **Live Demo:** [huffman-compressor-production.up.railway.app](https://huffman-compressor-production.up.railway.app)
- **GitHub:** [github.com/heisenberg-030/Huffman---compressor](https://github.com/heisenberg-030/Huffman---compressor)

---

## 📌 About The Project

This project implements **Huffman Coding** — a greedy algorithm used for lossless data compression. Characters that appear more frequently in a file are assigned shorter binary codes, while less frequent characters get longer codes, resulting in an overall smaller file size.

The unique aspect of this project is the architecture — a native **C++ compression engine** is bridged to a **Next.js web application** via Node.js `child_process`, combining systems-level programming with modern full-stack web development.

---

## ✨ Features

- 📁 Upload any `.txt` file and compress it using Huffman encoding
- 📊 View real-time compression stats — original size, compressed size, and compression ratio
- ⬇️ Download the compressed `.huf` file
- 🔄 Upload a `.huf` file and decompress it back to the original text
- 🌐 Fully deployed and accessible via a live URL

---

## 🛠️ Tech Stack

| Layer | Technology |
|---|---|
| Core Engine | C++ (Huffman encoding/decoding) |
| Backend | Next.js API Routes, Node.js child_process |
| Frontend | Next.js 15, React, Tailwind CSS |
| Deployment | Railway |
| Version Control | Git, GitHub |

---

## ⚙️ How Huffman Coding Works

1. **Frequency Analysis** — Count how often each character appears in the file
2. **Priority Queue** — Insert all characters into a min-heap sorted by frequency
3. **Build Huffman Tree** — Repeatedly merge the two lowest-frequency nodes until one tree remains
4. **Generate Codes** — Traverse the tree; left edge = 0, right edge = 1
5. **Encode** — Replace each character with its Huffman code
6. **Store Tree** — Serialize the tree into the compressed file for later decoding
7. **Decode** — Reconstruct the tree and walk it bit by bit to recover original text

---

## 🏗️ Architecture

```
User uploads .txt file (React Frontend)
          ↓
Next.js API Route receives file
          ↓
Node.js child_process calls C++ encoder binary
          ↓
C++ engine builds Huffman tree + encodes file
          ↓
Compressed .huf file returned with stats
          ↓
User downloads compressed file
```

---

## 🚀 Running Locally

### Prerequisites
- Node.js 18+
- g++ compiler

### Steps

```bash
# Clone the repository
git clone https://github.com/heisenberg-030/Huffman---compressor.git
cd Huffman---compressor

# Compile the C++ engine
mkdir -p bin
g++ Encoder.cpp -o bin/encoder
g++ Decoder.cpp -o bin/decoder

# Install dependencies
npm install

# Run the development server
npm run dev
```

Open [http://localhost:3000](http://localhost:3000) in your browser.

---

## 📁 Project Structure

```
huffman-compressor/
├── app/
│   ├── api/
│   │   ├── compress/
│   │   │   └── route.js      # Compression API endpoint
│   │   └── decompress/
│   │       └── route.js      # Decompression API endpoint
│   ├── page.js               # Main frontend UI
│   └── layout.js
├── bin/                      # Compiled C++ binaries
├── Encoder.cpp               # Huffman encoder source
├── Decoder.cpp               # Huffman decoder source
├── build.sh                  # Linux build script for Railway
├── public/
└── package.json
```

---

## 📊 Performance

| File Size | Compression Ratio |
|---|---|
| < 100 bytes | May increase (tree overhead) |
| ~1 KB | ~20–30% reduction |
| ~6 KB | ~48% reduction |
| Large files | Up to ~50% reduction |

> Note: Huffman compression works best on larger files with repeated characters. Small files may see size increase due to the overhead of storing the Huffman tree.

---

## 🧠 What I Learned

- How Huffman coding works at the binary level
- Bridging native C++ binaries with a Node.js backend using `child_process`
- Cross-platform binary compilation (Windows `.exe` vs Linux binary)
- File handling in Next.js API routes using `FormData` and `Buffer`
- Deploying full-stack apps with native dependencies on Railway

---

## 📄 License

MIT License — feel free to use this project for learning purposes.
