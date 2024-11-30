#include <algorithm>
#include <iostream>
#include <bits/stdc++.h>
#include "smith-waterman.hpp"

using namespace std;

__global__ void smith_waterman_kernel(
        const char* __restrict__ seq1,
        const char* __restrict__ seq2,
        int* __restrict__ score,
        int* __restrict__ maxScore,
        int* __restrict__ maxI,
        int* __restrict__ maxJ,
        int size1,
        int size2,
        int match,
        int mismatch,
        int gap,
        int diag)
{
    // Compute the thread's position in the anti-diagonal
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    int i = diag - idx;
    int j = idx + 1;

    if (i < 1 || i > size1 || j < 1 || j > size2)
        return;

    // Linear index in the score matrix
    int index = i * (size2 + 1) + j;// considering row major ordering
    int index_diag = (i - 1) * (size2 + 1) + (j - 1);
    int index_up = (i - 1) * (size2 + 1) + j;
    int index_left = i * (size2 + 1) + (j - 1);

    char a = seq1[i - 1]; // Adjusting for 0-based indexing
    char b = seq2[j - 1];

    int matchScore = (a == b) ? match : mismatch;

    int score_diag = score[index_diag] + matchScore;
    int score_up = score[index_up] + gap;
    int score_left = score[index_left] + gap;

    int cellScore = max(0, max(score_diag, max(score_up, score_left)));
    score[index] = cellScore;

    // Update maximum score using atomic operations
    if (cellScore > 0)
    {
        int oldMax = atomicMax(maxScore, cellScore);
        if (cellScore > oldMax)
        {
            atomicExch(maxI, i);
            atomicExch(maxJ, j);
        }
    }
}




std::pair<std::string, std::string> smithWaterman(const char *seq1, size_t size1, const char *seq2, size_t size2)
{

    
    int match = 2;     // Score for a match
    int mismatch = -1; // Score for a mismatch
    int gap = -1;      // Score for a gap

    std::vector<int> hscore((size1+1)*(size2+1),0);
    
    //memory allocation
    
    char *cuda_seq1, *cuda_seq2;
    int *maxScore, *maxI, *maxJ, *score;
    int hmaxScore, hmaxI, hmaxJ;
    cudaMalloc((void**)&cuda_seq1, size1 * sizeof(char));
    cudaMalloc((void**)&cuda_seq2, size2 * sizeof(char));
    cudaMalloc((void**)&score, (size1 + 1) * (size2 + 1) * sizeof(int));
    cudaMalloc((void**)&maxScore, sizeof(int));
    cudaMalloc((void**)&maxI, sizeof(int));
    cudaMalloc((void**)&maxJ, sizeof(int));


    //copy data to the gpu
    cudaMemcpy(cuda_seq1, seq1, size1 * sizeof(char), cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_seq2, seq2, size2 * sizeof(char), cudaMemcpyHostToDevice);
    cudaMemset(score, 0, (size1 + 1) * (size2 + 1) * sizeof(int));
    cudaMemset(maxScore, 0, sizeof(int));
    cudaMemset(maxI, 0, sizeof(int));
    cudaMemset(maxJ, 0, sizeof(int));
    
    int total_diagonals = size1 + size2 - 1;

    for (int diag = 1; diag <= total_diagonals; ++diag)
    {
        // Explicitly cast size1 and size2 to int
        int elements_in_diag = min(diag, min(static_cast<int>(size1), static_cast<int>(size2)));

        int threads_per_block = 256;
        //total threads shoudl be atleast equal to number of elements in the digonal
        int blocks = (elements_in_diag + threads_per_block - 1) / threads_per_block;

        smith_waterman_kernel<<<blocks, threads_per_block>>>(
                cuda_seq1, cuda_seq2, score, maxScore, maxI, maxJ,
                size1, size2, match, mismatch, gap, diag);
        // all kernels should finish before moving to the next diagonal
        cudaDeviceSynchronize();
    }

    cudaMemcpy(hscore.data(),score, (size1 + 1) * (size2 + 1) * sizeof(int),cudaMemcpyDeviceToHost);
    cudaMemcpy(&hmaxScore, maxScore,sizeof(int),cudaMemcpyDeviceToHost);
    cudaMemcpy(&hmaxI, maxI, sizeof(int),cudaMemcpyDeviceToHost);
    cudaMemcpy(&hmaxJ,maxJ, sizeof(int),cudaMemcpyDeviceToHost);

    // Backtrack to find the aligned sequences
    std::string alignedSeq1, alignedSeq2;
    size_t i = hmaxI, j = hmaxJ;
    cout<<"in backtracking"<< i << "and" << j;
    while (i > 0 && j > 0 && (hscore[i*(size2+1)+ j] > 0))
    {
        if (seq1[i - 1] == seq2[j - 1])
        {
            alignedSeq1 += seq1[i - 1];
            alignedSeq2 += seq2[j - 1];
            i--;
            j--;
        }
        else if (hscore[i*(size2+1)+ j] == hscore[(i-1)*(size2+1)+ j] + gap)
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
    cudaFree(score);
    cudaFree(maxScore);
    cudaFree(maxI);
    cudaFree(maxJ);
    return {alignedSeq1, alignedSeq2}; // Return the aligned sequences
}
