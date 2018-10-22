#include <iostream>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <tuple>

#include <utils.hpp>

class Linker {
    private:
        int error = 0;
        std::string errMsg;
        std::string genErrMsg(int, std::string);

        std::list<std::tuple<std::string, std::list<std::vector<std::string>>>> srcFiles;

        std::list<std::map<std::string, std::list<int>>> useTables;
    public:
        Linker(std::list<std::string>);
        int printOutput();
        int writeOutput();
        int getError();
        std::string getErrorMessage();

        int parseTables();
        int printTables();
};
