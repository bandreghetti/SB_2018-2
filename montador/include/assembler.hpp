#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <tuple>
#include <regex>

#include <utils.hpp>

class Assembler {
    private:
        std::string fileName;
        std::list<std::tuple<int, std::list<std::string>>> srcLines;
        std::map<std::string, int> symbolsMap;
        std::list<unsigned int> relative;
        std::list<short> machineCode;
        enum {
            ADD = 1,
            SUB, 
            MULT,
            DIV, 
            JMP, 
            JMPN,
            JMPP, 
            JMPZ, 
            COPY, 
            LOAD,
            STORE,
            INPUT,
            OUTPUT,
            STOP,
        };
        const std::map<std::string, short> opcodeMap = {
            {"ADD", 1},
            {"SUB", 2},
            {"MULT", 3},
            {"DIV", 4},
            {"JMP", 5},
            {"JMPN", 6},
            {"JMPP", 7},
            {"JMPZ", 8},
            {"COPY", 9},
            {"LOAD", 10},
            {"STORE", 11},
            {"INPUT", 12},
            {"OUTPUT", 13},
            {"STOP", 14}
        };
        const std::map<std::string, int> memSpaceMap = {
            {"CONST", 1},
            {"ADD", 2},
            {"SUB", 2},
            {"MULT", 2},
            {"DIV", 2},
            {"JMP", 2},
            {"JMPN", 2},
            {"JMPP", 2},
            {"JMPZ", 2},
            {"COPY", 3},
            {"LOAD", 2},
            {"STORE", 2},
            {"INPUT", 2},
            {"OUTPUT", 2},
            {"STOP", 1}
        };
        int error = 0;
        std::string errMsg;
        std::string genErrMsg(int, std::string);
    public:
        Assembler(std::string, std::list<std::tuple<int, std::list<std::string>>>);
        int printSource();
        int printOutput();
        int writeOutput();
        int firstPass();
        int secondPass();
        int getError();
        std::string getErrorMessage();
};
