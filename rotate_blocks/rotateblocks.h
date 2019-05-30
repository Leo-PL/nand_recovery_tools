#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include "blockmarker.h"
namespace rotateblocks
{

class RotateBlocks {
public:
    RotateBlocks(int argc, char *argv[]);
    ~RotateBlocks();
    int run();

private:
    using ByteOffset = unsigned long long int;
    struct Options {
        std::string outputFile;
        std::string logFile;
        std::string inputFile;
        size_t blockSize;
        size_t pageSize;
        BlockMarker marker;
        bool invertMarker;
    };

    Options parseArgs(int argc, char *argv[]);

    void validateOptions();
    void rotateBlocksContents();

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
