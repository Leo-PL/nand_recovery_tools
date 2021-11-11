#include "arrangeblocks.h"

#include <cstring>
#include <getopt.h>
#include <memory>
#include <unistd.h>

int main(int argc, char *argv[]) {
    return arrangeblocks::Arrangeblocks(argc, argv).run();
}

namespace arrangeblocks {

Arrangeblocks::Arrangeblocks(int argc, char *argv[]):
    options(parseArgs(argc,argv)),
    inputFile(options.inputFile, std::ios::binary | std::ios::in),
    outputFile(options.outputFile, std::ios::binary | std::ios::out | std::ios::trunc),
    logFile(options.logFile, std::ios::out | std::ios::trunc),
    inputStream(inputFile.is_open() ? inputFile.rdbuf() : std::cin.rdbuf()),
    outputStream(outputFile.is_open() ? outputFile.rdbuf() : std::cout.rdbuf()),
    logStream(logFile.is_open() ? logFile.rdbuf() : std::clog.rdbuf())
{
}

Arrangeblocks::~Arrangeblocks()
{
}

Arrangeblocks::Options Arrangeblocks::parseArgs(int argc, char *argv[])
{
    Options options;
    options.blockSize = 0;
    options.pageSize = 0;
    options.invertMarker = false;

    struct option longOptions[] = {
       { "output", required_argument, 0, 'o'},
       { "log", required_argument, 0, 'l'},
       { "block-size", required_argument, 0, 'b'},
       { "marker", required_argument, 0, 'm'},
       { "invert-marker", no_argument, 0, 'i'},
       { 0, 0, 0, 0}
    };

    for (;;)  {
        int optionIndex = 0;
        int c = getopt_long(argc, argv, "io:l:b:m:i", longOptions, &optionIndex);
        if (c == -1)
            break;

        switch (c)
        {
        case 'o':
            options.outputFile = optarg;
            break;
        case 'l':
            options.logFile = optarg;
            break;
        case 'b':
            options.blockSize = std::stoul(optarg);
            break;
        case 'm':
            options.markers.push_back(std::move(BlockMarker(optarg)));
            break;
        case 'i':
            options.invertMarker = true;
            break;
        }
    }

    if (optind < argc)
        options.inputFile = argv[optind];

    return options;
}

int Arrangeblocks::run()
{
    try {
        validateOptions();
        assembleBlocks();
    } catch (const std::exception& e) {
        log(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void Arrangeblocks::validateOptions()
{
    if (options.blockSize == 0)
        throw std::runtime_error("Block size cannot be zero!");
}

void Arrangeblocks::assembleBlocks()
{

    const auto blockSize = options.blockSize;
    auto buffer = std::make_unique<char[]>(blockSize);
    auto bufferPtr = buffer.get();

    while (!inputStream.eof()) {
        size_t bytesRead = inputStream.read(bufferPtr, blockSize).gcount();
        if (bytesRead == 0)
            break;

        if (bytesRead != blockSize)
            throw std::runtime_error("Partial read during rotating blocks: " + std::to_string(bytesRead) +
                                     " vs " + std::to_string(blockSize) + "bytes");

        size_t blockIndex = extractLogicalBlockNumber(bufferPtr, blockSize);
        size_t offset = blockIndex * blockSize;

        log("Mapping block " + std::to_string(blockIndex) + " to offset " + std::to_string(offset));
        //std::seekp(offset);
        //outputStream.write(bufferPtr, blockSize);
    }
}

size_t Arrangeblocks::extractLogicalBlockNumber(char *block, size_t size)
{
    size_t ret = 0;
    unsigned int shift = 0;

    for (const auto& marker: options.markers)
    {
        ret |= marker.value(block, size, options.invertMarker) << shift;
        shift += marker.valueBits();
    }

    return ret;
}

void Arrangeblocks::log(const std::string& message)
{
    logStream << message << std::endl;
}

}
