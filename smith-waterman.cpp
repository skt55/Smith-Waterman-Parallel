#include <algorithm>
#include <iostream>
#include <bits/stdc++.h>

using namespace std;

int smithWaterman(const std::string &seq1, const std::string &seq2)
{
    int match = 2;     // Score for a match
    int mismatch = -1; // Score for a mismatch
    int gap = -1;      // Score for a gap

    int n = seq1.length();
    int m = seq2.length();
    std::vector<std::vector<int>> score(n + 1, std::vector<int>(m + 1, 0));

    int maxScore = 0;

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
            maxScore = std::max(maxScore, score[i][j]);
        }
    }

    return maxScore; // Return the maximum score found
}
