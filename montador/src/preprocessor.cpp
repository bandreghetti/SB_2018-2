#include <preprocessor.hpp>

PreProcessor::PreProcessor(std::string fileName) {
    std::string asmName = fileName + ".asm";  // input file name
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
    while (getline(srcFile, line)) {
        auto lineTuple = std::make_tuple(lineCount, line);
        srcLines.push_back(lineTuple);
        ++lineCount;
    }
    srcFile.close();
}

PreProcessor::~PreProcessor() {}

void PreProcessor::printSource() {
    for (auto lineTuple : srcLines) {
        printf("%3d:%s\n", std::get<0>(lineTuple),
               std::get<1>(lineTuple).c_str());
    }
}

void PreProcessor::printOutput() {
    for (auto lineTuple : outLines) {
    }
}

void PreProcessor::preProcess() {
    std::map<std::string, int> equMap;
    for (auto lineTupleIt = srcLines.begin(); lineTupleIt != srcLines.end();
         ++lineTupleIt) {
        auto lineTuple = *lineTupleIt;
        int lineCount = std::get<0>(lineTuple);
        std::string line = std::get<1>(lineTuple);

        line = line.substr(0, line.find(";", 0));

        // If line is empty after removing spaces, remove it in pre-processing
        if (!std::all_of(line.begin(), line.end(), isspace)) {
            // Split line in tokens
            auto tokensInLine = tokenize(line);
            auto firstToken = tokensInLine.front();
            auto secondToken = *std::next(tokensInLine.begin());

            // Check if first token is a label
            if (isSuffix(firstToken, ":")) {
                // Check if label is an EQU label
                if (secondToken == "EQU") {
                    auto equLabel = firstToken;
                    auto equValStr = *std::next(tokensInLine.begin(), 2);
                    auto equVal = atoi(equValStr.c_str());
                    auto equTuple = std::make_tuple(equLabel, equVal);
                    equMap[equLabel] = equVal;
                    continue;
                }
            }

            if (firstToken == "IF" || secondToken == "IF") {
                // TODO: handle IF directive
            }

            // TODO: finish handling EQU directive
            // TODO: push_back outTuple to outLines properly

            auto outTuple = std::make_tuple(lineCount, line);
        }
    }
}
