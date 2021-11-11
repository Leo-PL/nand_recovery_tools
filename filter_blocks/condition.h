#ifndef CONDITION_H
#define CONDITION_H

#include <string>
#include <unistd.h>
#include <vector>

namespace filterblocks {

class Condition
{
public:
    using Byte = unsigned char;
    Condition(size_t offset, Byte mask, Byte value);
    explicit Condition(const std::string& str);
    //or std::span<char> in C++20
    bool matches(const std::vector<char>& buffer) const;

private:
    size_t offset;
    Byte mask;
    Byte value;
};

}

#endif // CONDITION_H
