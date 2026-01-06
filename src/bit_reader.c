#include "bit_reader.h"

void bit_reader_init(struct bit_reader *br, FILE *file)
{
    br->bitbuff = 0;
    br->nbits = 0;
    br->file = file;
}

static int bit_reader_fill(struct bit_reader *br)
{
    int c = fgetc(br->file);
    if (c == EOF)
        return -1;

    br->bitbuff = (br->bitbuff << 8) | (uint8_t) c;
    br->nbits += 8;
    return 0;
}

int bit_reader_read_bit(struct bit_reader *br)
{
    if (br->nbits == 0 && bit_reader_fill(br) == -1)
        return -1;

    int bit = (br->bitbuff >> (br->nbits - 1)) & 1;
    br->nbits--;
    return bit;
}

uint64_t bit_reader_read_bits(struct bit_reader *br, uint8_t nbits, bool *err)
{
    uint64_t val = 0;

    while (nbits--)
    {
        int bit = bit_reader_read_bit(br);
        if (bit == -1)
        {
            *err = true;
            return val;
        }
        val = (val << 1) | bit;
    }

    return val;
}