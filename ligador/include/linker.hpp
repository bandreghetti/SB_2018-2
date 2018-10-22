#include <iostream>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <set>

#include <utils.hpp>

class Linker {
    private:
        std::regex intRegEx = std::regex("(\\+|-)?[0-9]+");
        std::regex hexRegEx = std::regex("0(x|X)[0-9a-f]{1,4}");
        std::regex natRegEx = std::regex("[0-9]+");

        int error = 0;
        std::string errMsg;
        std::string genErrMsg(std::string, std::string);

        std::map<std::string, std::list<std::vector<std::string>>> srcFiles;

        std::map<std::string, std::map<std::string, std::list<int>>> useTables;
        std::set<std::string> definedSymbols;
        std::map<std::string, std::map<std::string, int>> defTables;
        std::map<std::string, std::list<int>> relativeList;
        std::map<std::string, std::list<int>> machineCode;
        std::map<std::string, unsigned int> sizeMap;
    public:
        Linker(std::list<std::string>);
        int printOutput();
        int writeOutput();
        int getError();
        std::string getErrorMessage();

        int parseTables();
        int printTables();
};
