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
    //std::cout<<num_blocks_seq1<<endl;
    //includes irregularly shaped blocks
    int num_blocks_seq2 = (size2 + _BLOCK_SIZE_ - 1) / _BLOCK_SIZE_;
    //std::cout<<num_blocks_seq2<<endl;


    start = std::chrono::high_resolution_clock::now();
    #pragma omp parallel num_threads(num_blocks_seq1)
    {
        //this will run total number of blocks per column times
        int i = omp_get_thread_num();
        int start_i = i * _BLOCK_SIZE_ + 1;
        int end_i = min(start_i + _BLOCK_SIZE_ - 1, (int)size1);

        for(int j = -i; j < num_blocks_seq2+num_blocks_seq1 - 1 - i; j++){
            int start_j = j * _BLOCK_SIZE_ + 1;
            int end_j = min(start_j + _BLOCK_SIZE_ - 1, (int)size2);
            
            if(j >= 0 && j < num_blocks_seq2){
                block_out = process_block(start_i, end_i, start_j, end_j, score, seq1, seq2);
                //#pragma omp critical
                //{
                //    cout<<start_i<<"_"<<end_i<<"|"<<start_j<<"_"<<end_j<<"|t"<<i<<endl;
                //}
                if (std::get<0>(block_out) > maxScore) {
                    #pragma omp critical
                    {
                        maxScore = std::get<0>(block_out);
                        maxI = std::get<1>(block_out);
                        maxJ = std::get<2>(block_out);
                    }
                }
            }
            #pragma omp barrier
            

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