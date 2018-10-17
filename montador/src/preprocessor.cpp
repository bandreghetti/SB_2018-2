#include <preprocessor.hpp>

PreProcessor::PreProcessor(std::string fileName)
{
    std::string asmName = fileName + ".asm"; // input file name
    if (!fileExists(asmName)) {
        std::cout << "File " + asmName + " does not exists\n";
        error = -1;
        return;
    }

    // Read all lines from file
    std::ifstream srcFile;
    srcFile.open(asmName);
    std::string line;
    unsigned int lineCount = 0;
    while(getline(srcFile, line)) {
        auto lineTuple = std::make_tuple(lineCount, line); 
        srcLines.push_back(lineTuple);
        ++lineCount;
    }
    srcFile.close();
}

PreProcessor::~PreProcessor()
{
}

void PreProcessor::printSource() {
    for(auto lineTuple : srcLines) {
        std::cout << std::get<0>(lineTuple) << ":\t" + std::get<1>(lineTuple) << "\n";
    }
}
