#ifndef BIT_READER_H
#define BIT_READER_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

struct bit_reader
{
    uint64_t bitbuff;
    uint8_t nbits;
    FILE *file;
};

void bit_reader_init(struct bit_reader *br, FILE *file);
int bit_reader_read_bit(struct bit_reader *br);
uint64_t bit_reader_read_bits(struct bit_reader *br, uint8_t nbits, bool *err);

#endif