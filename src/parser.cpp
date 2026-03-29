/**
 * @file parser.cpp
 * @brief Dataset parsing utilities and input-file parsing implementation.
 */
#include "parser.h"
#include <fstream>
#include <sstream>
#include <iostream>

/**
 * @brief Removes leading/trailing whitespace and surrounding double quotes from a field.
 * @param s Raw input string.
 * @return Normalized string without outer whitespace and optional enclosing quotes.
 * @complexity O(n), where n is the length of the input string.
 */
static std::string trim(const std::string& s) {
    const size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    const size_t end = s.find_last_not_of(" \t\r\n");
    std::string r = s.substr(start, end - start + 1);
    if (r.size() >= 2 && r.front() == '"' && r.back() == '"')
        r = r.substr(1, r.size() - 2);
    return r;
}

/**
 * @brief Splits a CSV line into trimmed fields while preserving commas inside quoted text.
 * @param line Raw CSV line.
 * @return Vector with parsed and trimmed field strings.
 * @complexity O(n), where n is the length of the line.
 */
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


/**
 * @brief Parses a dataset file with submissions, reviewers, parameters, and control sections.
 * @param filename Path to the dataset CSV-like input file.
 * @return Dataset object containing parsed records and validity/error metadata.
 * @complexity O(N), where N is the total number of characters read from the file.
 */
Dataset parseInputFile(const std::string& filename) {
    Dataset ds;

    std::ifstream fin(filename);
    if (!fin.is_open()) {
        ds.valid = false;
        ds.errorMsg = "Cannot open file: " + filename;
        return ds;
    }

    enum Section { NONE, SUBMISSIONS, REVIEWERS, PARAMETERS, CONTROL };
    Section section = NONE;
    std::string line;

    while (std::getline(fin, line)) {
        // Remove spaces in the end
        while (!line.empty() && (line.back() == '\r' || line.back() == ' '))
            line.pop_back();

        if (line.empty()) continue;

        if (line == "#Submissions") { section = SUBMISSIONS; continue; }
        if (line == "#Reviewers")   { section = REVIEWERS;   continue; }
        if (line == "#Parameters")  { section = PARAMETERS;  continue; }
        if (line == "#Control")     { section = CONTROL;     continue; }

        if (line[0] == '#') continue;

        if (section == SUBMISSIONS) {
            auto f = splitCSV(line);
            if (f.size() < 5) continue;

            Submission s;
            s.id          = std::stoi(f[0]);
            s.title       = f[1];
            s.authors     = f[2];
            s.email       = f[3];
            s.primaryTopic   = std::stoi(f[4]);
            s.secondaryTopic = (f.size() > 5 && !f[5].empty()) ? std::stoi(f[5]) : 0;

            ds.submissions.push_back(s);
        }

        else if (section == REVIEWERS) {
            auto f = splitCSV(line);
            if (f.size() < 4) continue;

            Reviewer r;
            r.id           = std::stoi(f[0]);
            r.name         = f[1];
            r.email        = f[2];
            r.primaryTopic   = std::stoi(f[3]);
            r.secondaryTopic = (f.size() > 4 && !f[4].empty()) ? std::stoi(f[4]) : 0;

            ds.reviewers.push_back(r);
        }

        else if (section == PARAMETERS) {
            auto f = splitCSV(line);
            if (f.size() < 2) continue;

            const std::string& key = f[0];
            int val = std::stoi(f[1]);

            if      (key == "MinReviewsPerSubmission")    ds.params.minReviewsPerSubmission    = val;
            else if (key == "MaxReviewsPerReviewer")      ds.params.maxReviewsPerReviewer      = val;
            else if (key == "PrimaryReviewerExpertise")   ds.params.primaryReviewerExpertise   = val;
            else if (key == "SecondaryReviewerExpertise") ds.params.secondaryReviewerExpertise = val;
            else if (key == "PrimarySubmissionDomain")    ds.params.primarySubmissionDomain    = val;
            else if (key == "SecondarySubmissionDomain")  ds.params.secondarySubmissionDomain  = val;
        }

        else if (section == CONTROL) {
            auto f = splitCSV(line);
            if (f.size() < 2) continue;

            const std::string& key = f[0];
            const std::string& val = f[1];

            if      (key == "GenerateAssignments") ds.control.generateAssignments = std::stoi(val);
            else if (key == "RiskAnalysis")        ds.control.riskAnalysis        = std::stoi(val);
            else if (key == "OutputFileName")      ds.control.outputFileName      = val;
        }
    }

    return ds;
}