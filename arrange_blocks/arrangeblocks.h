#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "blockmarker.h"
namespace arrangeblocks
{

class Arrangeblocks {
public:
    Arrangeblocks(int argc, char *argv[]);
    ~Arrangeblocks();
    int run();

private:
    using ByteOffset = unsigned long long int;
    struct Options {
        std::string outputFile;
        std::string logFile;
        std::string inputFile;
        size_t blockSize;
        size_t pageSize;
        std::vector<BlockMarker> markers;
        bool invertMarker;
    };

    static const size_t BITS_PER_BYTE = 8;

    Options parseArgs(int argc, char *argv[]);

    void validateOptions();
    void assembleBlocks();
    size_t extractLogicalBlockNumber(char* block, size_t size);

    void log(const std::string& message);
    Options options;
    std::ifstream inputFile;
    std::ofstream outputFile;
    std::ofstream logFile;
    std::istream inputStream;
    std::ostream outputStream;
    std::ostream logStream;
};

}
