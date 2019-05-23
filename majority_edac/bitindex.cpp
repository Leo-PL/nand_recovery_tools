#include "bitindex.h"

namespace majorityedac {

BitIndex::BitIndex(unsigned long long byte, unsigned int bit):
    byte(byte),
    bit(bit)
{}


bool operator<(const BitIndex &lhs, const BitIndex rhs)
{
    return (lhs.byte < rhs.byte) || (lhs.byte == rhs.byte && lhs.bit < rhs.bit);
}

}
