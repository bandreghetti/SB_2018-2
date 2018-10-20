#include <iostream>
#include <list>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Missing arguments! Expecting at least 1:" << std::endl
        << "Usage: montador <file-to-assemble-without-extension>" << std::endl;
        return -1;
    }

    auto mainName = std::string(argv[1]);
    std::list<std::string> filesToLink;
    for (int i = 2; i < argc; ++i) {
        filesToLink.push_back(std::string(argv[i]));
    }

}
