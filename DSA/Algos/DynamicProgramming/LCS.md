# Longest Common Subsequence (LCS)

## Introduction

Given two strings, find the length of their longest common subsequence — the longest sequence of characters that appears in both strings in the same relative order (not necessarily contiguous).

---

## Intuition

Define `dp[i][j]` = length of LCS of `s1[0..i-1]` and `s2[0..j-1]`. If the last characters match, they must be in any longest LCS ending here, so `dp[i][j] = dp[i-1][j-1] + 1`. If they don't match, the LCS either uses `s1[0..i-1]` with all of `s2[0..j]`, or all of `s1[0..i]` with `s2[0..j-1]` — take the max.

---

## When to Use

- Longest Common Subsequence.
- Minimum edit distance base (LCS ↔ edit distance connection).
- Minimum insertions/deletions to make two strings equal.
- Shortest Common Supersequence.
- Diff utilities (git diff is fundamentally LCS).

---

## Complexity Analysis

| Case    | Time   | Space  |
|---------|--------|--------|
| Best    | O(m·n) | O(m·n) |
| Average | O(m·n) | O(m·n) |
| Worst   | O(m·n) | O(m·n) |

Space can be reduced to O(min(m, n)) with rolling array (only need previous row).

---

## Visualization

```
s1 = "ABCBDAB", s2 = "BDCABA"

     ""  B  D  C  A  B  A
""  [ 0,  0,  0,  0,  0,  0,  0]
A   [ 0,  0,  0,  0,  1,  1,  1]
B   [ 0,  1,  1,  1,  1,  2,  2]
C   [ 0,  1,  1,  2,  2,  2,  2]
B   [ 0,  1,  1,  2,  2,  3,  3]
D   [ 0,  1,  2,  2,  2,  3,  3]
A   [ 0,  1,  2,  2,  3,  3,  4]
B   [ 0,  1,  2,  2,  3,  4,  4]

LCS length = dp[7][6] = 4 (e.g., "BCBA" or "BDAB")
```

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// LCS length — O(m*n) space
int lcs(string& s1, string& s2) {
    int m = s1.size(), n = s2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

    for (int i = 1; i <= m; i++)
        for (int j = 1; j <= n; j++) {
            if (s1[i-1] == s2[j-1]) dp[i][j] = dp[i-1][j-1] + 1;
            else dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
        }
    return dp[m][n];
}

// Space-optimized: O(min(m,n))
int lcsOptimized(string& s1, string& s2) {
    if (s1.size() < s2.size()) swap(s1, s2);  // s2 is shorter
    int m = s1.size(), n = s2.size();
    vector<int> prev(n + 1, 0), curr(n + 1, 0);

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i-1] == s2[j-1]) curr[j] = prev[j-1] + 1;
            else curr[j] = max(prev[j], curr[j-1]);
        }
        swap(prev, curr);
    }
    return prev[n];
}

// Minimum insertions + deletions to convert s1 to s2
// = (len(s1) - lcs) + (len(s2) - lcs) = m + n - 2*lcs
int minInsertDeleteOps(string& s1, string& s2) {
    int l = lcs(s1, s2);
    return s1.size() + s2.size() - 2 * l;
}

// Shortest Common Supersequence length = m + n - lcs
int shortestCommonSuperseq(string& s1, string& s2) {
    return s1.size() + s2.size() - lcs(s1, s2);
}
```

---

## Important Notes

- `dp[i][j]` uses 1-indexed strings (`s1[i-1]`, `s2[j-1]`) to simplify the `i-1, j-1` base case (row/column 0 is all zeros = empty string LCS).
- **Relationship to Edit Distance:** `editDist(s1, s2) = m + n - 2·LCS(s1, s2)` when only insertions and deletions are allowed (not substitutions). With substitutions, use full edit distance DP.
- LCS of a string with its reverse = **Longest Palindromic Subsequence**.
- For reconstructing the actual LCS (not just length), backtrack through the DP table: when `s1[i-1] == s2[j-1]`, go diagonal; otherwise go in the direction of the larger value.
