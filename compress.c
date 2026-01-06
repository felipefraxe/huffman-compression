#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bit_writer.h"
#include "huff.h"

#define BUFF_SIZE 4096

void freq_table_build(const char* path, uint32_t freqs[256]);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    const char *in_path = argv[1];

    uint32_t freqs[256];
    freq_table_build(in_path, freqs);

    struct huff_tree tree = {0};
    huff_tree_init(&tree, freqs);

    struct huff_code code_table[257] = {0};
    huff_tree_build_codes(&tree, code_table);

    FILE *infile = fopen(in_path, "rb");
    if (!infile)
    {
        fprintf(stderr, "Error: Unable to open input file.\n");
        return 1;
    }

    char out_path[strlen(in_path) + 3 + 1];
    sprintf(out_path, "%s.hf", in_path);
    FILE *outfile = fopen(out_path, "wb");
    if (!outfile)
    {
        fprintf(stderr, "Error: Unable to open output file.\n");
        fclose(infile);
        return 1;
    }

    uint8_t magic[] = {'H', 'U', 'F', 'F'};
    fwrite(magic, sizeof(magic), 1, outfile);

    struct bit_writer bw;
    bit_writer_init(&bw, outfile);

    huff_tree_serialize(&tree, &bw);

    size_t bytes_read = 0;
    for (uint8_t buff[BUFF_SIZE]; (bytes_read = fread(buff, 1, BUFF_SIZE, infile)) != 0;)
    {
        for (size_t i = 0; i < bytes_read; i++)
        {
            uint8_t byte = buff[i];
            struct huff_code code = code_table[byte];
            bit_writer_write(&bw, code.bits, code.len);
        }
    }
    struct huff_code eof_code = code_table[HUFF_EOF_SYMBOL];
    bit_writer_write(&bw, eof_code.bits, eof_code.len);
    bit_writer_flush(&bw);

    huff_tree_free(&tree);
    fclose(infile);
    fclose(outfile);
    return 0;
}

void freq_table_build(const char *path, uint32_t freqs[256])
{
    memset(freqs, 0, 256 * sizeof(*freqs));

    FILE *file = fopen(path, "rb");
    if (!file)
        return;

    size_t bytes_read = 0;
    for (uint8_t buff[BUFF_SIZE]; (bytes_read = fread(buff, 1, BUFF_SIZE, file)) != 0;)
    {
        for (size_t i = 0; i < bytes_read; i++)
        {
            uint8_t byte = buff[i];
            freqs[byte]++;
        }
    }

    fclose(file);
}
