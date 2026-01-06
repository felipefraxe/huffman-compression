#ifndef HUFF_H
#define HUFF_H

#include <stdint.h>
#include <stddef.h>
#include "bit_writer.h"
#include "bit_reader.h"

#define HUFF_EOF_SYMBOL 256

struct huff_node;

struct huff_tree
{
    struct huff_node *root;
};

struct huff_code
{
    uint64_t bits;
    uint8_t len;
};

void huff_tree_init(struct huff_tree *tree, const uint32_t freq[256]);
void huff_tree_free(struct huff_tree *tree);
void huff_tree_build_codes(struct huff_tree *tree, struct huff_code codes[257]);
uint16_t huff_tree_decode_symbol(struct huff_tree *tree, struct bit_reader *br);

/*
 * Huffman tree serialization:
 *  - Preorder traversal
 *  - 1 bit per node: 0 = internal, 1 = leaf
 *  - Leaf nodes followed by 9-bit symbol
 */
void huff_tree_serialize(struct huff_tree *tree, struct bit_writer *bw);
void huff_tree_deserialize(struct huff_tree *tree, struct bit_reader *br);

#endif