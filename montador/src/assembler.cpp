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

int Assembler::writeOutput() {
    if (error != 0) {
        return error;
    }
    std::string objName = fileName + ".obj";  // output file name
    std::ofstream outFile;
    outFile.open(objName);

    // Write RELATIVE section to object file
    outFile << "RELATIVE\n";
    for (auto rel = relative.begin(); rel != relative.end(); ++rel) {
        if(std::next(rel) != relative.end())
        {
            outFile << std::to_string(*rel) + " ";
        }
    }
    outFile << std::to_string(relative.back()) + "\n";
    
    // Write CODE section to object file
    outFile << "CODE\n";
    for (auto code = machineCode.begin(); code != machineCode.end(); ++code) {
        if(std::next(code) != machineCode.end())
        {
            outFile << std::to_string(*code) + " ";
        }
    }
    outFile << std::to_string(machineCode.back()) + "\n";

    outFile.close();
    
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

int Assembler::secondPass() {
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
            ++tokenIt;
        }

        if (tokenIt != line.end()) {
            auto op = *tokenIt;
            switch (section) {
            case BSS: 
                if (op == "SPACE") {
                    int nSpaces;
                    if (std::next(tokenIt) != line.end()) {
                        ++tokenIt;
                        nSpaces = std::atoi((*tokenIt).c_str());
                    } else {
                        nSpaces = 1;
                    }
                    for (;nSpaces > 0; --nSpaces) {
                        ++memCount;
                        machineCode.push_back(0);
                    }
                    // Check if there's any unexpected token after SPACE
                    if (std::next(tokenIt) != line.end()) {
                        errMsg = genErrMsg(lineCount, "found " + *std::next(tokenIt) + ", expected newline");
                        error = 1;
                        return error;
                    }
                } else {
                    errMsg = genErrMsg(lineCount, "non-SPACE operator/directive in BSS (uninitialized data) section");
                    error = 1;
                    return error;
                }
                break;
            case DATA:
                if (op == "CONST") {
                    if (std::next(tokenIt) != line.end()) {
                        ++tokenIt;
                        auto intRegEx = std::regex("(\\+|-)?[0-9]+");
                        auto hexRegEx = std::regex("(0x)[0-9a-fA-F]{1,4}");                        
                        if(std::regex_match(*tokenIt, intRegEx)) {
                            machineCode.push_back(std::atoi((*tokenIt).c_str()));
                        } else if (std::regex_match(*tokenIt, hexRegEx)) {
                            // TODO: find out how to convert hex to int an then to string
                        }
                        ++memCount;
                        if (std::next(tokenIt) != line.end()) {
                            errMsg = genErrMsg(lineCount, "found " + *std::next(tokenIt) + ", expected newline");
                            error = 1;
                            return error;
                        } 
                    } else {
                        errMsg = genErrMsg(lineCount, "newline found, expected immediate matching regular expression (0x|+|-)?[0-9a-fA-F]");
                        error = 1;
                        return error;
                    }
                } else {
                    errMsg = genErrMsg(lineCount, "non-CONST operator/directive in DATA section");
                    error = 1;
                    return error;
                }
                break;
            case TEXT:
                if (op == "SPACE" || op == "CONST") {
                    errMsg = genErrMsg(lineCount, op + " directive in TEXT section");
                    error = 1;
                    return error;
                }
                if (opcodeMap.count(op) > 0) {
                    auto opcode = opcodeMap.at(op);
                    machineCode.push_back(opcode);
                    ++memCount;
                    switch (opcode) {
                    case ADD:
                    case SUB:
                    case MULT:
                    case DIV:
                    case JMP:
                    case JMPN:
                    case JMPP:
                    case JMPZ:
                    case LOAD:
                    case STORE:
                    case INPUT:
                    case OUTPUT:
                        if (std::next(tokenIt) != line.end()) {
                            ++tokenIt;
                            auto operand = *tokenIt;
                            if (symbolsMap.count(operand) > 0) {
                                auto memOperand = symbolsMap.at(operand);
                                machineCode.push_back(memOperand);
                                relative.push_back(memCount);
                                ++memCount;
                            } else {
                                errMsg = genErrMsg(lineCount, "unknown symbol " + operand);
                                error = 1;
                                return error;
                            }
                            if (std::next(tokenIt) != line.end()) {
                                errMsg = genErrMsg(lineCount, "expecting newline, found " + *std::next(tokenIt));
                                error = 1;
                                return error;
                            }
                        } else {
                            errMsg = genErrMsg(lineCount, "expecting 1 operator, found none");
                            error = 1;
                            return error;
                        }
                        break;
                    case COPY:
                        // TODO: handle copy operator
                        break;
                    case STOP:
                        if (std::next(tokenIt) != line.end()) {
                            errMsg = genErrMsg(lineCount, "expecting newline, found " + *std::next(tokenIt));
                            error = 1;
                            return error;
                        }
                        break;
                    }
                } else {
                    errMsg = genErrMsg(lineCount, "unknown " + op + " operator");
                    error = 1;
                    return error;
                }
                break;
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

std::string Assembler::genErrMsg(int lineCount, std::string message) {
    return "line " + std::to_string(lineCount) + ": " + message;
}