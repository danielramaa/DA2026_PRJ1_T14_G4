//
// Created by danielramaa on 3/29/26.
//

#ifndef DA2026_PRJ1_T14_G3_MENU_H
#define DA2026_PRJ1_T14_G3_MENU_H

#include <string>

/**
 * @brief Runs the interactive terminal menu workflow for loading data and executing assignment operations.
 * @complexity Per interaction O(1) for navigation, up to O(T + R*T) when running assignment and optional risk analysis, where T is runAssignment complexity and R is reviewer count.
 */
void runInteractiveMenu();

/**
 * @brief Runs the tool in batch mode from command-line arguments.
 * @param inputFile Path to the dataset input file.
 * @param outputFile Optional output file path override; if empty, dataset control value is used.
 * @complexity O(T + R*T), where T is runAssignment complexity and R is reviewer count when risk analysis is enabled.
 */
void runBatchMode(const std::string& inputFile, const std::string& outputFile);

#endif