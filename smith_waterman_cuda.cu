#include <chrono>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cuda_runtime.h>

// This kenel fills the dp matrix based on 3 of its neighbors and penalties
__global__ void smith_waterman_kernel_optimized(
    const char *__restrict__ seq1,
    const char *__restrict__ seq2,
    int *__restrict__ score,
    int size1,
    int size2,
    int match,
    int mismatch,
    int gap,
    int diag)
{
    // shared memory used by threads in a block
    extern __shared__ int shared_score[];
    // current thread parameters
    int thread_id = threadIdx.x;
    int global_idx = blockIdx.x * blockDim.x + threadIdx.x;

    unsigned long long i = diag - global_idx;
    unsigned long long j = global_idx + 1;

    // checking bounds
    if (i < 1 || i > size1 || j < 1 || j > size2) return;

    // Compute indices
    unsigned long long index = i * (size2 + 1) + j;
    unsigned long long index_diag = (i - 1) * (size2 + 1) + (j - 1);
    unsigned long long index_up = (i - 1) * (size2 + 1) + j;
    unsigned long long index_left = i * (size2 + 1) + (j - 1);

    // checking miss match or match
    char a = seq1[i - 1];
    char b = seq2[j - 1];
    int matchScore = (a == b) ? match : mismatch;

    // Use shared memory to store neighboring values
    shared_score[thread_id] = score[index_diag];

    __syncthreads();

    // computing all the possibilites, removed conditional statements for speed up
    int score_diag = shared_score[thread_id] + matchScore;
    int score_up = score[index_up] + gap;
    int score_left = score[index_left] + gap;

    //updating the matrix
    int cellScore = max(0, max(score_diag, max(score_up, score_left)));
    score[index] = cellScore;
}

std::pair<std::string, std::string> smithWaterman(
    const char *seq1,
    size_t size1,
    const char *seq2,
    size_t size2)
{
    // penalties
    int match = 2, mismatch = -1, gap = -1;
    std::vector<int> hscore((size1 + 1) * (size2 + 1), 0);

    char *cuda_seq1, *cuda_seq2;
    int *cuda_score;
    auto start1 = std::chrono::high_resolution_clock::now();

    // assiging memory on the device
    cudaMalloc((void **)&cuda_seq1, size1 * sizeof(char));
    cudaMalloc((void **)&cuda_seq2, size2 * sizeof(char));
    cudaMalloc((void **)&cuda_score, (size1 + 1) * (size2 + 1) * sizeof(int));
    auto start2 = std::chrono::high_resolution_clock::now();

    // sending sequences to the device
    cudaMemcpy(cuda_seq1, seq1, size1 * sizeof(char), cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_seq2, seq2, size2 * sizeof(char), cudaMemcpyHostToDevice);
    cudaMemset(cuda_score, 0, (size1 + 1) * (size2 + 1) * sizeof(int));
    auto start3 = std::chrono::high_resolution_clock::now();

    // moving in wave-front (anti-diagonal order)
    int total_diagonals = size1 + size2 - 1;
    int threads_per_block = 1024; 
    // shared memory size to be used by each thread in a block
    int shared_mem_size = threads_per_block * sizeof(int);

    for (int diag = 1; diag <= total_diagonals; ++diag)
    {
        int elements_in_diag = min(diag, min(static_cast<int>(size1), static_cast<int>(size2)));
        // number of blocks required for the given diagonal
        int blocks = (elements_in_diag + threads_per_block - 1) / threads_per_block;

        smith_waterman_kernel_optimized<<<blocks, threads_per_block, shared_mem_size>>>(
            cuda_seq1, cuda_seq2, cuda_score,
            size1, size2, match, mismatch, gap, diag);
        // this sync ensures , nxt diagonal is solved only when prev is completed
        cudaDeviceSynchronize();
    }
    auto start4 = std::chrono::high_resolution_clock::now();

    // copying data back from the device to the host
    cudaMemcpy(hscore.data(), cuda_score, (size1 + 1) * (size2 + 1) * sizeof(int), cudaMemcpyDeviceToHost);
    auto start5 = std::chrono::high_resolution_clock::now();

    
    int maxI = 0,maxJ;
    int count = 0;
    for(int row = 0;row<=size1;++row){
        for(int col = 0;col<=size2;++col){
            int curr_element = hscore[row*(size1+1) + col];
            if(count<curr_element){
                maxI = row;
                maxJ = col;
                count = curr_element;
            }
         
        }
       
    }
    auto start6 = std::chrono::high_resolution_clock::now();

    // Backtrack to find the aligned sequences
    std::string alignedSeq1, alignedSeq2;
    int i = maxI, j = maxJ;
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

    auto start7 = std::chrono::high_resolution_clock::now();

    cudaFree(cuda_seq1);
    cudaFree(cuda_seq2);
    cudaFree(cuda_score);   
    auto start8 = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = start2 - start1;
    std::cout << "Memory1:           " << duration.count() << " seconds" << std::endl;

    std::chrono::duration<double> duration1 = start3 - start2;
    std::cout << "Memset/cpy:        " << duration1.count() << " seconds" << std::endl;

    std::chrono::duration<double> duration2 = start4 - start3;
    std::cout << "kernel time taken: " << duration2.count() << " seconds" << std::endl;

    std::chrono::duration<double> duration3 = start5 - start4;
    std::cout << "cpy2 time taken:   " << duration3.count() << " seconds" << std::endl;

    std::chrono::duration<double> duration4 = start6 - start5;
    std::cout << "max indices fetch: " << duration4.count() << " seconds" << std::endl;

    std::chrono::duration<double> duration5 = start7 - start6;
    std::cout << "backtrack time:    " << duration5.count() << " seconds" << std::endl;

     std::chrono::duration<double> duration6 = start8 - start7;
    std::cout << "cuda free:        " << duration6.count() << " seconds" << std::endl;
    // Reverse the aligned sequences
    std::reverse(alignedSeq1.begin(), alignedSeq1.end());
    std::reverse(alignedSeq2.begin(), alignedSeq2.end());
    std::cout << "Total cuda time is " << duration.count() + duration1.count() + duration2.count() + duration3.count() + duration6.count()<< "seconds" << std::endl;
    return {alignedSeq1, alignedSeq2}; // Return the aligned sequences
}



    
