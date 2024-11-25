#ifndef SMITH_WATERMAN_H
#define SMITH_WATERMAN_H

#include <string>
#include <vector>

// Function to perform the Smith-Waterman algorithm
std::pair<std::string, std::string> smithWaterman(const char *seq1, size_t size1, const char *seq2, size_t size2);

#endif // SMITH_WATERMAN_H