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
    }

    return ds;
}