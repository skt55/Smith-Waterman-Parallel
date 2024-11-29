#include <cuda.h>
#include <cuda_runtime.h>
#include <math.h>
#include <iostream>
#include <bits/stdc++.h>

#define MATCH_SCORE 2
#define MISMATCH_SCORE -1
#define GAP_PENALTY -1

//variable names taken from GPU computing gems
//this does a row of the matrix

//matrices is row major
__global__ void rowscan(int *H, int* E_bar, int *F, size_t row_length, int curr_row, char* seq1, char* seq2) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    
    //remember that col 0 is all 0
    if (i > 0 && i <= row_length) {
        // as in the book
        int matchScore = (seq1[i - 1] == seq2[curr_row - 1]) ? MATCH_SCORE : MISMATCH_SCORE;

        //F part
        int F_up = F[i + (curr_row - 1) * row_length];
        int H_up = H[i + (curr_row - 1) * row_length];
        int F_me = max(F_up, H_up - 1) - 1;
        F[i + (curr_row) * row_length] = F_me;

        //Hbar
        int H_diag = H[i - 1 + (curr_row - 1) * row_length];
        int H_bar_me = max(max(H_diag + matchScore, F_me), 0);


        //START OF E_BAR PART
        //MAX PREFIIX SCAN

        //this is as wide as the row
        extern __shared__ int shared_data[];

        int tid = threadIdx.x;
        int idx = blockIdx.x * blockDim.x + threadIdx.x;
        
        //this will not hold the zero in col 0
        shared_data[i - 1] = H[i];
        
        __syncthreads();

        // Perform the prefix maximum operation in shared memory
        for (int offset = 1; offset < blockDim.x; offset *= 2) {
            int temp = 0;
            if (i - 1 >= offset) {
                temp = shared_data[i - offset - 1];
            }
            __syncthreads();
            shared_data[i - 1] = max(shared_data[i - 1], temp);
            __syncthreads();
        }

        E_bar[i + (curr_row) * row_length] = shared_data[i - 1];
        // Write the result to global memory


        //finally H
        int E_bar_me = E_bar[i + (curr_row) * row_length];
        int H_me = max(H_bar_me, E_bar_me - 1);

        H[i + (curr_row) * row_length] = H_me; 
    }
}

/*
__global__ void maxPrefixScan_Ebar(int* input, int* output, int N) {
    extern __shared__ int shared_data[];

        int tid = threadIdx.x;
        int idx = blockIdx.x * blockDim.x + threadIdx.x;

        if (idx < N) {
            shared_data[tid] = input[idx];
        }
        __syncthreads();

        // Perform the prefix maximum operation in shared memory
        for (int offset = 1; offset < blockDim.x; offset *= 2) {
            int temp = 0;
            if (tid >= offset) {
                temp = shared_data[tid - offset];
            }
            __syncthreads();
            shared_data[tid] = max(shared_data[tid], temp);
            __syncthreads();
        }

        // Write the result to global memory
        if (idx < N) {
            output[idx] = shared_data[tid];
        }
}
*/

std::pair<std::string, std::string> smithWaterman(const char *seq1, size_t size1, const char *seq2, size_t size2) {
    // Allocate memory on the device
    int *d_H, *d_E, *d_F, *d_S, *d_Gs, *d_Ge;
    cudaMalloc(&d_H, sizeof(int) * (size1 + 1) * (size2 + 1));
    cudaMalloc(&d_E, sizeof(int) * (size1 + 1) * (size2 + 1));
    cudaMalloc(&d_F, sizeof(int) * (size1 + 1) * (size2 + 1);

    // Copy data to device
    cudaMemset(d_H, 0, sizeof(int) * (size1 + 1) * (size2 + 1));
    cudaMemset(d_E, 0, sizeof(int) * (size1 + 1) * (size2 + 1));
    cudaMemset(d_F, 0, sizeof(int) * (size1 + 1) * (size2 + 1));


    // Define block and grid dimensions
    dim3 blockDim(16, 16);
    dim3 gridDim((n + blockDim.x - 1) / blockDim.x, (m + blockDim.y - 1) / blockDim.y);

    // Launch kernel
    for (int row = 1; row <= size2; row++ ) {
        rowscan<<<gridDim, blockDim>>>(d_H, d_E, d_F, d_S, d_Gs, d_Ge, n, m);
    }

    // Copy result back to host
    int* H = (int*)malloc(sizeof(int) * (size1 + 1) * (size2 + 1));
    cudaMemcpy(H, d_H, sizeof(int) * (size1 + 1) * (size2 + 1), cudaMemcpyDeviceToHost);

    // Free device memory
    cudaFree(d_H);
    cudaFree(d_E);
    cudaFree(d_F);

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