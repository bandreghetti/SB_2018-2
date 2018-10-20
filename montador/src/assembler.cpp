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

    // Write TABLE USE section to object file
    outFile << "TABLE USE\n";
    for (auto use : useTable) {
        auto rot = std::get<0>(use);
        auto addr = std::get<1>(use);
        outFile << rot + ' ' + std::to_string(addr) + '\n';
    }

    // Write TABLE DEFINITION section to object file
    outFile << "TABLE DEFINITION\n";
    for (auto def : definitionTable) {
        auto rot = std::get<0>(def);
        auto addr = std::get<1>(def);
        outFile << rot + ' ' + std::to_string(addr) + '\n';
    }

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
    bool inModule = false;
    bool hadText = false;

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
                if (hadText) {
                    errMsg = genErrMsg(lineCount, "SECTION TEXT must not be split inside a module");
                    error = 1;
                    return error;
                }
                section = TEXT;
                hadText = true;
            } else if (section == NONE) {
                errMsg = genErrMsg(lineCount, "SECTION TEXT must be the first first inside a module");
                error = 1;
                return error;
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
        }

        // Get iterator to the first token in line
        auto tokenIt = line.begin();

        // If line begins with label
        auto label = line.front();
        if (isSuffix(label, ":")) {
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
                    if (std::regex_match(*nextTokenIt, natRegEx)) {
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
            // If token is EXTERN, add label to externSymbols set
            } else if (token == "EXTERN") {
                // Check if label was defined
                if (label == "") {
                    errMsg = genErrMsg(lineCount, "EXTERN directive requires label");
                    error = 1;
                    return error;
                }
                // Add label to externSymbols set
                externSymbols.insert(label);
                // Set label address to 0 (its true address will be set by linker)
                symbolsMap[label] = 0;
            // If token is BEGIN, handle errors and control flags as needed
            } else if (token == "BEGIN") {
                // Check if already in a module
                if (inModule) {
                    errMsg = genErrMsg(lineCount, "cannot BEGIN module inside another module");
                    error = 1;
                    return error;
                }
                inModule = true;
                hadText = false;
            } else if (token == "END") {
                // Check if in a module
                if (!inModule) {
                    errMsg = genErrMsg(lineCount, "cannot END module outside a module");
                    error = 1;
                    return error;
                }
                // Check if TEXT section was declared
                if (!hadText) {
                    errMsg = genErrMsg(lineCount, "module must have a TEXT section");
                    error = 1;
                    return error;
                }
                inModule = false;
                section = NONE;
            } else {
                // Since instruction/directive was not handled above, check if it is defined
                if(memSpaceMap.count(token) == 0) {
                    errMsg = genErrMsg(lineCount, "instruction/directive " + token + " not defined");
                    error = 1;
                    return error;
                }
                // If defined, reserve space for the instruction/directive accordingly
                memCount += memSpaceMap.at(token);
            }
        }
    }

    // Check if TEXT section was declared
    if (!hadText) {
        errMsg = "TEXT section not found";
        error = 1;
        return error;
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
        }

        // Get iterator to the first token in line
        auto tokenIt = line.begin();

        // Check if line begins with label
        if (isSuffix(line.front(), ":")) {
            auto label = line.front();
            label.pop_back();

            ++tokenIt;
        }

        // If there is an instruction/directive in this line
        if (tokenIt != line.end()) {
            auto op = *tokenIt;
            // Handle EXTERN keyword no matter where it is in the code
            if (op == "EXTERN") {
                // EXTERN does no require arguments
                if (std::next(tokenIt) != line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting newline, found " + *std::next(tokenIt));
                    error = 1;
                    return error;
                }
                // Continue on to next line
                continue;
            }
            // Handle PUBLIC keyword no matter where it is in the code
            if (op == "PUBLIC") {
                // PUBLIC requires a single symbol as argument
                if (std::next(tokenIt) == line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting symbol, found newline");
                    error = 1;
                    return error;
                }

                // Advance tokenIt to symbol
                ++tokenIt;
                auto symbol = *tokenIt;

                // Check if symbol is extern
                if (externSymbols.count(symbol) > 0) {
                    errMsg = genErrMsg(lineCount, "cannot make extern symbol public");
                    error = 1;
                    return error;
                }

                // Check if symbol was defined
                if (symbolsMap.count(symbol) == 0) {
                    errMsg = genErrMsg(lineCount, "unknown symbol " + symbol);
                    error = 1;
                    return error;
                }

                // Add public symbol to definitions table
                auto definitionTuple = std::make_tuple(symbol, symbolsMap.at(symbol));
                definitionTable.push_back(definitionTuple);

                // PUBLIC expect exactly 1 argument
                if (std::next(tokenIt) != line.end()) {
                    errMsg = genErrMsg(lineCount, "expecting newline, found " + *std::next(tokenIt));
                    error = 1;
                    return error;
                }

                // Continue on to next line
                continue;
            }

            // Handle BEGIN and END keywords no matter where they are in the code
            if (op == "BEGIN" || op == "END") {
                continue;
            }

            // Handle operators according to which section we are in
            switch (section) {
            case BSS:
                // SPACE is the only supported directive in BSS section
                if (op != "SPACE") {
                    errMsg = genErrMsg(lineCount, "non-SPACE operator/directive in BSS (uninitialized data) section");
                    error = 1;
                    return error;
                }

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

                break;
            case DATA:
                // CONST is the only supported directive in BSS section
                if (op != "CONST") {
                    errMsg = genErrMsg(lineCount, "non-CONST operator/directive in DATA section");
                    error = 1;
                    return error;
                }

                // Check if CONST's required argument was given
                if (std::next(tokenIt) == line.end()) {
                    errMsg = genErrMsg(lineCount, "newline found, expected decimal or hexadecimal (with 0x prefix) immediate");
                    error = 1;
                    return error;
                }

                // Advance tokenIt to argument
                ++tokenIt;

                // Check if given argument is decimal or hexadecimal
                if(std::regex_match(*tokenIt, intRegEx)) {
                    machineCode.push_back(std::atoi((*tokenIt).c_str()));
                } else if (std::regex_match(*tokenIt, hexRegEx)) {
                    // Convert to lower case if hexadecimal
                    for(auto &c : *tokenIt) {
                        c = std::tolower(c);
                    }
                    machineCode.push_back(std::stoi(*tokenIt, 0, 16));
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

                break;
            case TEXT:
                // TEXT section cannot have SPACE or CONST directives
                if (op == "SPACE" || op == "CONST") {
                    errMsg = genErrMsg(lineCount, op + " directive in TEXT section");
                    error = 1;
                    return error;
                }

                // Check if instruction is defined in instructions map
                if (opcodeMap.count(op) == 0) {
                    errMsg = genErrMsg(lineCount, "unknown " + op + " operator");
                    error = 1;
                    return error;
                }
                short opcode = opcodeMap.at(op);

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
                    if (std::next(tokenIt) == line.end()) {
                        errMsg = genErrMsg(lineCount, "expecting 1 operand, found none");
                        error = 1;
                        return error;
                    }

                    // Advance tokenIt to argument
                    ++tokenIt;

                    // Check if argument is defined in symbols map
                    handleArgument(lineCount, &tokenIt, line.end(), &memCount);
                    if (error) return error;

                    // Check if more than 1 argument was given
                    if (std::next(tokenIt) != line.end()) {
                        errMsg = genErrMsg(lineCount, "expecting newline, found " + *std::next(tokenIt));
                        error = 1;
                        return error;
                    }

                    break;
                case COPY:
                    // COPY takes 2 arguments, possibly comma-separated
                    // Check if first argument was given
                    if (std::next(tokenIt) == line.end()) {
                        errMsg = genErrMsg(lineCount, "expecting 2 operands, found none");
                        error = 1;
                        return error;
                    }

                    // Advance tokenIt to first argument
                    ++tokenIt;

                    // Call handleArgument to update machineCode, relative and useTable
                    handleArgument(lineCount, &tokenIt, line.end(), &memCount);
                    if (error) return error;

                    // Check if second argument was given
                    if (std::next(tokenIt) == line.end()) {
                        errMsg = genErrMsg(lineCount, "expecting 2 operands, found 1");
                        error = 1;
                        return error;
                    }
                    // Advance tokenIt to second argument
                    ++tokenIt;

                    // Call handleArgument to update machineCode, relative and useTable
                    handleArgument(lineCount, &tokenIt, line.end(), &memCount);
                    if (error) return error;

                    // Check if more than 2 arguments were given
                    if (std::next(tokenIt) != line.end()) {
                        errMsg = genErrMsg(lineCount, "expecting newline, found " + *std::next(tokenIt));
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

void Assembler::handleArgument(int lineCount, std::list<std::string>::iterator* tokenItPtr, std::list<std::string>::iterator lineEnd, int* memCountPtr) {
    auto operand = **tokenItPtr;

    // Check if operator is COPY (takes two arguments, need to handle comma)
    bool isCopy = false;
    if (*std::prev(*tokenItPtr) == "COPY") {
        isCopy = true;
        // Remove comma if needed
        if (isSuffix(operand, ",")) {
            operand.pop_back();
        }
    }

    // Check if argument is defined in symbols map
    if (symbolsMap.count(operand) == 0) {
        errMsg = genErrMsg(lineCount, "unknown symbol " + operand);
        error = 1;
        return;
    }
    auto memOperand = symbolsMap.at(operand);

    // Handle LABEL + N case
    if (std::next(*tokenItPtr) != lineEnd && symbolsMap.count(*std::next(*tokenItPtr)) == 0) {
        ++*tokenItPtr;
        // Check if next token is a +
        auto plusSign = **tokenItPtr;
        if (plusSign != "+") {
            if (isCopy) {
                errMsg = genErrMsg(lineCount, "expecting + or known symbol, found " + plusSign);
            } else {
                errMsg = genErrMsg(lineCount, "expecting + or newline, found " + plusSign);
            }
            error = 1;
            return;
        }
        // Check if there is a token after +
        if (std::next(*tokenItPtr) == lineEnd) {
            errMsg = genErrMsg(lineCount, "expecting decimal number, found newline");
            error = 1;
            return;
        }
        ++*tokenItPtr;
        auto N = **tokenItPtr;
        // Handle comma if necessary
        if (isCopy && isSuffix(N, ",")) {
            N.pop_back();
        }
        // Check if token after + is a valid number
        if (!std::regex_match(N, natRegEx)) {
            errMsg = genErrMsg(lineCount, "expecting decimal number, found " + N);
            error = 1;
            return;
        }
        // Since everything went ok, increment memOperand
        memOperand += std::atoi(N.c_str());
    }
    // Include relative symbol address to machine code
    machineCode.push_back(memOperand);
    // If operand is an extern symbol, add its address to use table
    if (externSymbols.count(operand) > 0) {
        auto useTuple = std::make_tuple(operand, *memCountPtr);
        useTable.push_back(useTuple);
    // Else, add its address to relative list
    } else {
        relative.push_back(*memCountPtr);
    }
    ++*memCountPtr;
}
