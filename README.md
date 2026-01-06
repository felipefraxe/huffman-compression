# Huffman Encoder/Decoder in C

A small, efficient Huffman compression program written in C.  

Supports:  
- Compressing arbitrary files into a `.hf` format.  
- Decompressing `.hf` files back to their original form.  
- Uses a pre-order serialized Huffman tree and bit-level I/O.

## Compile

### Using `clang`:

```bash
clang -Wall -Wextra -O2 bit_reader.c bit_writer.c huff.c compress.c -o compress
clang -Wall -Wextra -O2 bit_reader.c bit_writer.c huff.c decompress.c -o decompress
```

### Using `gcc`:
```bash
gcc -Wall -Wextra -O2 bit_reader.c bit_writer.c huff.c compress.c -o compress
gcc -Wall -Wextra -O2 bit_reader.c bit_writer.c huff.c decompress.c -o decompress
```

## Usage
### Compress
```bash
./compress <filename>
```

### Decompress
```bash
decompress <filename.hf>
```

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
