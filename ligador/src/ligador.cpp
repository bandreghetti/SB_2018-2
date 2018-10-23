#include <iostream>
#include <list>

#include <linker.hpp>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Missing arguments! Expecting at least 1:" << std::endl
        << "Usage: montador <main-file-to-link-without-extension> ...[modules-to-link]" << std::endl;
        return -1;
    }

    std::list<std::string> filesToLink;
    for (int i = 1; i < argc; ++i) {
        filesToLink.push_back(std::string(argv[i]));
    }

    Linker linker(filesToLink);

    int err;
    err = linker.parseTables();
    if (err) {
        std::cout << linker.getErrorMessage();
        return -1;
    }

    err = linker.link();
    if (err) {
        std::cout << linker.getErrorMessage();
        return -1;
    }

    linker.printTables();
    linker.writeOutput();

    return 0;
}
