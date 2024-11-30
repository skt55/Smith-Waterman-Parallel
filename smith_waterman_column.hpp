#ifndef SMITH_WATERMAN_OPENMP_H
#define SMITH_WATERMAN_OPENMP_H

#include <algorithm>
#include <iostream>
#include <bits/stdc++.h>
#include "smith-waterman.hpp"

// Function to perform the Smith-Waterman algorithm
std::tuple<int, int, int> process_block(int start_i, int end_i, int start_j, int end_j, 
                   std::vector<std::vector<int>>& matrix, const std::string& seq1, const std::string& seq2);

std::pair<std::string, std::string> smithWaterman(const char *seq1, size_t size1, const char *seq2, size_t size2);

#endif // SMITH_WATERMAN_OPENMP_H