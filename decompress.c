#include <stdlib.h>
#include <string.h>
#include "bit_reader.h"
#include "huff.h"

#define HUFF_EOF_SYMBOL 256

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename.hf>\n", argv[0]);
        return 1;
    }
    const char *in_path = argv[1];
    FILE *infile = fopen(in_path, "rb");
    if (!infile)
    {
        fprintf(stderr, "Error: Unable to open input file.\n");
        return 1;
    }

    uint8_t magic[4];
    fread(magic, sizeof(magic), 1, infile);

    if (memcmp(magic, "HUFF", 4) != 0)
    {
        fprintf(stderr, "Error: Invalid file format.\n");
        fclose(infile);
        return 1;
    }

    struct bit_reader br;
    bit_reader_init(&br, infile);

    struct huff_tree tree = {0};
    huff_tree_deserialize(&tree, &br);

    char out_path[strlen(in_path) + 2];
    memcpy(out_path, in_path, strlen(in_path) - 2);
    strncat(out_path, "dec", 4);
    FILE *outfile = fopen(out_path, "wb");
    if (!outfile)
    {
        fprintf(stderr, "Error: Unable to open output file.\n");
        fclose(infile);
        return 1;
    }

    uint16_t symbol;
    while ((symbol = huff_tree_decode_symbol(&tree, &br)) != HUFF_EOF_SYMBOL)
        fputc((uint8_t) symbol, outfile);

    huff_tree_free(&tree);    
    fclose(infile);
    fclose(outfile);
    return 0;
}