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
    int error = 0;
    std::string fileName;
    std::list<std::tuple<int, std::string>> srcLines;
    std::list<std::tuple<int, std::list<std::string>>> outLines;

   public:
    PreProcessor(std::string fileName);
    ~PreProcessor();
    void printSource();
    void printOutput();
    void preProcess();
};
