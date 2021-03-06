#include "png.h"




const unsigned char Png::header_magic[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };



size_t Png::Leanify(size_t size_leanified /*= 0*/)
{

    char *p_read, *p_write;

    // header
    p_read = fp;
    p_write = p_read - size_leanified;

    if (size_leanified)
    {
        memmove(p_write, p_read, sizeof(header_magic));
    }

    p_read += sizeof(header_magic);
    p_write += sizeof(header_magic);


    // chunk
    uint32_t chunk_type;

    do
    {

        // read chunk length
        // use bswap to convert Big-Endian to Little-Endian

        uint32_t chunk_lenth = bswap32(*(uint32_t *)p_read);

        // read chunk type
        chunk_type = *(uint32_t *)(p_read + 4);

        // judge the case of first letter
        // remove all ancillary chunks except tRNS and APNG chunks and npTc
        // tRNS has transparency information
        if (chunk_type & 0x20000000)
        {

            switch (chunk_type)
            {
            case 'SNRt':    // tRNS     transparent
            case 'LTca':    // acTL     APNG
            case 'LTcf':    // fcTL     APNG
            case 'TAdf':    // fdAT     APNG    TODO: use zopfli to recompress fdAT
            case 'cTpn':    // npTc     Android 9Patch images (*.9.png)
                // move this chunk
                if (p_write != p_read)
                {
                    memmove(p_write, p_read, chunk_lenth + 12);
                }
                p_write += chunk_lenth + 12;
                break;

            default:
                // remove this chunk
                if (is_verbose)
                {
                    // chunk name
                    for (int i = 4; i < 8; i++)
                    {
                        std::cout << p_read[i];
                    }
                    std::cout << " chunk removed." << std::endl;
                }
                break;
            }

        }
        else
        {
            // move this chunk
            if (p_write != p_read)
            {
                memmove(p_write, p_read, chunk_lenth + 12);
            }
            p_write += chunk_lenth + 12;
        }


        // skip whole chunk
        p_read += chunk_lenth + 12;


    } while (chunk_type != 'DNEI');     // IEND

    fp -= size_leanified;
    uint32_t png_size = p_write - fp;

    if (is_recompress)
    {
        ZopfliPNGOptions zopflipng_options;
        zopflipng_options.lossy_transparent = true;
        // see the switch above for information about these chunks
        zopflipng_options.keepchunks.push_back("acTL");
        zopflipng_options.keepchunks.push_back("fcTL");
        zopflipng_options.keepchunks.push_back("fdAT");
        zopflipng_options.keepchunks.push_back("npTc");
        zopflipng_options.num_iterations = iterations;
        zopflipng_options.num_iterations_large = iterations / 3 + 1;
        // trying both methods does not worth the time it spend
        // it's better to use the time for more iterations.
        // zopflipng_options.block_split_strategy = 3;


        const std::vector<unsigned char> origpng(fp, fp + png_size);
        std::vector<unsigned char> resultpng;

        if (ZopfliPNGOptimize(origpng, zopflipng_options, is_verbose, &resultpng))
        {
            // error occurred
            return png_size;
        }

        // only use the result png if it is smaller
        // sometimes the original png is already highly optimized
        // then maybe zopfli will produce bigger file
        if (resultpng.size() < png_size)
        {
            memcpy(fp, resultpng.data(), resultpng.size());
            return resultpng.size();
        }
    }


    return png_size;

}
