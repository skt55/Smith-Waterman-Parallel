#include <algorithm>
#include <iostream>
#include <bits/stdc++.h>
#include "smith-waterman.hpp"

using namespace std;

std::pair<std::string, std::string> smithWaterman(const std::string &seq1, const std::string &seq2)
{
    int match = 2;     // Score for a match
    int mismatch = -1; // Score for a mismatch
    int gap = -1;      // Score for a gap

    int n = seq1.length();
    int m = seq2.length();
    std::vector<std::vector<int>> score(n + 1, std::vector<int>(m + 1, 0));

    int maxScore = 0;
    int maxI = 0, maxJ = 0;

    // Fill the scoring matrix
    for (int i = 1; i <= n; ++i)
    {
        for (int j = 1; j <= m; ++j)
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

    // Backtrack to find the aligned sequences
    std::string alignedSeq1, alignedSeq2;
    int i = maxI, j = maxJ;

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