#include <algorithm>
#include <iostream>
#include <bits/stdc++.h>
#include "smith-waterman.hpp"
#include <omp.h>
#include <chrono>

using namespace std;

const int _BLOCK_SIZE_ = 64;
// Define gap penalties and match/mismatch scores
#define MATCH_SCORE 2
#define MISMATCH_SCORE -1
#define GAP_PENALTY -1

//start and end are inclusive
std::tuple<int, int, int> process_block(int start_i, int end_i, int start_j, int end_j, 
                   std::vector<std::vector<int>>& matrix, const std::string& seq1, const std::string& seq2) {

    int match = 2;     // Score for a match
    int mismatch = -1; // Score for a mismatch
    int gap = -1;      // Score for a gap

    int maxScore = 0;
    int maxI = 0;
    int maxJ = 0;

    for (size_t i = start_i; i <= end_i; ++i)
    {
        for (size_t j = start_j; j <= end_j; ++j)
        {
            
            int matchScore = (seq1[i - 1] == seq2[j - 1]) ? match : mismatch;
            matrix[i][j] = std::max({0,
                                    matrix[i - 1][j - 1] + matchScore,
                                    matrix[i - 1][j] + gap,
                                    matrix[i][j - 1] + gap});
            if (matrix[i][j] > maxScore)
            {
                maxScore = matrix[i][j];
                maxI = i;
                maxJ = j;
            }
        }
    }
    return std::make_tuple(maxScore, maxI, maxJ);
}

std::pair<std::string, std::string> smithWaterman(const char *seq1, size_t size1, const char *seq2, size_t size2) {
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
    std::tuple<int, int, int> block_out;
    int num_blocks_seq1 = (size1 + _BLOCK_SIZE_ - 1) / _BLOCK_SIZE_;
    //includes irregularly shaped blocks
    int num_blocks_seq2 = (size2 + _BLOCK_SIZE_ - 1) / _BLOCK_SIZE_;


    start = std::chrono::high_resolution_clock::now();
    //below, k, block_num_x, block_num_y start at 0
    //k is the number of the anti-diagonal
    //it is also the sum of the indexes of a block in it (if square)
    for (int k = 0; k < num_blocks_seq1 + num_blocks_seq2 - 1; ++k) {
                                            // -2 since its 0 indexed
        // is going through the diagonal
        // block_num_x + block_num_y = k

        // x is in i direction
        // y is in j direction
        // the matrix is j major (column major)
        // thesse diagonals are going up and to the right
        #pragma omp parallel for schedule(dynamic)
        for (int block_num_x = 0; block_num_x <= k; ++block_num_x) {
            int block_num_y = k - block_num_x;
            // Check if the bounds
            // diagonals go up right, so need to check negative y, overbound x, and overbound y
            if (block_num_y >= 0 && block_num_x < num_blocks_seq1 && block_num_y < num_blocks_seq2) {
                int start_i = block_num_x * _BLOCK_SIZE_ + 1;
                int start_j = block_num_y * _BLOCK_SIZE_ + 1;
                int end_i = min(start_i + _BLOCK_SIZE_ - 1, (int)size1);
                int end_j = min(start_j + _BLOCK_SIZE_ - 1, (int)size2);
                // std::cout << start_i << "_" << end_i << "|" << start_j << "_" << end_j << " num_x = " << block_num_x << " num_y = " << block_num_y << std::endl;
                block_out = process_block(start_i, end_i, start_j, end_j, score, seq1, seq2);
                if (std::get<0>(block_out) > maxScore) {
                    #pragma omp critical
                    {
                        maxScore = std::get<0>(block_out);
                        maxI = std::get<1>(block_out);
                        maxJ = std::get<2>(block_out);
                    }
                }
            }
        }
        // std::cout << std::endl;  // New line for each anti-diagonal
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