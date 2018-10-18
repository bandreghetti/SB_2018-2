#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <utils.hpp>
#include <preprocessor.hpp>

using namespace std;

int main(int argc, char** argv) { 
    if (argc < 2) {
        cout << "Missing arguments! Expecting 1:" << endl
        << "Usage: montador <file-to-assemble-without-extension>" << endl;
        return -1;
    }
  
    string fileName = string(argv[1]); 

    PreProcessor pp(fileName);
    if(pp.error) {
        return -1;
    }
    // pp.printSource();
    pp.preProcess();
    pp.printOutput();
    pp.writeOutput();

    string objFile = fileName + ".obj"; // object file output

    return 0;
}
