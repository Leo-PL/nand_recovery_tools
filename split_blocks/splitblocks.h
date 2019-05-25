#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace splitblocks
{

class SplitBlocks {
public:
    SplitBlocks(int argc, char *argv[]);
    ~SplitBlocks();
    int run();

private:
    using ByteOffset = unsigned long long int;
    struct Options {
        std::string inputFile;
        std::string logFile;
        std::vector<std::string> outputFiles;
        std::vector<size_t> blockSizes;
    };
    struct Output {
        std::ofstream file;
        size_t blockSize;
    };

    Options parseArgs(int argc, char *argv[]);
    void validateOptions();
    void openOutputFiles();
    void splitInputBlocks();

    void log(const std::string& message);
    Options options;
    std::ifstream inputFile;
    std::ofstream logFile;
    std::istream inputStream;
    std::ostream logStream;
    std::vector<Output> outputFiles;
};

}
