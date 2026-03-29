#include "types.h"
#include "parser.h"
#include <iostream>

int main() {
    Dataset ds = parseInputFile("dataset1.csv");

    if (!ds.valid) {
        std::cerr << "Erro: " << ds.errorMsg << std::endl;
        return 1;
    }

    std::cout << "Submissions: " << ds.submissions.size() << std::endl;
    std::cout << "Reviewers: " << ds.reviewers.size() << std::endl;

    return 0;
}