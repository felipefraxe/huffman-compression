#include "bit_writer.h"

static inline void bw_flush_bytes(struct bit_writer *bw)
{
    if (bw->outsize == 0)
        return;

    fwrite(bw->outbuff, 1, bw->outsize, bw->file);
    bw->outsize = 0;
}

void bit_writer_init(struct bit_writer *bw, FILE *file)
{
    bw->bitbuff = 0;
    bw->nbits = 0;
    bw->outsize = 0;
    bw->file = file;
}

void bit_writer_write(struct bit_writer *bw, uint64_t bits, uint8_t nbits)
{
    while (nbits-- > 0)
    {
        bw->bitbuff = (bw->bitbuff << 1) | ((bits >> nbits) & 1);
        bw->nbits++;

        while (bw->nbits >= 8)
        {
            uint8_t byte = (bw->bitbuff >> (bw->nbits - 8)) & 0xFF;
            bw->outbuff[bw->outsize++] = byte;

            if (bw->outsize == sizeof(bw->outbuff))
                bw_flush_bytes(bw);

            bw->nbits -= 8;
            bw->bitbuff &= (1ULL << bw->nbits) - 1;
        }
    }
}

void bit_writer_flush(struct bit_writer *bw)
{
    if (bw->nbits > 0)
    {
        uint8_t byte = bw->bitbuff << (8 - bw->nbits);
        bw->outbuff[bw->outsize++] = byte;

        bw->bitbuff = 0;
        bw->nbits  = 0;
    }

    if (bw->outsize > 0)
        bw_flush_bytes(bw);
}
