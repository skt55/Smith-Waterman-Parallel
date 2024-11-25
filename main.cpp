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
        // Convert strings to char arrays
        const char *seq1Ptr = seq1.c_str();
        const char *seq2Ptr = seq2.c_str();
        size_t size1 = seq1.length();
        size_t size2 = seq2.length();

        // Call smithWaterman with char pointers and their sizes
        std::pair<std::string, std::string> out = smithWaterman(seq1Ptr, size1, seq2Ptr, size2);
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