# Huffman Encoder/Decoder in C

A small, efficient Huffman compression program written in C.  

Supports:  
- Compressing arbitrary files into a `.hf` format.  
- Decompressing `.hf` files back to their original form.  
- Uses a pre-order serialized Huffman tree and bit-level I/O.

## Compile

Simply run
```bash
make
```
This will produce two executables in bin/:
- bin/compress
- bin/decompress

## Clean Build files
```bash
make clean
```
This will:
- Delete all .o files in build/
- Delete all executables in bin/

## Usage
### Compress
```bash
./bin/compress <filename>
```
Generates a compressed file: <filename>.hf.

### Decompress
```bash
./bin/decompress <filename.hf>
```
Generates a decompressed file: <filename>.dec.

## License

This project is open source and released under the **MIT License**.  
You are free to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the software, as long as the original copyright notice and this permission notice are included.  

See [LICENSE](LICENSE) for full details.

## Notes

- The program works with **any file type**, including binary files.  
- Compression is implemented using a **pre-order serialized Huffman tree** with **bit-level I/O** for efficiency.
- An internal **EOF symbol** ensures the exact end of the original file is preserved during decompression.
- For best performance, compile with optimization flags like `-O2` or `-O3`.
- Optional: `-Wall -Wextra` flags help catch warnings and improve code safety.
