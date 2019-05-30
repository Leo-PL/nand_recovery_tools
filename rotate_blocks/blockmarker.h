#ifndef BLOCKMARKER_H
#define BLOCKMARKER_H

#include <cstddef>
#include <string>

namespace rotateblocks {

class BlockMarker
{
public:
    using Byte = unsigned char;
    BlockMarker();
    BlockMarker(size_t offset, Byte mask);
    explicit BlockMarker(const std::string& str);
    Byte value(char* const buffer, size_t size, bool invertMarker) const;

private:
    size_t offset;
    Byte mask;
};

}

#endif // BLOCKMARKER_H
