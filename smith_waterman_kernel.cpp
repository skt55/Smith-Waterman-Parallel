#include <algorithm>
#include <iostream>
#include <bits/stdc++.h>
#include "smith-waterman.hpp"
#include <immintrin.h>

using namespace std;

const int _BLOCK_SIZE_ = 64;
// Define gap penalties and match/mismatch scores
#define MATCH_SCORE 2
#define MISMATCH_SCORE -1
#define GAP_PENALTY -1



void process_anti_diagonal(int sum, int N, int M,
                           std::vector<std::vector<int>>& matrix,
                           const std::string& seq1, const std::string& seq2,
                           int match, int mismatch, int gap,
                           int& maxScore, int& maxI, int& maxJ) {
    std::vector<int> i_vals, j_vals;

    for (int i = 1; i <= N; ++i) {
        int j = sum - i;
        if (j >= 1 && j <= M) {
            i_vals.push_back(i);
            j_vals.push_back(j);
        }
    }

    int size = i_vals.size();
    int simd_width = 8; // For AVX2 (256 bits / 32 bits per int)

    for (int k = 0; k < size; k += simd_width) {
        int chunk_size = std::min(simd_width, size - k);

        // Load i and j values
        int i_chunk[8] = {0}, j_chunk[8] = {0};
        for (int idx = 0; idx < chunk_size; ++idx) {
            i_chunk[idx] = i_vals[k + idx];
            j_chunk[idx] = j_vals[k + idx];
        }
        __m256i vec_i = _mm256_loadu_si256((__m256i*)i_chunk);
        __m256i vec_j = _mm256_loadu_si256((__m256i*)j_chunk);

        // Load sequences
        int seq1_chars[8] = {0}, seq2_chars[8] = {0};
        for (int idx = 0; idx < chunk_size; ++idx) {
            seq1_chars[idx] = seq1[i_vals[k + idx] - 1];
            seq2_chars[idx] = seq2[j_vals[k + idx] - 1];
        }

        // Load match/mismatch scores
        __m256i vec_seq1 = _mm256_loadu_si256((__m256i*)seq1_chars);
        __m256i vec_seq2 = _mm256_loadu_si256((__m256i*)seq2_chars);

        __m256i vec_match = _mm256_set1_epi32(match);
        __m256i vec_mismatch = _mm256_set1_epi32(mismatch);

        // Compare characters
        __m256i vec_cmp = _mm256_cmpeq_epi32(vec_seq1, vec_seq2);
        __m256i vec_matchScore = _mm256_blendv_epi8(vec_mismatch, vec_match, vec_cmp);

        // Load previous scores
        int diag_scores[8] = {0}, up_scores[8] = {0}, left_scores[8] = {0};
        for (int idx = 0; idx < chunk_size; ++idx) {
            int i = i_vals[k + idx];
            int j = j_vals[k + idx];
            diag_scores[idx] = matrix[i - 1][j - 1];
            up_scores[idx] = matrix[i - 1][j];
            left_scores[idx] = matrix[i][j - 1];
        }

        __m256i vec_diag = _mm256_loadu_si256((__m256i*)diag_scores);
        __m256i vec_up = _mm256_loadu_si256((__m256i*)up_scores);
        __m256i vec_left = _mm256_loadu_si256((__m256i*)left_scores);

        __m256i vec_gap = _mm256_set1_epi32(gap);

        // Compute scores
        __m256i score_diag = _mm256_add_epi32(vec_diag, vec_matchScore);
        __m256i score_up = _mm256_add_epi32(vec_up, vec_gap);
        __m256i score_left = _mm256_add_epi32(vec_left, vec_gap);

        // Compute max scores
        __m256i vec_zero = _mm256_setzero_si256();
        __m256i temp_max = _mm256_max_epi32(score_diag, score_up);
        temp_max = _mm256_max_epi32(temp_max, score_left);
        temp_max = _mm256_max_epi32(temp_max, vec_zero);

        // Store results
        int scores[8] = {0};
        _mm256_storeu_si256((__m256i*)scores, temp_max);

        for (int idx = 0; idx < chunk_size; ++idx) {
            int i = i_vals[k + idx];
            int j = j_vals[k + idx];
            matrix[i][j] = scores[idx];

            // Update max score
            if (scores[idx] > maxScore) {
                maxScore = scores[idx];
                maxI = i;
                maxJ = j;
            }
        }
    }
}


//start and end are inclusive
std::tuple<int, int, int> process_block(int start_i, int end_i, int start_j, int end_j, 
                   std::vector<std::vector<int>>& matrix, const std::string& seq1, const std::string& seq2) {
    int match = 2;     // Score for a match
    int mismatch = -1; // Score for a mismatch
    int gap = -1;      // Score for a gap

    int maxScore = 0;
    int maxI = 0;
    int maxJ = 0;

    int N = end_i - start_i + 1;
    int M = end_j - start_j + 1;

    for (int sum = 2; sum <= N + M; ++sum) {
        process_anti_diagonal(sum, N, M, matrix, seq1, seq2, match, mismatch, gap, maxScore, maxI, maxJ);
    }

    return std::make_tuple(maxScore, maxI, maxJ);
}

std::pair<std::string, std::string> smithWaterman(const char *seq1, size_t size1, const char *seq2, size_t size2) {
    int match = 2;     // Score for a match
    int mismatch = -1; // Score for a mismatch
    int gap = -1;      // Score for a gap

    std::vector<std::vector<int>> score(size1 + 1, std::vector<int>(size2 + 1, 0));

    int maxScore = 0;
    int maxI = 0, maxJ = 0;

    std::tuple<int, int, int> block_out;
    // Process blocks in parallel
    for (size_t start_i = 1; start_i < size1; start_i += _BLOCK_SIZE_) {
        for (size_t start_j = 1; start_j < size2; start_j += _BLOCK_SIZE_) {
            int end_i = min(start_i + _BLOCK_SIZE_ - 1, size1);
            int end_j = min(start_j + _BLOCK_SIZE_ - 1, size2);
            //std::cout << start_i << "_" << end_i << "|" << start_j << "_" << end_j << std::endl;
            block_out = process_block(start_i, end_i, start_j, end_j, score, seq1, seq2);
            if (std::get<0>(block_out) > maxScore) {
                maxScore = std::get<0>(block_out);
                maxI = std::get<1>(block_out);
                maxJ = std::get<2>(block_out);
            }
        }
    }

    // Backtrack to find the aligned sequences
    std::string alignedSeq1, alignedSeq2;
    size_t i = maxI, j = maxJ;

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

    // Reverse the aligned sequences
    std::reverse(alignedSeq1.begin(), alignedSeq1.end());
    std::reverse(alignedSeq2.begin(), alignedSeq2.end());

    return {alignedSeq1, alignedSeq2}; // Return the aligned sequences
}
