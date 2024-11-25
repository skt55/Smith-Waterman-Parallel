#include <iostream>
#include <chrono>
#include "smith-waterman.h"

void runTests()
{
    std::string seq1 = "AGCT";
    std::string seq2 = "AGCT";
    std::cout << "Test 1: " << smithWaterman(seq1, seq2) << std::endl;

    seq1 = "AGCTG";
    seq2 = "AGCT";
    std::cout << "Test 2: " << smithWaterman(seq1, seq2) << std::endl;
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();

    runTests();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Total time taken: " << duration.count() << " seconds" << std::endl;

    return 0;
}