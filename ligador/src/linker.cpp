#include <linker.hpp>

Linker::Linker(std::list<std::string> filesToLink) {
    for (auto fileName : filesToLink) {
        std::string objName = fileName + ".obj";  // object file name
        if (!fileExists(objName)) {
            errMsg = "File " + objName + " does not exist\n";
            error = 1;
            return;
        }
        // Open file
        std::ifstream objFile;
        objFile.open(objName);
        std::string line;
        std::list<std::vector<std::string>> fileLines;
        // Handle TABLE USE section
        while (getline(objFile, line)) {
            std::vector<std::string> tokens;
            if (line == "TABLE USE" ||
                line == "TABLE DEFINITION" ||
                line == "RELATIVE" ||
                line == "CODE")
            {
                tokens.push_back(line);
            } else {
                tokens = split(line, ' ');
            }

            fileLines.push_back(tokens);
        }
        srcFiles.push_back(std::make_tuple(objName, fileLines));
        objFile.close();
    }
}

enum {
    NONE = 0,
    USE,
    DEF,
    REL,
    CODE
};

int Linker::parseTables() {
    if (error) {
        return error;
    }

    for (auto file : srcFiles) {
        auto fileName = std::get<0>(file);

        std::map<std::string, std::list<int>> useTable;
        std::map<std::string, int> defTable;

        auto lines = std::get<1>(file);
        auto section = NONE;
        for (auto line : lines) {
            // Handle section change
            if (line[0] == "TABLE USE") {
                section = USE;
                continue;
            } else if (line[0] == "TABLE DEFINITION") {
                section = DEF;
                continue;
            } else if (line[0] == "RELATIVE") {
                section = REL;
                continue;
            } else if (line[0] == "CODE") {
                section = CODE;
                continue;
            }

            // Check if in a section
            if (section == NONE) {
                errMsg = genErrMsg(fileName, "wrong format. every entry must be under a marker");
                return error;
            }

            if (section == USE || section == DEF) {
                // Check if there are 2 tokens in line
                if (line.size() != 2) {
                    errMsg = genErrMsg(fileName, "TABLE USE section lines must be of the form: LABEL ADDR");
                    return error;
                }

                // Check if second label is a natural number
                if (!std::regex_match(line[1], natRegEx)) {
                    errMsg = genErrMsg(fileName, "TABLE USE addresses must be natural numbers");
                    return error;
                }

                auto label = line[0];
                auto addr = std::stoi(line[1]);

                if (section == USE) {
                    useTable[label].push_back(addr);
                    // TODO: check for repeating address
                } else if (section == DEF) {
                    // Check for local redefition
                    if (defTable.count(label) > 0) {
                        errMsg = genErrMsg(fileName, "TABLE DEFINITION symbol " + label + "local redefinition");
                        return error;
                    }

                    // Check for global redefinition
                    if (definedSymbols.count(label) > 0) {
                        errMsg = genErrMsg(fileName, "TABLE DEFINITION symbol " + label + "global redefinition");
                        return error;
                    }

                    defTable[label] = addr;
                    definedSymbols.insert(label);
                }
            } else if (section == REL) {

            } else if (section == CODE) {

            }
        }
        useTables[fileName] = useTable;
        defTables[fileName] = defTable;
    }

    return 0;
}

int Linker::printTables() {
    std::cout << "USE TABLES\n";
    for (auto useTable : useTables) {
        auto fileName = std::get<0>(useTable);
        std::cout << fileName + '\n';
        auto useMap = std::get<1>(useTable);
        for (auto kvPair : useMap) {
            auto label = kvPair.first;
            std::cout << label + ':';
            auto useList = kvPair.second;
            for (auto useAddr : useList) {
                std::cout << ' ' + std::to_string(useAddr);
            }
            std::cout << std::endl;
        }
    }
}

std::string Linker::getErrorMessage() {
    return errMsg;
}

int Linker::getError() {
    return error;
}

std::string Linker::genErrMsg(std::string fileName, std::string message) {
    error = 1;
    return "error in file " + fileName + ": " + message;
}
