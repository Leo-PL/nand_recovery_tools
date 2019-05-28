#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace joinblocks
{

class JoinBlocks {
public:
    JoinBlocks(int argc, char *argv[]);
    ~JoinBlocks();
    int run();

private:
    using ByteOffset = unsigned long long int;
    struct Options {
        std::string outputFile;
        std::string logFile;
        std::vector<std::string> inputFiles;
        std::vector<size_t> blockSizes;
    };
    struct Input {
        std::ifstream file;
        size_t blockSize;
    };

    Options parseArgs(int argc, char *argv[]);
    void validateOptions();
    void openInputFiles();
    void joinInputBlocks();

    void log(const std::string& message);
    Options options;
    std::ofstream outputFile;
    std::ofstream logFile;
    std::ostream outputStream;
    std::ostream logStream;
    std::vector<Input> inputFiles;
};

}
