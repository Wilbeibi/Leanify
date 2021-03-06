#include "leanify.h"


// return new size
size_t LeanifyFile(void *file_pointer, size_t file_size, size_t size_leanified /*= 0*/)
{
    if (!memcmp(file_pointer, Png::header_magic, sizeof(Png::header_magic)))
    {
        if (is_verbose)
        {
            std::cout << "PNG detected." << std::endl;
        }
        return Png(file_pointer).Leanify(size_leanified);
    }
    else if (!memcmp(file_pointer, Lua::header_magic, sizeof(Lua::header_magic)))
    {
        if (is_verbose)
        {
            std::cout << "Lua detected." << std::endl;
        }
        return Lua(file_pointer).Leanify(size_leanified);
    }
    else if (!memcmp(file_pointer, Zip::header_magic, sizeof(Zip::header_magic)))
    {
        if (is_verbose)
        {
            std::cout << "ZIP detected." << std::endl;
        }
        return Zip(file_pointer, file_size).Leanify(size_leanified);
    }
    else if (!memcmp(file_pointer, Rdb::header_magic, sizeof(Rdb::header_magic)))
    {
        if (is_verbose)
        {
            std::cout << "RDB detected." << std::endl;
        }
        return Rdb(file_pointer).Leanify(size_leanified);
    }
    else if (!memcmp(file_pointer, Gz::header_magic, sizeof(Gz::header_magic)))
    {
        if (is_verbose)
        {
            std::cout << "GZ detected." << std::endl;
        }
        return Gz(file_pointer, file_size).Leanify(size_leanified);
    }
    else if (!memcmp(file_pointer, Gft::header_magic, sizeof(Gft::header_magic)))
    {
        if (is_verbose)
        {
            std::cout << "GFT detected." << std::endl;
        }
        return Gft(file_pointer, file_size).Leanify(size_leanified);
    }
    else if (!memcmp(file_pointer, Ico::header_magic, sizeof(Ico::header_magic)))
    {
        if (is_verbose)
        {
            std::cout << "ICO detected." << std::endl;
        }
        return Ico(file_pointer).Leanify(size_leanified);
    }
    else
    {
        // xml file does not have header magic to tell if it is a xml file.
        // have to try to parse and see if there is any errors.
        Xml x(file_pointer, file_size);
        if (x.IsValid())
        {
            if (is_verbose)
            {
                std::cout << "XML detected." << std::endl;
            }
            return x.Leanify(size_leanified);
        }
    }

    if (is_verbose)
    {
        std::cout << "Format not supported!" << std::endl;
    }
    // for unsupported format, just memmove it.
    return Format(file_pointer, file_size).Leanify(size_leanified);
}