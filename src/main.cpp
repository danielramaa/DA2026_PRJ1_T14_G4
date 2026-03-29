#include "assignment.h"
#include "parser.h"
#include <iostream>

int main() {
    Dataset ds = parseInputFile("dataset1.csv");
    AssignmentResult result = runAssignment(ds, 1);

    std::cout << "Success: " << result.success << "\n";
    std::cout << "Assignments: " << result.assignments.size() << "\n";
    std::cout << "Missing: " << result.missing.size() << "\n";

    writeOutput(ds.control.outputFileName, result, {}, 0);
    std::cout << "Output written to: " << ds.control.outputFileName << "\n";

    return 0;
}