#include "invert.h"

#include <cstring>
#include <getopt.h>
#include <memory>
#include <unistd.h>

int main(int argc, char *argv[]) {
    return invert::Invert(argc, argv).run();
}

namespace invert {

Invert::Invert(int argc, char *argv[]):
    options(parseArgs(argc,argv)),
    inputFile(options.inputFile, std::ios::binary | std::ios::in),
    outputFile(options.outputFile, std::ios::binary | std::ios::out | std::ios::trunc),
    logFile(options.logFile, std::ios::out | std::ios::trunc),
    inputStream(inputFile.is_open() ? inputFile.rdbuf() : std::cin.rdbuf()),
    outputStream(outputFile.is_open() ? outputFile.rdbuf() : std::cout.rdbuf()),
    logStream(logFile.is_open() ? logFile.rdbuf() : std::clog.rdbuf())
{
}

Invert::~Invert()
{
}

Invert::Options Invert::parseArgs(int argc, char *argv[])
{
    Options options;

    struct option longOptions[] = {
       { "output", required_argument, 0, 'o'},
       { "log", required_argument, 0, 'l'},
       { 0, 0, 0, 0}
    };

    for (;;)  {
        int optionIndex = 0;
        int c = getopt_long(argc, argv, "o:l:", longOptions, &optionIndex);
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
        }
    }

    if (optind < argc)
        options.inputFile = argv[optind];

    return options;
}

int Invert::run()
{
    try {
        invertContents();
    } catch (std::exception& e) {
        log(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


void Invert::invertContents()
{
    using Word = unsigned long long int;
    auto buffer = std::make_unique<char[]>(DEFAULT_BLOCK_SIZE);
    auto bufferPtr = buffer.get();
    auto bufferPtrWord = reinterpret_cast<Word*>(bufferPtr);
    ByteOffset offset = 0;

    while (!inputStream.eof()) {
        auto bytesRead = inputStream.read(bufferPtr, DEFAULT_BLOCK_SIZE).gcount();
        if (bytesRead == 0)
            break;

        for(unsigned int index = 0; index < bytesRead / sizeof(Word); ++index)
            bufferPtrWord[index] ^= ~0ULL;

        outputStream.write(bufferPtr, bytesRead);
        offset += bytesRead;
    }
}

void Invert::log(const std::string& message)
{
    logStream << message << std::endl;
}

}
