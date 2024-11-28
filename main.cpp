#include <iostream>
#include <chrono>
#include <fstream>
#include "smith-waterman.hpp"
#include <sstream>

void runTests(int lineNumber)
{
    std::ifstream inputFile("input.txt");
    std::ofstream outputFile("output.txt");

    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file!" << std::endl;
        return;
    }

    std::string seq1, seq2;
    std::string line;
    int currentLine = 1;

    // Read the file line by line
    while (std::getline(inputFile, line)) {
        if (currentLine == lineNumber) {
            std::istringstream lineStream(line);
            if (lineStream >> seq1 >> seq2) {
                // Convert strings to char arrays
                const char *seq1Ptr = seq1.c_str();
                const char *seq2Ptr = seq2.c_str();
                size_t size1 = seq1.length();
                size_t size2 = seq2.length();
                std::cout << "Finished String Loading" << std::endl;
                // Call smithWaterman with char pointers and their sizes

                auto start = std::chrono::high_resolution_clock::now();
                std::pair<std::string, std::string> out = smithWaterman(seq1Ptr, size1, seq2Ptr, size2);
                auto end = std::chrono::high_resolution_clock::now();
                
                std::chrono::duration<double> duration = end - start;
                std::cout << "Total time taken: " << duration.count() << " seconds" << std::endl;
                outputFile << out.first << " " << out.second << std::endl;
            } else {
                std::cerr << "Error: Line " << lineNumber << " is not properly formatted!" << std::endl;
            }
            break;
        }
        currentLine++;
    }

    if (currentLine < lineNumber) {
        std::cerr << "Error: The input file has fewer lines than the specified line number!" << std::endl;
    }

    inputFile.close();
    outputFile.close();
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <lineNumber>" << std::endl;
        return 1;
    }

    int lineNumber;
    try {
        lineNumber = std::stoi(argv[1]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Invalid line number!" << std::endl;
        return 1;
    }

    //auto start = std::chrono::high_resolution_clock::now();

    runTests(lineNumber);

    //auto end = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> duration = end - start;
    //std::cout << "Total time taken: " << duration.count() << " seconds" << std::endl;

    return 0;
}