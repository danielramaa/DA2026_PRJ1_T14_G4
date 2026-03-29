//
// Created by danielramaa on 3/29/26.
//

#include "../include/menu.h"
#include "parser.h"
#include "types.h"
#include <iostream>

static Dataset currentDataset;
static bool datasetLoaded = false;

void runInteractiveMenu() {
    std::string choice;

    while (true) {
        std::cout << "\n=== Conference Assignment Tool ===\n";
        std::cout << "1. Load dataset\n";
        std::cout << "2. List submissions\n";
        std::cout << "3. List reviewers\n";
        std::cout << "4. Show parameters\n";
        std::cout << "0. Exit\n";
        std::cout << "Choice: ";
        std::getline(std::cin, choice);

        if (choice == "0") { std::cout << "Exiting.\n"; break; }
        else if (choice == "1") { /* TODO */ }
        else if (choice == "2") { /* TODO */ }
        else if (choice == "3") { /* TODO */ }
        else if (choice == "4") { /* TODO */ }
        else std::cout << "Invalid option.\n";
    }
}

void runBatchMode(const std::string& inputFile, const std::string& outputFile) {
    /* TODO */
}