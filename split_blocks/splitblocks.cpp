#include "splitblocks.h"

#include <algorithm>
#include <cstring>
#include <getopt.h>
#include <memory>
#include <numeric>
#include <unistd.h>

int main(int argc, char *argv[]) {
    return splitblocks::SplitBlocks(argc, argv).run();
}

namespace splitblocks {

SplitBlocks::SplitBlocks(int argc, char *argv[]):
    options(parseArgs(argc,argv)),
    inputFile(options.inputFile, std::ios::binary | std::ios::in),
    logFile(options.logFile, std::ios::out | std::ios::trunc),
    inputStream(inputFile.is_open() ? inputFile.rdbuf() : std::cin.rdbuf()),
    logStream(logFile.is_open() ? logFile.rdbuf() : std::clog.rdbuf())
{
}

SplitBlocks::~SplitBlocks()
{
}

SplitBlocks::Options SplitBlocks::parseArgs(int argc, char *argv[])
{
    Options options;

    struct option longOptions[] = {
       { "output", required_argument, 0, 'o'},
       { "log", required_argument, 0, 'l'},
       { "size", required_argument, 0, 's'},
       { 0, 0, 0, 0}
    };

    for (;;) {
        int optionIndex = 0;
        int c = getopt_long(argc, argv, "o:l:s:", longOptions, &optionIndex);
        if (c == -1)
            break;

        switch (c)
        {
        case 'o':
            options.outputFiles.push_back(optarg);
            break;
        case 'l':
            options.logFile = optarg;
            break;
        case 's':
            options.blockSizes.push_back(std::stoul(optarg));
            break;
        }
    }

    if (optind < argc)
        options.inputFile = argv[optind];

    return options;
}

void SplitBlocks::validateOptions()
{
    if (options.outputFiles.empty())
        throw std::runtime_error("No output files specified, please use -o option.");
    if (options.blockSizes.empty())
        throw std::runtime_error("No output block sizes specified, please use -s option");
    if (options.outputFiles.size() != options.blockSizes.size())
        throw std::runtime_error("Mismatch between number of output files and output block sizes");
    if (std::any_of(options.outputFiles.cbegin(),
                    options.outputFiles.cend(),
                    [](std::string name){ return name.empty(); }))
        throw std::runtime_error("At least one of output names is empty");
    if (std::any_of(options.blockSizes.cbegin(),
                    options.blockSizes.cend(),
                    [](int size){ return (size == 0); }))
        throw std::runtime_error("At least one of output block sizes is zero");
}

void SplitBlocks::openOutputFiles()
{
    for (size_t index = 0; index < options.outputFiles.size(); ++index) {
        Output out;
        out.blockSize = options.blockSizes[index];
        out.file.open(options.outputFiles[index], std::ios::binary | std::ios::out | std::ios::trunc);
        if (!out.file.is_open())
            throw std::runtime_error("Cannot open file " + options.outputFiles[index]);

        outputFiles.push_back(std::move(out));
    }
}

int SplitBlocks::run()
{
    try {
        validateOptions();
        openOutputFiles();
        splitInputBlocks();
    } catch (const std::exception& e) {
        log(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


void SplitBlocks::splitInputBlocks()
{
    size_t readSize = std::accumulate(options.blockSizes.cbegin(),
                                      options.blockSizes.cend(),
                                      0);
    auto buffer = std::make_unique<char[]>(readSize);
    auto bufferPtr = buffer.get();

    while (!inputStream.eof()) {
        size_t bytesRead = inputStream.read(bufferPtr, readSize).gcount();
        if (bytesRead == 0)
            break;

        if (bytesRead != readSize)
            throw std::runtime_error("Partial read during splitting data: " + std::to_string(bytesRead) +
                                     " vs " + std::to_string(readSize) + "bytes");

        size_t offset = 0;

        for (auto& out: outputFiles) {
            const size_t writeSize = out.blockSize;
            out.file.write(bufferPtr + offset, writeSize);
            offset += writeSize;
        }
    }
}

void SplitBlocks::log(const std::string& message)
{
    logStream << message << std::endl;
}

}
