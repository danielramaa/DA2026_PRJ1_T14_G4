//
// Created by danielramaa on 3/28/26.
//

#ifndef DA2026_PRJ1_T14_G3_PARSER_H
#define DA2026_PRJ1_T14_G3_PARSER_H

#include "types.h"
#include <string>

/**
 * @brief Parses a conference dataset file into structured in-memory data.
 * @param filename Path to the input dataset file.
 * @return Parsed dataset with validity status and error message when parsing fails.
 * @complexity O(N), where N is the total number of characters read from the file.
 */
Dataset parseInputFile(const std::string& filename);

#endif