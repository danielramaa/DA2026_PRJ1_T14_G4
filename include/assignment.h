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

struct Assignment {
    int submissionId;
    int reviewerId;
    int matchedDomain;
};

struct AssignmentResult {
    bool success;
    std::vector<Assignment> assignments;
    std::vector<std::tuple<int,int,int>> missing; // {submissionId, domain, missingCount}
};

AssignmentResult runAssignment(const Dataset& ds, int mode);

void writeOutput(const std::string& filename,
                 const AssignmentResult& result,
                 const std::vector<int>& riskyReviewers,
                 int riskK);

std::vector<int> runRiskAnalysis(const Dataset& ds);
#endif