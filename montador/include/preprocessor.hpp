#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <tuple>

#include <utils.hpp>

class PreProcessor {
   private:
    std::string fileName;
    std::list<std::tuple<int, std::string>> srcLines;
    std::list<std::tuple<int, std::list<std::string>>> outLines;
   public:
    int error = 0;
    PreProcessor(std::string fileName);
    ~PreProcessor();
    void printSource();
    void printOutput();
    void writeOutput();
    void preProcess();
    std::list<std::tuple<int, std::list<std::string>>> getOutput();
};
