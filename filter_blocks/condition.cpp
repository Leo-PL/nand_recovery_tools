#include "condition.h"

#include <cstdio>
#include <stdexcept>

namespace filterblocks {

Condition::Condition(size_t offset, Byte mask, Byte value):
    offset(offset),
    mask(mask),
    value(value)
{}

Condition::Condition(const std::string &str)
{
    if (std::sscanf(str.c_str(), "%zu/%hhx/%hhx", &offset, &mask, &value) != 3)
        throw std::runtime_error("Error parsing condition: " + str);
}

bool Condition::matches(const std::vector<char>& buffer) const
{
    if(offset < buffer.size())
        return (buffer[offset] & mask) == (value & mask);
    else
        throw std::runtime_error("Condition offset outside buffer size: " +
                                 std::to_string(offset) + " vs " +
                                 std::to_string(buffer.size()) + " bytes");
}

}
