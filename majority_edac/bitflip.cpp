#include "bitflip.h"

#include <stdexcept>

namespace majorityedac {

BitFlip::BitFlip():
    onesCount(0),
    samplesCount(0)
{

}

void BitFlip::addSamples(BitValue value, unsigned int count)
{
    samplesCount += count;
    if (value)
        onesCount += count;
}

bool BitFlip::isOne() const
{
    if (samplesCount % 2 == 0)
        throw std::runtime_error("BUG: Majority decision taken with even sample count.");
    if (onesCount == samplesCount)
        throw std::runtime_error("BUG: Bitflip misdetected. Ones count cannot equal samples count.");
    else if (onesCount == 0)
        throw std::runtime_error("BUG: Bitflip misdetected. Ones count cannot equal zero.");

    return 2 * onesCount > samplesCount;
}

}
