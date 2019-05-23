#ifndef BITINDEX_H
#define BITINDEX_H

namespace majorityedac {

struct BitIndex
{
    BitIndex(unsigned long long int byte, unsigned int bit);
    unsigned long long int byte;
    unsigned int bit;
};

bool operator<(const BitIndex& lhs, const BitIndex rhs);

}

#endif // BITINDEX_H
