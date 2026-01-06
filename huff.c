#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "huff.h"

struct huff_node
{
    uint16_t symbol;
    uint32_t freq;
    struct huff_node *left;
    struct huff_node *right;
};

struct heap
{
    struct huff_node *nodes[257];
    size_t size;
};

static inline void swap(struct huff_node **a, struct huff_node **b)
{
    struct huff_node *tmp = *a;
    *a = *b;
    *b = tmp;
}

static void sift_up(struct heap *heap, size_t i)
{
    while (i > 0)
    {
        size_t parent = (i - 1) / 2;
        if (heap->nodes[i]->freq >= heap->nodes[parent]->freq)
            return;
        swap(&heap->nodes[i], &heap->nodes[parent]);
        i = parent;
    }
}

static void sift_down(struct heap *heap, size_t i)
{
    for (;;)
    {
        size_t left = (2 * i) + 1;
        size_t right = left + 1;
        size_t smallest = i;

        if (left < heap->size && heap->nodes[left]->freq < heap->nodes[smallest]->freq)
            smallest = left;
        if (right < heap->size && heap->nodes[right]->freq < heap->nodes[smallest]->freq)
            smallest = right;

        if (smallest == i)
            return;
        swap(&heap->nodes[i], &heap->nodes[smallest]);
        i = smallest;
    }
}

static void heap_build(struct heap *heap)
{
    if (heap->size < 2)
        return;

    for (size_t i = (heap->size / 2); i-- > 0;)
        sift_down(heap, i);
}

static struct huff_node *heap_extract_min(struct heap *heap)
{
    if (heap->size == 0)
        return NULL;

    struct huff_node *min = heap->nodes[0];
    heap->size--;

    if (heap->size > 0)
    {
        heap->nodes[0] = heap->nodes[heap->size];
        sift_down(heap, 0);
    }

    return min;
}

static void heap_insert(struct heap *heap, struct huff_node *node)
{
    heap->nodes[heap->size++] = node;
    sift_up(heap, heap->size - 1);
}

static inline struct huff_node *huff_node_alloc(uint16_t symbol, uint32_t freq)
{
    struct huff_node *node = malloc(sizeof(*node));
    if (node == NULL)
        return NULL;

    node->symbol = symbol;
    node->freq = freq;
    node->left = NULL;
    node->right = NULL;

    return node;
}

static inline bool huff_node_is_leaf(struct huff_node *node)
{
    return node->left == NULL && node->right == NULL;
}

void huff_tree_init(struct huff_tree *tree, const uint32_t freq[256])
{
    struct heap heap = {0};
    for (uint16_t symbol = 0; symbol <= 0xFF; symbol++)
    {
        if (freq[symbol] == 0)
            continue;

        struct huff_node *node = huff_node_alloc(symbol, freq[symbol]);
        if (node == NULL)
        {
            fprintf(stderr, "Error: Unable to allocate memory for Huffman tree node.\n");
            return;
        }
        heap.nodes[heap.size++] = node;
    }
    heap.nodes[heap.size++] = huff_node_alloc(HUFF_EOF_SYMBOL, 1);
    heap_build(&heap);

    while (heap.size > 1)
    {
        struct huff_node *left = heap_extract_min(&heap);
        struct huff_node *right = heap_extract_min(&heap);

        struct huff_node *parent = huff_node_alloc(0, left->freq + right->freq);
        if (parent == NULL)
        {
            fprintf(stderr, "Error: Unable to allocate memory for Huffman tree node.\n");
            return;
        }
        parent->left = left;
        parent->right = right;

        heap_insert(&heap, parent);
    }

    tree->root = heap_extract_min(&heap);
}

static void huff_tree_free_node(struct huff_node *node)
{
    if (node == NULL)
        return;
    huff_tree_free_node(node->left);
    huff_tree_free_node(node->right);
    free(node);
}

void huff_tree_free(struct huff_tree *tree)
{
    huff_tree_free_node(tree->root);
    tree->root = NULL;
}

static void huff_tree_build_code(struct huff_node *node, struct huff_code codes[257], uint64_t code, uint8_t depth)
{
    if (node == NULL)
        return;

    if (huff_node_is_leaf(node))
    {
        codes[node->symbol].bits = code;
        codes[node->symbol].len = depth ? depth : 1;
        return;
    }

    huff_tree_build_code(node->left, codes, (code << 1) | 0, depth + 1);
    huff_tree_build_code(node->right, codes, (code << 1) | 1, depth + 1);
}

void huff_tree_build_codes(struct huff_tree *tree, struct huff_code codes[257])
{
    if (tree->root == NULL)
        return;
    huff_tree_build_code(tree->root, codes, 0, 0);
}

static void huff_tree_serialize_node(struct huff_node *node, struct bit_writer *bw)
{
    if (huff_node_is_leaf(node))
    {
        bit_writer_write(bw, 1, 1);
        bit_writer_write(bw, node->symbol, 9);
        return;
    }

    bit_writer_write(bw, 0, 1);
    huff_tree_serialize_node(node->left, bw);
    huff_tree_serialize_node(node->right, bw);
}

void huff_tree_serialize(struct huff_tree *tree, struct bit_writer *bw)
{
    if (tree->root == NULL)
        return;
    huff_tree_serialize_node(tree->root, bw);
}

static struct huff_node *huff_tree_deserialize_node(struct bit_reader *br)
{
    int bit = bit_reader_read_bit(br);

    if (bit == 1)
    {
        bool err = false;
        uint64_t symbol = bit_reader_read_bits(br, 9, &err);
        if (err)
            return NULL;
        struct huff_node *node = huff_node_alloc((uint16_t) symbol, 0);
        return node;
    }

    struct huff_node *node = huff_node_alloc(0, 0);
    node->left  = huff_tree_deserialize_node(br);
    node->right = huff_tree_deserialize_node(br);
    return node;
}

void huff_tree_deserialize(struct huff_tree *tree, struct bit_reader *br)
{
    tree->root = huff_tree_deserialize_node(br);
}

uint16_t huff_tree_decode_symbol(struct huff_tree *tree, struct bit_reader *br)
{
    struct huff_node *node = tree->root;
    while (node != NULL && huff_node_is_leaf(node) == false)
    {
        int bit;
        if ((bit = bit_reader_read_bit(br)) == -1)
            return HUFF_EOF_SYMBOL;
        node = (bit == 0) ? node->left : node->right;
    }
    return node->symbol;
}
