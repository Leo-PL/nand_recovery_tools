#include "blockmarker.h"

#include <cstdio>
#include <stdexcept>

namespace rotateblocks {

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
        return ((invertMarker) ? ~buffer[offset] : buffer[offset]) & mask;
    else
        throw std::runtime_error("Condition offset outside buffer size: " +
                                 std::to_string(offset) + " vs " +
                                 std::to_string(size) + " bytes");
}

}
