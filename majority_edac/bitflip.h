#ifndef BITFLIP_H
#define BITFLIP_H

namespace majorityedac {

struct BitFlip
{
    typedef bool BitValue;
    BitFlip();
    void addSamples(BitValue value, unsigned int count);
    bool isOne() const;
    unsigned int onesCount;
    unsigned int samplesCount;
};

}

#endif // BITFLIP_H
