#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "condition.h"

namespace filterblocks
{

class FilterBlocks {
public:
    FilterBlocks(int argc, char *argv[]);
    ~FilterBlocks();
    int run();

private:
    struct Options {
        std::string outputFile;
        std::string logFile;
        std::string inputFile;
        std::size_t blockSize;
        std::vector<Condition> conditions;
    };

    Options parseArgs(int argc, char *argv[]);
    void validateOptions();
    void filterblocksContents();

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
