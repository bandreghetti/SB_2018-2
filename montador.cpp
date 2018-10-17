#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include "utils.cpp"

using namespace std;

int preProcess(const string fileName);

int main(int argc, char** argv) { 
    if (argc < 2) {
        cout << "Missing arguments! Expecting 1:" << endl
        << "Usage: montador <file-to-assemble-without-extension>" << endl;
        return -1;
    }
  
    string fileName = string(argv[1]);

    preProcess(fileName);    

    string objFile = fileName + ".obj"; // object file output

    return 0;
}

int preProcess(const string fileName) {
    string asmName = fileName + ".asm"; // input file name
    if (!fileExists(asmName)) {
        cout << "File " + asmName + " does not exists" << endl;
        return -1;
    }
    string preName = fileName + ".pre"; // pre-processed file output

    // Read all lines from file
    ifstream srcFile;
    srcFile.open(asmName);
    list<string> srcLines;
    string line;
    for(getline(srcFile, line); !srcFile.eof(); getline(srcFile, line)) {
        srcLines.push_back(line);
    }
    srcFile.close();

    for(auto i = srcLines.begin(); i != srcLines.end(); ++i){
        cout << *i << endl;
    }

}
