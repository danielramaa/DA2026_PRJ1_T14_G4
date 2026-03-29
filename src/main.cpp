#include "types.h"
#include "parser.h"
#include "menu.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc >= 3 && std::string(argv[1]) == "-b") {
        std::string inputFile  = argv[2];
        std::string outputFile = (argc >= 4) ? argv[3] : "";
        runBatchMode(inputFile, outputFile);
    } else {
        runInteractiveMenu();
    }
    return 0;
}