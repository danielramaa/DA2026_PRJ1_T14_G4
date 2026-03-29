#include "assignment.h"
#include "parser.h"
#include <iostream>

int main() {
    Dataset ds = parseInputFile("dataset3.csv");
    AssignmentResult result = runAssignment(ds, 1);

    std::vector<int> risky = runRiskAnalysis(ds);

    std::cout << "Risky reviewers: ";
    if (risky.empty())
        std::cout << "none\n";
    else
        for (int id : risky) std::cout << id << " ";
    std::cout << "\n";

    writeOutput(ds.control.outputFileName, result, risky, ds.control.riskAnalysis);

    return 0;
}