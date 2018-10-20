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
            // Section lines must always have 2 tokens (SECTION <section_name>)
            if (line.size() != 2) {
                errMsg = genErrMsg(lineCount, "section lines must always have 2 tokens");
                error = 1;
                return error;
            }
            // Change the section variable according to the second token
            if (line.back() == "TEXT") {
                section = TEXT;
            } else if (line.back() == "DATA") {
                section = DATA;
            } else if (line.back() == "BSS") {
                section = BSS;
            } else {
                errMsg = genErrMsg(lineCount, "unknown section name " + line.back());
                error = 1;
                return error;
            }
            continue;
        } else if (section == NONE) {
            errMsg = genErrMsg(lineCount, "pre-processed code should begin with a SECTION line");
            error = 1;
            return error;
        }

        // Get iterator to the first token in line
        auto tokenIt = line.begin();

        // If line begins with label
        if (isSuffix(line.front(), ":")) {
            auto label = line.front();
            label.pop_back(); // Remove ':' from the label

            // Check if label matches valid regular expression
            std::regex labelRegEx("[a-zA-Z_][a-zA-Z0-9_]*");
            if (!std::regex_match(label, labelRegEx)) {
                errMsg = genErrMsg(lineCount, "invalid label " + label);
                error = 1;
                return error;
            }

            // Check if label already exists
            if (symbolsMap.count(label) > 0) {
                errMsg = genErrMsg(lineCount, "symbol redefinition");
                error = 1;
                return error;
            }

            // If everything's ok, add symbol to symbols table
            symbolsMap[label] = memCount;
            // Advance to second token in line
            ++tokenIt;
        }

        // If there are any other tokens, increment memCount accordingly
        if (tokenIt != line.end()) {
            auto token = *tokenIt;
            // If token is SPACE, handle possible argument for space reserving
            if (token == "SPACE") {
                // Check whether SPACE was given an argument or not
                auto nextTokenIt = std::next(tokenIt);
                if (nextTokenIt != line.end()) {
                    // Since an argument was given, check if it is valid
                    auto intRegEx = std::regex("[0-9]+");
                    if (std::regex_match(*nextTokenIt, intRegEx)) {
                        memCount += std::atoi((*nextTokenIt).c_str());
                    } else {
                        errMsg = genErrMsg(lineCount, "invalid argument for SPACE directive: " + *nextTokenIt);
                        error = 1;
                        return error;
                    }
                } else {
                    // Since no argument was given, reserve only one space
                    memCount += 1;
                }
            } else {
                // Since instruction/directive was not SPACE, check if it is defined
                if(memSpaceMap.count(token) > 0) {
                    // If defined, reserve space for the instruction/directive accordingly
                    memCount += memSpaceMap.at(token);
                } else {
                    errMsg = genErrMsg(lineCount, "instruction/directive " + token + " not defined");
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
            // Section lines must always have 2 tokens (SECTION <section_name>)
            if (line.size() != 2) {
                errMsg = genErrMsg(lineCount, "section lines must always have 2 tokens");
                error = 1;
                return error;
            }
            // Change the section variable according to the second token
            if (line.back() == "TEXT") {
                section = TEXT;
            } else if (line.back() == "DATA") {
                section = DATA;
            } else if (line.back() == "BSS") {
                section = BSS;
            } else {
                errMsg = genErrMsg(lineCount, "unknown section name " + line.back());
                error = 1;
                return error;
            }
            continue;
        } else if (section == NONE) {
            errMsg = genErrMsg(lineCount, "pre-processed code should begin with a SECTION line");
            error = 1;
            return error;
        }

        // Get iterator to the first token in line
        auto tokenIt = line.begin();

        // If line begins with label, ignore it
        if (isSuffix(line.front(), ":")) {
            ++tokenIt;
        }

        // If there is an instruction/directive in this line
        if (tokenIt != line.end()) {
            auto op = *tokenIt;
            // Handle operators according to which section we are in
            switch (section) {
            case BSS:
                // SPACE is the only supported directive in BSS section
                if (op == "SPACE") {
                    int nSpaces;
                    // Set nSpaces var according to argument given
                    if (std::next(tokenIt) != line.end()) {
                        ++tokenIt;
                        nSpaces = std::atoi((*tokenIt).c_str());
                    } else {
                        nSpaces = 1;
                    }
                    // Reserve memory space according to nSpaces
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
                // CONST is the only supported directive in BSS section
                if (op == "CONST") {
                    // Check if CONST's required argument was given
                    if (std::next(tokenIt) != line.end()) {
                        ++tokenIt;
                        auto intRegEx = std::regex("(\\+|-)?[0-9]+");
                        auto hexRegEx = std::regex("(0x)[0-9a-fA-F]{1,4}");
                        // Check if given argument is decimal or hexadecimal
                        if(std::regex_match(*tokenIt, intRegEx)) {
                            machineCode.push_back(std::atoi((*tokenIt).c_str()));
                        } else if (std::regex_match(*tokenIt, hexRegEx)) {
                            // TODO: find out how to convert hex to int an then to string
                        } else {
                            errMsg = genErrMsg(lineCount, "invalid immediate " + *tokenIt);
                            error = 1;
                            return error;
                        }

                        // Reserve memory space for constant value
                        ++memCount;

                        // CONST expects only a single value
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
                // TEXT section cannot have SPACE or CONST directives
                if (op == "SPACE" || op == "CONST") {
                    errMsg = genErrMsg(lineCount, op + " directive in TEXT section");
                    error = 1;
                    return error;
                }
                // Check if instruction is defined in instructions map
                if (opcodeMap.count(op) > 0) {
                    auto opcode = opcodeMap.at(op);

                    // Add instruction opcode to code
                    machineCode.push_back(opcode);
                    ++memCount;

                    // Handle arguments according to which instruction was given
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
                        // These instructions take a single defined symbol as argument
                        // Check if argument was given
                        if (std::next(tokenIt) != line.end()) {
                            ++tokenIt;
                            auto operand = *tokenIt;
                            // Check if argument is defined in symbols map
                            if (symbolsMap.count(operand) > 0) {
                                auto memOperand = symbolsMap.at(operand);
                                // TODO: handle LABEL+N case
                                // Include relative symbol address to machine code
                                machineCode.push_back(memOperand);
                                relative.push_back(memCount);
                                ++memCount;
                            } else {
                                errMsg = genErrMsg(lineCount, "unknown symbol " + operand);
                                error = 1;
                                return error;
                            }
                            // Check if more than 1 argument was given
                            if (std::next(tokenIt) != line.end()) {
                                errMsg = genErrMsg(lineCount, "expecting newline, found " + *std::next(tokenIt));
                                error = 1;
                                return error;
                            }
                        } else {
                            errMsg = genErrMsg(lineCount, "expecting 1 operand, found none");
                            error = 1;
                            return error;
                        }
                        break;
                    case COPY:
                        // COPY takes 2 arguments, possibly comma-separated
                        // Check if first argument was given
                        if (std::next(tokenIt) != line.end()) {
                            ++tokenIt;
                            auto operand = *tokenIt;
                            // Remove comma if needed
                            if (isSuffix(operand, ",")) {
                                operand.pop_back();
                            }
                            // Check if symbol is defined in symbols table
                            if (symbolsMap.count(operand) > 0) {
                                auto memOperand = symbolsMap.at(operand);
                                // TODO: handle LABEL+N case
                                // Include relative symbol address to machine code
                                machineCode.push_back(memOperand);
                                relative.push_back(memCount);
                                ++memCount;
                            } else {
                                errMsg = genErrMsg(lineCount, "unknown symbol " + operand);
                                error = 1;
                                return error;
                            }
                            // Check if second argument was given
                            if (std::next(tokenIt) != line.end()) {
                                ++tokenIt;
                                auto operand = *tokenIt;
                                // Check if symbol is defined in symbols table
                                if (symbolsMap.count(operand) > 0) {
                                    auto memOperand = symbolsMap.at(operand);
                                    // TODO: handle LABEL+N case
                                    // Include relative symbol address to machine code
                                    machineCode.push_back(memOperand);
                                    relative.push_back(memCount);
                                    ++memCount;
                                } else {
                                    errMsg = genErrMsg(lineCount, "unknown symbol " + operand);
                                    error = 1;
                                    return error;
                                }
                                // Check if more than 2 arguments were given
                                if (std::next(tokenIt) != line.end()) {
                                    errMsg = genErrMsg(lineCount, "expecting newline, found " + *std::next(tokenIt));
                                    error = 1;
                                    return error;
                                }
                            } else {
                                errMsg = genErrMsg(lineCount, "expecting 2 operands, found 1");
                                error = 1;
                                return error;
                            }
                        } else {
                            errMsg = genErrMsg(lineCount, "expecting 2 operands, found none");
                            error = 1;
                            return error;
                        }
                        break;
                    case STOP:
                        // STOP does not take any arguments
                        // Check if an argument was given
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
