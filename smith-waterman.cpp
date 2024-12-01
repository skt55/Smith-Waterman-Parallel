#include <algorithm>
#include <iostream>
#include <bits/stdc++.h>
#include "smith-waterman.hpp"
#include <chrono>

using namespace std;

std::pair<std::string, std::string> smithWaterman(const char *seq1, size_t size1, const char *seq2, size_t size2)
{
    int match = 2;     // Score for a match
    int mismatch = -1; // Score for a mismatch
    int gap = -1;      // Score for a gap

    //MATRIX ALLOCATION + TIMING HARNESS
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::vector<int>> score(size1 + 1, std::vector<int>(size2 + 1, 0));
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Matrix Allocation Time: " << duration.count() << " seconds" << std::endl;

    int maxScore = 0;
    int maxI = 0, maxJ = 0;

    start = std::chrono::high_resolution_clock::now();
    // Fill the scoring matrix
    for (size_t i = 1; i <= size1; ++i)
    {
        for (size_t j = 1; j <= size2; ++j)
        {
            int matchScore = (seq1[i - 1] == seq2[j - 1]) ? match : mismatch;
            score[i][j] = std::max({0,
                                    score[i - 1][j - 1] + matchScore,
                                    score[i - 1][j] + gap,
                                    score[i][j - 1] + gap});
            if (score[i][j] > maxScore)
            {
                maxScore = score[i][j];
                maxI = i;
                maxJ = j;
            }
        }
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Matrix Processing Time: " << duration.count() << " seconds" << std::endl;


    start = std::chrono::high_resolution_clock::now();
    // Backtrack to find the aligned sequences
    std::string alignedSeq1, alignedSeq2;
    size_t i = maxI, j = maxJ;
    //BACKTRACKING
    while (i > 0 && j > 0 && score[i][j] > 0)
    {
        if (seq1[i - 1] == seq2[j - 1])
        {
            alignedSeq1 += seq1[i - 1];
            alignedSeq2 += seq2[j - 1];
            i--;
            j--;
        }
        else if (score[i][j] == score[i - 1][j] + gap)
        {
            alignedSeq1 += seq1[i - 1];
            alignedSeq2 += '-';
            i--;
        }
        else
        {
            alignedSeq1 += '-';
            alignedSeq2 += seq2[j - 1];
            j--;
        }
    }
    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Backtrack Matrix Time:  " << duration.count() << " seconds" << std::endl;


    // Reverse the aligned sequences
    std::reverse(alignedSeq1.begin(), alignedSeq1.end());
    std::reverse(alignedSeq2.begin(), alignedSeq2.end());

    return {alignedSeq1, alignedSeq2}; // Return the aligned sequences
}