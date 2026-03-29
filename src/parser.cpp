//
// Created by danielramaa on 3/28/26.
//
#include "parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Remove leading/trailing whitespace and surrounding quotes
static std::string trim(const std::string& s) {
    const size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    const size_t end = s.find_last_not_of(" \t\r\n");
    std::string r = s.substr(start, end - start + 1);
    if (r.size() >= 2 && r.front() == '"' && r.back() == '"')
        r = r.substr(1, r.size() - 2);
    return r;
}

// Split the CSV line into fields, respecting quoted fields
static std::vector<std::string> splitCSV(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;
    for (char c : line) {
        if (c == '"') { inQuotes = !inQuotes; field += c; }
        else if (c == ',' && !inQuotes) { fields.push_back(trim(field)); field.clear(); }
        else field += c;
    }
    fields.push_back(trim(field));
    return fields;
}


Dataset parseInputFile(const std::string& filename) {
    Dataset ds;

    std::ifstream fin(filename);
    if (!fin.is_open()) {
        ds.valid = false;
        ds.errorMsg = "Cannot open file: " + filename;
        return ds;
    }

    // TODO: parsing

    return ds;
}