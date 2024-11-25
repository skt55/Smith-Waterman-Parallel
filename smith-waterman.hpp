// smith-waterman.h
#ifndef SMITH_WATERMAN_H
#define SMITH_WATERMAN_H

#include <string>
#include <vector>

// Function to perform the Smith-Waterman algorithm
std::pair<std::string, std::string> smithWaterman(const std::string &seq1, const std::string &seq2);

#endif // SMITH_WATERMAN_H