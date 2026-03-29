//
// Created by Daniel on 29/03/2026.
//

#ifndef DA2026_PRJ1_T14_G3_ASSIGNMENT_H
#define DA2026_PRJ1_T14_G3_ASSIGNMENT_H

#include "types.h"
#include "graph.h"
#include <vector>
#include <string>
#include <tuple>

/**
 * @brief Represents one assignment between a submission and a reviewer with the matched domain.
 */
struct Assignment {
    int submissionId;
    int reviewerId;
    int matchedDomain;
};

/**
 * @brief Stores assignment execution status, produced assignments, and unmet review requirements.
 */
struct AssignmentResult {
    bool success;
    std::vector<Assignment> assignments;
    std::vector<std::tuple<int,int,int>> missing; // {submissionId, domain, missingCount}
};

/**
 * @brief Computes reviewer assignments for all submissions according to the selected matching mode.
 * @param ds Parsed dataset containing submissions, reviewers, parameters, and control flags.
 * @param mode Assignment mode (1 for primary-topic matching, 2 for primary/secondary matching).
 * @return Assignment result with success flag, assignment list, and missing-review entries.
 * @complexity O(VE^2 + S*R), dominated by Edmonds-Karp max-flow, where V is graph vertices and E is edges.
 */
AssignmentResult runAssignment(const Dataset& ds, int mode);

/**
 * @brief Writes assignments, missing requirements, and optional risk analysis results to an output file.
 * @param filename Path of the output file to write.
 * @param result Assignment result to serialize.
 * @param riskyReviewers Reviewer IDs considered critical by risk analysis.
 * @param riskK Risk-analysis control value used to decide whether to output risk data.
 * @complexity O(A log R + M + K), where A is assignments, R is distinct reviewers in assignments, M is missing entries, and K is risky reviewers.
 */
void writeOutput(const std::string& filename,
                 const AssignmentResult& result,
                 const std::vector<int>& riskyReviewers,
                 int riskK);

/**
 * @brief Identifies critical reviewers whose removal causes assignment infeasibility.
 * @param ds Parsed dataset used as baseline for leave-one-reviewer-out analysis.
 * @return Sorted list of reviewer IDs classified as risky.
 * @complexity O(R * T), where R is number of reviewers and T is the complexity of one runAssignment call.
 */
std::vector<int> runRiskAnalysis(const Dataset& ds);
#endif