#include "rotateblocks.h"

#include <cstring>
#include <getopt.h>
#include <memory>
#include <unistd.h>

int main(int argc, char *argv[]) {
    return rotateblocks::RotateBlocks(argc, argv).run();
}

namespace rotateblocks {

RotateBlocks::RotateBlocks(int argc, char *argv[]):
    options(parseArgs(argc,argv)),
    inputFile(options.inputFile, std::ios::binary | std::ios::in),
    outputFile(options.outputFile, std::ios::binary | std::ios::out | std::ios::trunc),
    logFile(options.logFile, std::ios::out | std::ios::trunc),
    inputStream(inputFile.is_open() ? inputFile.rdbuf() : std::cin.rdbuf()),
    outputStream(outputFile.is_open() ? outputFile.rdbuf() : std::cout.rdbuf()),
    logStream(logFile.is_open() ? logFile.rdbuf() : std::clog.rdbuf())
{
}

RotateBlocks::~RotateBlocks()
{
}

RotateBlocks::Options RotateBlocks::parseArgs(int argc, char *argv[])
{
    Options options;
    options.blockSize = 0;
    options.pageSize = 0;
    options.invertMarker = false;

    struct option longOptions[] = {
       { "output", required_argument, 0, 'o'},
       { "log", required_argument, 0, 'l'},
       { "block-size", required_argument, 0, 'b'},
       { "page-size", required_argument, 0, 'p'},
       { "marker", required_argument, 0, 'm'},
       { "invert-marker", no_argument, 0, 'i'},
       { 0, 0, 0, 0}
    };

    for (;;)  {
        int optionIndex = 0;
        int c = getopt_long(argc, argv, "io:l:b:p:m:i", longOptions, &optionIndex);
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
        case 'p':
            options.pageSize = std::stoul(optarg);
            break;
        case 'm':
            options.marker = BlockMarker(optarg);
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

int RotateBlocks::run()
{
    try {
        validateOptions();
        rotateBlocksContents();
    } catch (std::exception& e) {
        log(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void RotateBlocks::validateOptions()
{
    if (options.blockSize == 0)
        throw std::runtime_error("Block size cannot be zero!");
    if (options.pageSize == 0)
        throw std::runtime_error("Page size cannot be zero!");
    if (options.blockSize < options.pageSize)
        throw std::runtime_error("Block size cannot be smaller than page size!");
    if (options.blockSize % options.pageSize != 0)
        throw std::runtime_error("Block size must be multiple of page size!");
}

void RotateBlocks::rotateBlocksContents()
{

    const auto blockSize = options.blockSize;
    const auto pageSize = options.pageSize;
    const auto pageCount = blockSize / pageSize;
    auto buffer = std::make_unique<char[]>(blockSize);
    auto bufferPtr = buffer.get();
    unsigned int blockIndex = 0;

    log ("Page count per block is " + std::to_string(pageCount));
    while (!inputStream.eof()) {
        size_t bytesRead = inputStream.read(bufferPtr, blockSize).gcount();
        if (bytesRead == 0)
            break;

        if (bytesRead != blockSize)
            throw std::runtime_error("Partial read during rotating blocks: " + std::to_string(bytesRead) +
                                     " vs " + std::to_string(blockSize) + "bytes");

        size_t markerValue = options.marker.value(bufferPtr, blockSize, options.invertMarker);
        size_t pages = (markerValue) % pageCount;
        size_t shift = pageSize * pages;

        outputStream.write(bufferPtr + shift, blockSize - shift);
        if (shift) {
            log("Rotating block " + std::to_string(blockIndex) + " by " + std::to_string(pages) + " pages");
            outputStream.write(bufferPtr, shift);
        }
        ++blockIndex;
    }
}

void RotateBlocks::log(const std::string& message)
{
    logStream << message << std::endl;
}

}
