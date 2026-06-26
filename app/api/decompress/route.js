import { NextResponse } from "next/server";
import { writeFile, readFile, unlink } from "fs/promises";
import { exec } from "child_process";
import { promisify } from "util";
import path from "path";

const execAsync = promisify(exec);

export async function POST(request) {
  try {
    const formData = await request.formData();
    const file = formData.get("file");

    if (!file) {
      return NextResponse.json({ error: "No file uploaded" }, { status: 400 });
    }

    const bytes = await file.arrayBuffer();
    const buffer = Buffer.from(bytes);
    const inputPath = path.join(process.cwd(), "tmp", `compressed_${Date.now()}.huf`);
    const decoderPath = path.join(process.cwd(), "bin", "decoder.exe");

    await import("fs").then((fs) =>
      fs.promises.mkdir(path.join(process.cwd(), "tmp"), { recursive: true })
    );

    await writeFile(inputPath, buffer);

    const { stdout } = await execAsync(`"${decoderPath}" "${inputPath}"`);

    const outputPath = inputPath.replace(".huf", "_decompressed.txt");
    const decompressedBuffer = await readFile(outputPath);

    const originalSize = stdout.match(/Original size: (\d+)/)?.[1] || "N/A";
    const decompressedSize = decompressedBuffer.length.toString();

    await unlink(inputPath);
    await unlink(outputPath);

    return new NextResponse(decompressedBuffer, {
      status: 200,
      headers: {
        "Content-Type": "text/plain",
        "Content-Disposition": `attachment; filename="decompressed.txt"`,
        "X-Original-Size": originalSize,
        "X-Decompressed-Size": decompressedSize,
      },
    });
  } catch (error) {
    console.error("Decompression error:", error);
    return NextResponse.json({ error: error.message }, { status: 500 });
  }
}