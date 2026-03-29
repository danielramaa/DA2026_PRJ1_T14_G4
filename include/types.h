//
// Created by danielramaa on 3/28/26.
//

#ifndef DA2026_PRJ1_T14_G3_TYPES_H
#define DA2026_PRJ1_T14_G3_TYPES_H

#include <string>
#include <vector>

/**
 * @brief Represents a paper submission with author metadata and topic domains.
 */
struct Submission {
    int id;
    std::string title;
    std::string authors;
    std::string email;
    int primaryTopic;
    int secondaryTopic; // 0 if not defined
};

/**
 * @brief Represents a reviewer profile and the domains they can review.
 */
struct Reviewer {
    int id;
    std::string name;
    std::string email;
    int primaryTopic;
    int secondaryTopic; // 0 if not defined
};

/**
 * @brief Stores assignment constraints and scoring weights loaded from the dataset.
 */
struct Parameters {
    int minReviewsPerSubmission    = 1;
    int maxReviewsPerReviewer      = 1;
    int primaryReviewerExpertise   = 1;
    int secondaryReviewerExpertise = 0;
    int primarySubmissionDomain    = 1;
    int secondarySubmissionDomain  = 0;
};

/**
 * @brief Stores execution flags and output settings for assignment and risk analysis.
 */
struct Control {
    int generateAssignments    = 1;
    int riskAnalysis           = 0;
    std::string outputFileName = "output.csv";
};

/**
 * @brief Aggregates all parsed dataset sections and parser validity information.
 */
struct Dataset {
    std::vector<Submission> submissions;
    std::vector<Reviewer>   reviewers;
    Parameters params;
    Control    control;
    bool valid = true;
    std::string errorMsg;
};

#endif