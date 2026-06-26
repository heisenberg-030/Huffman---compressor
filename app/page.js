"use client";
import { useState } from "react";

export default function Home() {
  const [tab, setTab] = useState("compress");
  const [file, setFile] = useState(null);
  const [stats, setStats] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);
  const [downloadUrl, setDownloadUrl] = useState(null);
  const [downloadName, setDownloadName] = useState("");

  const reset = () => {
    setFile(null);
    setStats(null);
    setError(null);
    if (downloadUrl) URL.revokeObjectURL(downloadUrl);
    setDownloadUrl(null);
    setDownloadName("");
  };

  const handleTabChange = (t) => {
    setTab(t);
    reset();
  };

  const handleFileChange = (e) => {
    reset();
    setFile(e.target.files[0]);
  };

  const handleSubmit = async () => {
    if (!file) return;
    setLoading(true);
    setError(null);
    setStats(null);
    if (downloadUrl) URL.revokeObjectURL(downloadUrl);
    setDownloadUrl(null);

    try {
      const formData = new FormData();
      formData.append("file", file);

      const endpoint = tab === "compress" ? "/api/compress" : "/api/decompress";
      const res = await fetch(endpoint, { method: "POST", body: formData });

      if (!res.ok) {
        const err = await res.json();
        throw new Error(err.error || "Operation failed");
      }

      if (tab === "compress") {
        const originalSize = res.headers.get("X-Original-Size");
        const compressedSize = res.headers.get("X-Compressed-Size");
        const ratio = res.headers.get("X-Compression-Ratio");
        setStats({ originalSize, compressedSize, ratio, mode: "compress" });
        setDownloadName("compressed.huf");
      } else {
        const originalSize = res.headers.get("X-Original-Size");
        const decompressedSize = res.headers.get("X-Decompressed-Size");
        setStats({ originalSize, decompressedSize, mode: "decompress" });
        setDownloadName("decompressed.txt");
      }

      const blob = await res.blob();
      const url = URL.createObjectURL(blob);
      setDownloadUrl(url);
    } catch (err) {
      setError(err.message);
    } finally {
      setLoading(false);
    }
  };

  const isCompress = tab === "compress";

  return (
    <main className="min-h-screen bg-gray-950 text-white flex flex-col items-center justify-center p-8">
      <div className="w-full max-w-lg bg-gray-900 rounded-2xl p-8 shadow-2xl border border-gray-800">

        <h1 className="text-3xl font-bold text-center mb-2 text-white">
          Huffman Compressor
        </h1>
        <p className="text-center text-gray-400 text-sm mb-6">
          Compress any text file using a C++ Huffman encoding engine
        </p>

        {/* Tab Switcher */}
        <div className="flex bg-gray-800 rounded-xl p-1 mb-6">
          <button
            onClick={() => handleTabChange("compress")}
            className={`flex-1 py-2 rounded-lg text-sm font-semibold transition-colors ${
              isCompress ? "bg-blue-600 text-white" : "text-gray-400 hover:text-white"
            }`}
          >
            Compress
          </button>
          <button
            onClick={() => handleTabChange("decompress")}
            className={`flex-1 py-2 rounded-lg text-sm font-semibold transition-colors ${
              !isCompress ? "bg-blue-600 text-white" : "text-gray-400 hover:text-white"
            }`}
          >
            Decompress
          </button>
        </div>

        {/* Upload Area */}
        <div className="border-2 border-dashed border-gray-700 rounded-xl p-6 text-center mb-6 hover:border-blue-500 transition-colors">
          <input
            type="file"
            accept={isCompress ? ".txt" : ".huf"}
            onChange={handleFileChange}
            className="hidden"
            id="fileInput"
            key={tab}
          />
          <label htmlFor="fileInput" className="cursor-pointer">
            <div className="text-4xl mb-3">📁</div>
            {file ? (
              <p className="text-blue-400 font-medium">{file.name}</p>
            ) : (
              <p className="text-gray-400">
                Click to upload a{" "}
                <span className="text-white">{isCompress ? ".txt" : ".huf"}</span> file
              </p>
            )}
          </label>
        </div>

        {file && (
          <p className="text-center text-gray-500 text-sm mb-4">
            File size: <span className="text-white">{file.size} bytes</span>
          </p>
        )}

        <button
          onClick={handleSubmit}
          disabled={!file || loading}
          className="w-full py-3 rounded-xl font-semibold text-white bg-blue-600 hover:bg-blue-500 disabled:opacity-40 disabled:cursor-not-allowed transition-colors"
        >
          {loading ? "Processing..." : isCompress ? "Compress File" : "Decompress File"}
        </button>

        {error && (
          <div className="mt-6 bg-red-900/40 border border-red-700 rounded-xl p-4 text-red-300 text-sm">
            Error: {error}
          </div>
        )}

        {stats && (
          <div className="mt-6 bg-gray-800 rounded-xl p-5 border border-gray-700">
            <h2 className="text-lg font-semibold mb-4 text-white">
              {isCompress ? "Compression Results" : "Decompression Results"}
            </h2>
            <div className="grid grid-cols-2 gap-4 text-center">
              <div className="bg-gray-900 rounded-lg p-3">
                <p className="text-gray-400 text-xs mb-1">Original</p>
                <p className="text-white font-bold">{stats.originalSize} B</p>
              </div>
              <div className="bg-gray-900 rounded-lg p-3">
                <p className="text-gray-400 text-xs mb-1">
                  {isCompress ? "Compressed" : "Decompressed"}
                </p>
                <p className="text-green-400 font-bold">
                  {isCompress ? stats.compressedSize : stats.decompressedSize} B
                </p>
              </div>
            </div>

            {isCompress && stats.ratio && (
              <div className="mt-3 bg-gray-900 rounded-lg p-3 text-center">
                <p className="text-gray-400 text-xs mb-1">Compression Ratio</p>
                <p className="text-blue-400 font-bold">
                  {(parseFloat(stats.ratio) * 100).toFixed(1)}%
                </p>
              </div>
            )}

            {downloadUrl && (
              <a
                href={downloadUrl}
                download={downloadName}
                className="mt-4 block w-full py-3 rounded-xl font-semibold text-center text-white bg-green-600 hover:bg-green-500 transition-colors"
              >
                Download {isCompress ? "Compressed" : "Decompressed"} File
              </a>
            )}
          </div>
        )}
      </div>
    </main>
  );
}