#ifndef CONDITION_H
#define CONDITION_H

#include <string>
#include <unistd.h>

namespace filterblocks {

class Condition
{
public:
    using Byte = unsigned char;
    Condition(size_t offset, Byte mask, Byte value);
    explicit Condition(const std::string& str);
    bool matches(char* const buffer, size_t size) const;

private:
    size_t offset;
    Byte mask;
    Byte value;
};

}

#endif // CONDITION_H
