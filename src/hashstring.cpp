
#include <string>
#include <SDL.h>
#include "hashstring.h"

namespace venk
{

#define FNV_32_PRIME ((Uint32)0x01000193)

/* the magic number is the inital seed value for the hash algo (FNV1A) */
Uint32 HashString::hval = ((Uint32) 0x811c9dc5);

HashString::HashString()
{

}

HashString::HashString(const char *str)
{
    crc = Hash(str);
#ifndef NDEBUG
    originalString = std::string(str);
#endif
}


HashString::HashString(const std::string &str)
{
    crc = Hash(str.c_str());
#ifndef NDEBUG
    originalString = str;
#endif
}

Uint32 HashString::getValue()
{
    return crc;
}

HashString::operator Uint32()
{
    return crc;
}

#ifndef NDEBUG
std::string HashString::getString()
{
    return originalString;
}
#endif

Uint32 HashString::Hash(const char *str)
{
    Uint32 *s = (Uint32 *) str;

    /*
     * FNV-1a hash each octet in the buffer
     */
    while (*s)
    {

        /* xor the bottom with the current octet */
        HashString::hval ^= (Uint32)(*s++);

        /* multiply by the 32 bit FNV magic prime mod 2^32 */
        HashString::hval *= FNV_32_PRIME;
    }

    return HashString::hval;
}

}
