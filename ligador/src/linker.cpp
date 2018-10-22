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
        std::vector<std::string> tokens;
        // Handle TABLE USE section
        while (getline(objFile, line)) {
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
        auto lines = std::get<1>(file);
        auto section = NONE;
        for (auto line : lines) {
            // Handle section change
            if (line[0] == "TABLE USE") {
                section = USE;
            } else if (line[0] == "TABLE DEFINITION") {
                section = DEF;
            } else if (line[0] == "RELATIVE") {
                section = REL;
            } else if (line[0] == "CODE") {
                section = CODE;
            }

            switch(section) {
            case USE:

                break;
            case DEF:

                break;
            case REL:

                break;
            case CODE:

                break;
            case NONE:

                break;
            }
        }
    }
}

std::string Linker::getErrorMessage() {
    return errMsg;
}

int Linker::getError() {
    return error;
}
