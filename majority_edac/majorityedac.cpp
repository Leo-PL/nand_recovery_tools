#include "majorityedac.h"

#include <cstring>
#include <getopt.h>
#include <memory>
#include <unistd.h>

int main(int argc, char *argv[]) {
    return majorityedac::MajorityEdac(argc, argv).run();
}

namespace majorityedac {

MajorityEdac::MajorityEdac(int argc, char *argv[]):
    options(parseArgs(argc,argv)),
    outputFile(options.outputFile, std::ios::binary | std::ios::out | std::ios::trunc),
    logFile(options.logFile, std::ios::out | std::ios::trunc),
    outputStream(outputFile.is_open() ? outputFile.rdbuf() : std::cout.rdbuf()),
    logStream(logFile.is_open() ? logFile.rdbuf() : std::clog.rdbuf())
{
}

MajorityEdac::~MajorityEdac()
{
}

MajorityEdac::Options MajorityEdac::parseArgs(int argc, char *argv[])
{
    Options options;

    struct option longOptions[] = {
       { "output", required_argument, 0, 'o'},
       { "log", required_argument, 0, 'l'},
       { 0, 0, 0, 0}
    };

    for (;;) {
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

    while (optind < argc)
    {
        options.inputFiles.push_back(argv[optind]);
        ++optind;
    }

    return options;
}

int MajorityEdac::run()
{
    try {
        validateInputFileCount();
        findBitflipsInFiles();
        reconstructFile();
    } catch (std::exception& e) {
        log(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void MajorityEdac::validateInputFileCount()
{
    auto fileCount = options.inputFiles.size();
    if (fileCount == 0)
        throw std::runtime_error("No input files.");

    if (fileCount % 2 == 0)
        throw std::runtime_error("Expected odd count of input images");
}

void MajorityEdac::findBitflipsInFiles()
{
    const auto& inputFilenames = options.inputFiles;
    for(auto iter = inputFilenames.begin() + 1; iter != inputFilenames.end(); ++iter)
    {
        auto bitflips = findBitflipsInPair(inputFilenames[0], *iter);
        for (auto bitflip: bitflips) {
            bitFlipMap[bitflip.first].addSamples(bitflip.second, 1);
        }
    }

}

MajorityEdac::BitFlipVector MajorityEdac::findBitflipsInPair(const std::string& referenceFilename,
                                                             const std::string& comparedFilename)
{
    std::ifstream referenceFile(referenceFilename, std::ios::binary | std::ios::in);
    std::ifstream comparedFile(comparedFilename, std::ios::binary | std::ios::in);
    if (!referenceFile.is_open())
        throw std::runtime_error("Failed to open reference file: " + referenceFilename);

    if (!comparedFile.is_open())
        throw std::runtime_error("Failed to open compared file: " + referenceFilename);

    auto referenceBuffer = std::make_unique<char[]>(DEFAULT_BLOCK_SIZE);
    auto comparedBuffer = std::make_unique<char[]>(DEFAULT_BLOCK_SIZE);

    BitFlipVector result;
    ByteOffset currentOffset = 0;

    while (!(referenceFile.eof() || comparedFile.eof()))
    {
        auto bytesReadFromReference = referenceFile.read(referenceBuffer.get(), DEFAULT_BLOCK_SIZE).gcount();
        auto bytesReadFromCompared = comparedFile.read(comparedBuffer.get(), DEFAULT_BLOCK_SIZE).gcount();
        if (bytesReadFromReference != bytesReadFromCompared)
            throw std::runtime_error("Reference file (" + referenceFilename +
                                     ") and compared filename (" + comparedFilename +
                                     ") differ in size (" + std::to_string(bytesReadFromReference) +
                                     " vs " + std::to_string(bytesReadFromCompared) +
                                     " bytes)");

        compareBuffers(referenceBuffer.get(), comparedBuffer.get(), currentOffset, bytesReadFromCompared, result);
        currentOffset += bytesReadFromReference;
    }
    return result;
}

void MajorityEdac::compareBuffers(char* const referenceBuffer,
                                  char* const comparedBuffer,
                                  MajorityEdac::ByteOffset offset,
                                  size_t bufferSize,
                                  BitFlipVector &result)
{
    using Word = unsigned long long;
    const Word* const reference = reinterpret_cast<Word*>(referenceBuffer);
    const Word* const compared = reinterpret_cast<Word*>(comparedBuffer);
    const unsigned int maxIndex = bufferSize / sizeof(Word);

    for(unsigned int index(0); index < maxIndex; ++index)
    {
        Word syndrome = reference[index] ^ compared[index];

        while(syndrome)
        {
            int bitIndex = ffsll(syndrome) - 1;

            Word mask = 1ULL << bitIndex;
            auto byteOffset = index * sizeof(Word) + (bitIndex >> 3);
            if(byteOffset > bufferSize)
                break;

            auto bitOffset = bitIndex & 0x7;
            auto bitValue = !!(compared[index] & mask);

            result.push_back(std::make_pair(BitIndex(byteOffset + offset, bitOffset), bitValue));

            syndrome &= ~mask;
        }
    }
}

void MajorityEdac::reconstructFile()
{
    const std::string& inputFilename = options.inputFiles[0];
    std::ifstream inputStream(inputFilename, std::ios::binary | std::ios::in);
    if (!inputStream.is_open())
        throw std::runtime_error("Failed to open reference file: " + inputFilename);

    auto buffer = std::make_unique<char[]>(DEFAULT_BLOCK_SIZE);
    auto bufferPtr = buffer.get();
    ByteOffset offset = 0;
    const auto imageCount = options.inputFiles.size();

    while (!inputStream.eof()) {
        auto bytesRead = inputStream.read(bufferPtr, DEFAULT_BLOCK_SIZE).gcount();

        auto lowerBound = bitFlipMap.lower_bound(BitIndex(offset, 0));
        auto upperBound = bitFlipMap.upper_bound(BitIndex(offset + DEFAULT_BLOCK_SIZE - 1, 0));
        for (auto iter = lowerBound; iter != upperBound; ++iter) {
            const auto& index = iter->first;
            auto& value = iter->second;

            size_t localOffset = index.byte - offset;
            int mask = (1 << index.bit);

            BitValue refValue = !!(buffer[localOffset] & mask);
            value.addSamples(refValue, imageCount - value.samplesCount);

            log("Correcting bit flip at " + std::to_string(index.byte) +
                ":" + std::to_string(index.bit) +
                " to " + std::to_string((unsigned int) value.isOne()) +
                " weight: " + std::to_string(value.onesCount) +
                "/" + std::to_string(value.samplesCount));

            if(value.isOne())
                buffer[localOffset] |= mask;
            else
                buffer[localOffset] &= ~mask;

        }
        outputStream.write(bufferPtr, bytesRead);
        offset += bytesRead;
    }
}

void MajorityEdac::log(const std::string& message)
{
    logStream << message << std::endl;
}

}
