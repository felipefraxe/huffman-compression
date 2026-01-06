#ifndef BIT_WRITER_H
#define BIT_WRITER_H

#include <stdio.h>
#include <stdint.h>

struct bit_writer
{
    uint64_t bitbuff;
    uint8_t  nbits;

    uint8_t outbuff[64];
    uint8_t outsize;

    FILE *file;
};

void bit_writer_init(struct bit_writer *bw, FILE *file);
void bit_writer_write(struct bit_writer *bw, uint64_t bits, uint8_t nbits);
void bit_writer_flush(struct bit_writer *bw);

#endif