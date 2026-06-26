import { NextResponse } from "next/server";
import { writeFile, readFile, unlink } from "fs/promises";
import { exec } from "child_process";
import { promisify } from "util";
import path from "path";

const execAsync = promisify(exec);

export async function POST(request) {
  try {
    // Get the uploaded file
    const formData = await request.formData();
    const file = formData.get("file");

    if (!file) {
      return NextResponse.json({ error: "No file uploaded" }, { status: 400 });
    }

    // Save uploaded file temporarily
    const bytes = await file.arrayBuffer();
    const buffer = Buffer.from(bytes);
    const inputPath = path.join(process.cwd(), "tmp", `input_${Date.now()}.txt`);
    const encoderPath = path.join(process.cwd(), "bin", "encoder.exe");

    // Create tmp folder if it doesn't exist
    await import("fs").then((fs) =>
      fs.promises.mkdir(path.join(process.cwd(), "tmp"), { recursive: true })
    );

    // Write uploaded file to tmp folder
    await writeFile(inputPath, buffer);

    // Call the C++ encoder binary
    const { stdout, stderr } = await execAsync(`"${encoderPath}" "${inputPath}"`);

    // The encoder saves output as input_timestamp_compressed.huf
    const compressedPath = inputPath.replace(".txt", "_compressed.huf");

    // Read the compressed file
    const compressedBuffer = await readFile(compressedPath);

    // Parse stats from stdout
    const originalSize = stdout.match(/Original file size: (\d+)/)?.[1] || "N/A";
    const compressedSize = stdout.match(/Compressed file size: (\d+)/)?.[1] || "N/A";
    const ratio = stdout.match(/Compression ratio: ([\d.]+)/)?.[1] || "N/A";

    // Cleanup temp input file
    await unlink(inputPath);
    await unlink(compressedPath);

    // Return compressed file + stats
    return new NextResponse(compressedBuffer, {
      status: 200,
      headers: {
        "Content-Type": "application/octet-stream",
        "Content-Disposition": `attachment; filename="compressed.huf"`,
        "X-Original-Size": originalSize,
        "X-Compressed-Size": compressedSize,
        "X-Compression-Ratio": ratio,
      },
    });
  } catch (error) {
    console.error("Compression error:", error);
    return NextResponse.json({ error: error.message }, { status: 500 });
  }
}