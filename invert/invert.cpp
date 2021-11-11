#include "invert.h"

#include <cstring>
#include <getopt.h>
#include <vector>
#include <unistd.h>
#include <algorithm>
#include <iterator>

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
    std::istreambuf_iterator<char>readerStart{inputStream};
    std::istreambuf_iterator<char>readerEnd{};
    std::ostreambuf_iterator<char>writer{outputStream};
    std::transform(
        readerStart,
        readerEnd,
        writer,
        [](const auto b) { return ~b; });
}

void Invert::log(const std::string& message)
{
    logStream << message << std::endl;
}

}
