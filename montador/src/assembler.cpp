#include <assembler.hpp>

Assembler::Assembler(std::string fileName, std::list<std::tuple<int, std::list<std::string>>> srcLines) {
    this->fileName = fileName;
    this->srcLines = srcLines;
}

int Assembler::printSource() {
    if (error != 0) {
        return error;
    }
    for (auto lineTuple : srcLines) {
        printf("%3d:", std::get<0>(lineTuple));
        auto tokens = std::get<1>(lineTuple);
        for (auto token : tokens) {
            printf(" %s", token.c_str());
        }
        printf("\n");
    }
    return 0;
}

enum {
    NONE = 0,
    TEXT,
    DATA,
    BSS    
};

int Assembler::firstPass() {
    if (error != 0) {
        return error;
    }
    int memCount = 0;
    int section = NONE;
    
    for (auto lineIt = srcLines.begin(); lineIt != srcLines.end(); ++lineIt)
    {
        auto line = std::get<1>(*lineIt);
        auto lineCount = std::get<0>(*lineIt);

        // Handle section change
        if (line.front() == "SECTION") {
            // Section lines must always have 2 tokens
            if (line.size() != 2) {
                errMsg = "line " + std::to_string(lineCount) + ": section lines must always have 2 tokens";
                error = 1;
                return error;
            }
            // Change  the section variable according to the second token
            if (line.back() == "TEXT") {
                section = TEXT;
            } else if (line.back() == "DATA") {
                section = DATA;
            } else if (line.back() == "BSS") {
                section = BSS;
            } else {
                errMsg = "line " + std::to_string(lineCount) + ": unknown section name " + line.back();
                error = 1;
                return error;
            }
            continue;
        } else if (section == NONE) {
            errMsg = "line " + std::to_string(lineCount) + ": pre-processed code should begin with a SECTION line";
            error = 1;
            return error;
        }

        auto tokenIt = line.begin();

        // If line begins with label
        if (isSuffix(line.front(), ":")) {
            auto label = line.front();
            label.pop_back(); // Remove ':' from the label
            
            // Check if label matches valid regular expression
            std::regex labelRegEx("[a-zA-Z_][a-zA-Z0-9_]*");
            if (!std::regex_match(label, labelRegEx)) {
                errMsg = "line " + std::to_string(lineCount) + ": invalid label \'" + label + '\'';
                error = 1;
                return error;
            }

            // Check if label already exists
            if (symbolsMap.count(label) > 0) {
                errMsg = "line " + std::to_string(lineCount) + ": symbol redefinition";
                error = 1;
                return error;
            }

            symbolsMap[label] = memCount;
            ++tokenIt;
        }

        // If there are any other tokens, increment memCount accordingly
        if (tokenIt != line.end()) {
            auto token = *tokenIt;
            if (token == "SPACE") {
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt != line.end()) {
                    // TODO: check if next token is valid number
                    memCount += std::atoi((*nextTokenIt).c_str());
                } else {
                    memCount += 1;
                }
            } else {
                if(memSpaceMap.count(token) > 0) {
                    memCount += memSpaceMap.at(token);
                } else {
                    errMsg = "line " + std::to_string(lineCount) + ": instruction/directive \'" + token + "\' not defined in memSpaceMap";
                    error = 1;
                    return error;
                }
            }
        }
    }

    return 0;
}

std::string Assembler::getErrorMessage() {
    return errMsg;
}

int Assembler::getError() {
    return error;
}