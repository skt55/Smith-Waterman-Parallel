#include <iostream>
#include <chrono>
#include <fstream>
#include "smith-waterman.hpp"

void runTests()
{
    std::ifstream inputFile("input.txt");
    std::ofstream outputFile("output.txt");

    std::string seq1, seq2;
    if (inputFile >> seq1 >> seq2)
    {
        std::pair<std::string, std::string> out = smithWaterman(seq1, seq2);
        outputFile << out.first << " " << out.second << std::endl;
    }

    inputFile.close();
    outputFile.close();
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