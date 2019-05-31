#include "blockmarker.h"

#include <cstdio>
#include <stdexcept>

namespace arrangeblocks {

BlockMarker::BlockMarker():
    offset(0),
    mask(0)
{}

BlockMarker::BlockMarker(size_t offset, Byte mask):
    offset(offset),
    mask(mask)
{}

BlockMarker::BlockMarker(const std::string& str)
{
    if (std::sscanf(str.c_str(), "%zu/%hhx", &offset, &mask) != 2)
        throw std::runtime_error("Error parsing condition: " + str);
}

BlockMarker::Byte BlockMarker::value(char* const buffer, size_t size, bool invertMarker) const
{
    if(offset < size)
        return foldMasked((invertMarker) ? ~buffer[offset] : buffer[offset]);
    else
        throw std::runtime_error("Condition offset outside buffer size: " +
                                 std::to_string(offset) + " vs " +
                                 std::to_string(size) + " bytes");
}

BlockMarker::Byte BlockMarker::valueBits() const
{
    return __builtin_popcount(mask);
}

BlockMarker::Byte BlockMarker::foldMasked(BlockMarker::Byte input) const
{
    Byte ret = 0;
    int foldedIndex = 0;
    for (int i = 0; i < 8; ++i) {
        Byte tempMask = (1 << i);
        if (mask & tempMask) {
            if (input & tempMask)
                ret |= (1 << foldedIndex);
            ++foldedIndex;
        }
    }
    return ret;
}

}
