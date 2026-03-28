//
// Created by danielramaa on 3/28/26.
//

#ifndef DA2026_PRJ1_T14_G3_TYPES_H
#define DA2026_PRJ1_T14_G3_TYPES_H

#include <string>
#include <vector>

struct Submission {
    int id;
    std::string title;
    std::string authors;
    std::string email;
    int primaryTopic;
    int secondaryTopic; // 0 if not defined
};

struct Reviewer {
    int id;
    std::string name;
    std::string email;
    int primaryTopic;
    int secondaryTopic; // 0 if not defined
};

struct Parameters {
    int minReviewsPerSubmission    = 1;
    int maxReviewsPerReviewer      = 1;
    int primaryReviewerExpertise   = 1;
    int secondaryReviewerExpertise = 0;
    int primarySubmissionDomain    = 1;
    int secondarySubmissionDomain  = 0;
};

struct Control {
    int generateAssignments    = 1;
    int riskAnalysis           = 0;
    std::string outputFileName = "output.csv";
};

struct Dataset {
    std::vector<Submission> submissions;
    std::vector<Reviewer>   reviewers;
    Parameters params;
    Control    control;
    bool valid = true;
    std::string errorMsg;
};

#endif