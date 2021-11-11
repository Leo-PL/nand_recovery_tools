#include "joinblocks.h"

#include <algorithm>
#include <cstring>
#include <getopt.h>
#include <memory>
#include <numeric>
#include <unistd.h>

int main(int argc, char *argv[]) {
    return joinblocks::JoinBlocks(argc, argv).run();
}

namespace joinblocks {

JoinBlocks::JoinBlocks(int argc, char *argv[]):
    options(parseArgs(argc,argv)),
    outputFile(options.outputFile, std::ios::binary | std::ios::out | std::ios::trunc),
    logFile(options.logFile, std::ios::out | std::ios::trunc),
    outputStream(outputFile.is_open() ? outputFile.rdbuf() : std::cout.rdbuf()),
    logStream(logFile.is_open() ? logFile.rdbuf() : std::clog.rdbuf())
{
}

JoinBlocks::~JoinBlocks()
{
}

JoinBlocks::Options JoinBlocks::parseArgs(int argc, char *argv[])
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
            options.outputFile = optarg;
            break;
        case 'l':
            options.logFile = optarg;
            break;
        case 's':
            options.blockSizes.push_back(std::stoul(optarg));
            break;
        }
    }

    while (optind < argc) {
        options.inputFiles.push_back(argv[optind]);
        ++optind;
    }

    return options;
}

void JoinBlocks::validateOptions()
{
    if (options.inputFiles.empty())
        throw std::runtime_error("No output files specified, please use -o option.");
    if (options.blockSizes.empty())
        throw std::runtime_error("No output block sizes specified, please use -s option");
    if (options.inputFiles.size() != options.blockSizes.size())
        throw std::runtime_error("Mismatch between number of output files and output block sizes");
    if (std::any_of(options.inputFiles.cbegin(),
                    options.inputFiles.cend(),
                    [](std::string name){ return name.empty(); }))
        throw std::runtime_error("At least one of output names is empty");
    if (std::any_of(options.blockSizes.cbegin(),
                    options.blockSizes.cend(),
                    [](int size){ return (size == 0); }))
        throw std::runtime_error("At least one of output block sizes is zero");
}

void JoinBlocks::openInputFiles()
{
    for (size_t index = 0; index < options.inputFiles.size(); ++index) {
        Input in;
        in.blockSize = options.blockSizes[index];
        in.file.open(options.inputFiles[index], std::ios::binary | std::ios::in);
        if (!in.file.is_open())
            throw std::runtime_error("Cannot open file " + options.inputFiles[index]);

        inputFiles.push_back(std::move(in));
    }
}

int JoinBlocks::run()
{
    try {
        validateOptions();
        openInputFiles();
        joinInputBlocks();
    } catch (const std::exception& e) {
        log(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


void JoinBlocks::joinInputBlocks()
{
    size_t bufferSize = *std::max_element(options.blockSizes.cbegin(), options.blockSizes.cend());
    auto buffer = std::make_unique<char[]>(bufferSize);
    auto bufferPtr = buffer.get();
    bool finished = false;

    while (!finished) {
        for (auto& in: inputFiles) {
            const size_t readSize = in.blockSize;
            size_t bytesRead = in.file.read(bufferPtr, readSize).gcount();

            if (in.file.eof() || bytesRead == 0) {
                finished = true;
                break;
            }

            if (bytesRead != readSize)
                throw std::runtime_error("Partial read during jointing data: " + std::to_string(bytesRead) +
                                         " vs " + std::to_string(readSize) + " bytes");
            outputStream.write(bufferPtr, bytesRead);
        }
    }
}

void JoinBlocks::log(const std::string& message)
{
    logStream << message << std::endl;
}

}
