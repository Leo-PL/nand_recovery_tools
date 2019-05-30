#include "filterblocks.h"

#include <algorithm>
#include <cstring>
#include <getopt.h>
#include <memory>
#include <unistd.h>

int main(int argc, char *argv[]) {
    return filterblocks::FilterBlocks(argc, argv).run();
}

namespace filterblocks {

FilterBlocks::FilterBlocks(int argc, char *argv[]):
    options(parseArgs(argc,argv)),
    inputFile(options.inputFile, std::ios::binary | std::ios::in),
    outputFile(options.outputFile, std::ios::binary | std::ios::out | std::ios::trunc),
    logFile(options.logFile, std::ios::out | std::ios::trunc),
    inputStream(inputFile.is_open() ? inputFile.rdbuf() : std::cin.rdbuf()),
    outputStream(outputFile.is_open() ? outputFile.rdbuf() : std::cout.rdbuf()),
    logStream(logFile.is_open() ? logFile.rdbuf() : std::clog.rdbuf())
{
}

FilterBlocks::~FilterBlocks()
{
}

FilterBlocks::Options FilterBlocks::parseArgs(int argc, char *argv[])
{
    Options options;
    options.blockSize = 0;
    options.invertCondition = false;

    struct option longOptions[] = {
       { "output", required_argument, 0, 'o'},
       { "log", required_argument, 0, 'l'},
       { "size", required_argument, 0, 's'},
       { "condition", required_argument, 0, 'c'},
       { "invert", no_argument, 0, 'i'},
       { 0, 0, 0, 0}
    };

    for (;;)  {
        int optionIndex = 0;
        int c = getopt_long(argc, argv, "o:l:s:c:i", longOptions, &optionIndex);
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
        case 's':
            options.blockSize = std::stoul(optarg);
            break;
        case 'c':
            options.conditions.push_back(std::move(Condition(optarg)));
            break;
        case 'i':
            options.invertCondition = true;
            break;
        }
    }

    if (optind < argc)
        options.inputFile = argv[optind];

    return options;
}

int FilterBlocks::run()
{
    try {
        validateOptions();
        filterblocksContents();
    } catch (std::exception& e) {
        log(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void FilterBlocks::validateOptions()
{
    if (options.blockSize == 0)
        throw std::runtime_error("Block size cannot be zero!");
}

void FilterBlocks::filterblocksContents()
{
    const auto blockSize = options.blockSize;
    auto buffer = std::make_unique<char[]>(blockSize);
    auto bufferPtr = buffer.get();

    while (!inputStream.eof()) {
        auto bytesRead = inputStream.read(bufferPtr, blockSize).gcount();
        if (bytesRead == 0)
            break;
        if (std::all_of(options.conditions.cbegin(),
                       options.conditions.cend(),
                       [bufferPtr, blockSize](const Condition& c){ return c.matches(bufferPtr, blockSize); }) !=
            options.invertCondition)
            outputStream.write(bufferPtr, bytesRead);
    }
}

void FilterBlocks::log(const std::string& message)
{
    logStream << message << std::endl;
}

}
