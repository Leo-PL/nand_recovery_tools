#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

namespace invert
{

class Invert {
public:
    Invert(int argc, char *argv[]);
    ~Invert();
    int run();

private:
    using ByteOffset = std::size_t;
    struct Options {
        std::string outputFile;
        std::string logFile;
        std::string inputFile;
    };

    static const size_t DEFAULT_BLOCK_SIZE = 16384;

    Options parseArgs(int argc, char *argv[]);
    void invertContents();

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
