#include <string>
#include <fstream>
#include <list>
#include <iostream>
#include <tuple>
#include <utils.hpp>

class PreProcessor
{
private:
    int error = 0;
    std::string fileName;
    std::list<std::tuple<int, std::string>> srcLines;
public:
    PreProcessor(std::string fileName);
    ~PreProcessor();
    void printSource();
};
