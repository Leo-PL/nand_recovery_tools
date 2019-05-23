#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "bitindex.h"
#include "bitflip.h"

namespace majorityedac
{

class MajorityEdac {
public:
    MajorityEdac(int argc, char *argv[]);
    ~MajorityEdac();
    int run();

private:
    using ByteOffset = unsigned long long;
    using BitValue = bool;
    using BitFlipVector = std::vector<std::pair<BitIndex, BitValue> >;
    using BitFlipMap = std::map<BitIndex, BitFlip>;

    struct Options {
        std::string outputFile;
        std::string logFile;
        std::vector<std::string> inputFiles;
    };

    static const size_t DEFAULT_BLOCK_SIZE = 16384;

    Options parseArgs(int argc, char *argv[]);
    void redirectOutputs();
    void validateInputFileCount();
    void findBitflipsInFiles();
    BitFlipVector findBitflipsInPair(
            const std::string& referenceFilename,
            const std::string& comparedFilename);
    void compareBuffers(char* const referenceBuffer,
                        char* const comparetBuffer,
                        ByteOffset offset,
                        size_t bufferSize,
                        BitFlipVector &result);
    void reconstructFile();

    void log(const std::string& message);
    Options options;
    std::ofstream outputFile;
    std::ofstream logFile;
    std::ostream outputStream;
    std::ostream logStream;
    BitFlipMap bitFlipMap;
};

}
