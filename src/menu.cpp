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

        if (choice == "0") {
            std::cout << "Exiting.\n";
            break;
        } else if (choice == "1") {
            std::cout << "Enter file path: ";
            std::string path;
            std::getline(std::cin, path);

            Dataset ds = parseInputFile(path);
            if (!ds.valid) {
                std::cerr << "Error: " << ds.errorMsg << "\n";
            } else {
                currentDataset = ds;
                datasetLoaded = true;
                std::cout << "Loaded " << ds.submissions.size() << " submissions and "
                          << ds.reviewers.size() << " reviewers.\n";
            }
        }
        else if (choice == "2") {
            if (!datasetLoaded) { std::cout << "No dataset loaded.\n"; continue; }
            std::cout << "\n--- Submissions ---\n";
            for (auto& s : currentDataset.submissions)
                std::cout << "  " << s.id << " | " << s.title
                          << " | primary: " << s.primaryTopic
                          << " | secondary: " << s.secondaryTopic << "\n";
        } else if (choice == "3") {
            if (!datasetLoaded) { std::cout << "No dataset loaded.\n"; continue; }
            std::cout << "\n--- Reviewers ---\n";
            for (auto& r : currentDataset.reviewers)
                std::cout << "  " << r.id << " | " << r.name
                          << " | primary: " << r.primaryTopic
                          << " | secondary: " << r.secondaryTopic << "\n";
        } else if (choice == "4") {
            if (!datasetLoaded) { std::cout << "No dataset loaded.\n"; continue; }
            std::cout << "\n--- Parameters ---\n";
            std::cout << "MinReviewsPerSubmission:    " << currentDataset.params.minReviewsPerSubmission    << "\n";
            std::cout << "MaxReviewsPerReviewer:      " << currentDataset.params.maxReviewsPerReviewer      << "\n";
            std::cout << "PrimaryReviewerExpertise:   " << currentDataset.params.primaryReviewerExpertise   << "\n";
            std::cout << "SecondaryReviewerExpertise: " << currentDataset.params.secondaryReviewerExpertise << "\n";
            std::cout << "PrimarySubmissionDomain:    " << currentDataset.params.primarySubmissionDomain    << "\n";
            std::cout << "SecondarySubmissionDomain:  " << currentDataset.params.secondarySubmissionDomain  << "\n";
            std::cout << "\n--- Control ---\n";
            std::cout << "GenerateAssignments: " << currentDataset.control.generateAssignments << "\n";
            std::cout << "RiskAnalysis:        " << currentDataset.control.riskAnalysis        << "\n";
            std::cout << "OutputFileName:      " << currentDataset.control.outputFileName      << "\n";
        } else {
            std::cout << "Invalid option.\n";
        }
    }
}

void runBatchMode(const std::string& inputFile, const std::string& outputFile) {
    /* TODO */
}