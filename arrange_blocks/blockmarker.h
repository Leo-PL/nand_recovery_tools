#ifndef BLOCKMARKER_H
#define BLOCKMARKER_H

#include <cstddef>
#include <string>

namespace arrangeblocks {

class BlockMarker
{
public:
    using Byte = unsigned char;
    BlockMarker();
    BlockMarker(size_t offset, Byte mask);
    explicit BlockMarker(const std::string& str);
    Byte value(char* const buffer, size_t size, bool invertMarker) const;
    Byte valueBits() const;

private:
    Byte foldMasked(Byte input) const;
    size_t offset;
    Byte mask;
};

}

#endif // BLOCKMARKER_H
