# Longest Increasing Subsequence (LIS)

## Introduction

Finds the length of the longest strictly increasing subsequence in an array. Classic DP problem with two solutions: O(n²) simple DP and O(n log n) patience sorting with binary search.

---

## Intuition

**O(n²) DP:** `dp[i]` = length of LIS ending at index `i`. For each `i`, scan all `j < i` where `nums[j] < nums[i]` and take `dp[i] = max(dp[j]) + 1`.

**O(n log n):** Maintain a list `tails[]` where `tails[k]` = the smallest possible tail element of all increasing subsequences of length `k+1`. For each new number, binary search for where it fits: if it extends, append; if it improves, replace. `tails.size()` is the LIS length.

The O(n log n) approach is not directly reconstructing the LIS — it's computing its length through a cleverly maintained structure.

---

## When to Use

- Longest Increasing Subsequence.
- Longest Non-Decreasing Subsequence (change `<` to `<=`).
- Longest Chain of pairs (sort first, then LIS on second element).
- Russian Doll Envelopes (2D LIS).
- Any problem reducible to "longest chain where each element must strictly follow the previous."

---

## Recognition Pattern

```
"Longest subsequence where each element is larger than the previous"
"Sequence of pairs where (a,b) fits inside (c,d)"
"Longest increasing chain"
```
→ Think LIS.

---

## Complexity Analysis

| Approach       | Time       | Space |
|----------------|------------|-------|
| DP simple      | O(n²)      | O(n)  |
| Binary search  | O(n log n) | O(n)  |

---

## Visualization

```
nums = [10, 9, 2, 5, 3, 7, 101, 18]

O(n²) DP:
dp =  [ 1,  1,  1,  2,  2,  3,   4,   4]
       10   9   2   5   3   7  101  18

LIS = 4 (e.g., 2, 3, 7, 101)

O(n log n) — tails array:

10   → tails=[10]
9    → replace 10 → tails=[9]
2    → replace 9  → tails=[2]
5    → extend    → tails=[2,5]
3    → replace 5  → tails=[2,3]
7    → extend    → tails=[2,3,7]
101  → extend    → tails=[2,3,7,101]
18   → replace 101→ tails=[2,3,7,18]

LIS length = tails.size() = 4
```

---

## Critical Code Explanation

### O(n log n) — lower_bound for strict LIS

```cpp
auto it = lower_bound(tails.begin(), tails.end(), x);
// lower_bound: finds first position >= x → replaces tails[pos]
// For non-decreasing: use upper_bound
```

For strict LIS, use `lower_bound` (finds first element ≥ x). If `x` equals an existing tail, it replaces it without extending. For non-decreasing LIS, use `upper_bound` (allows equal elements to extend).

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// O(n²) DP — easier to extend for reconstruction
int lisDP(vector<int>& nums) {
    int n = nums.size(), res = 1;
    vector<int> dp(n, 1);

    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++)
            if (nums[j] < nums[i]) dp[i] = max(dp[i], dp[j] + 1);
        res = max(res, dp[i]);
    }
    return res;
}

// O(n log n) — patience sorting with binary search
int lisBinarySearch(vector<int>& nums) {
    vector<int> tails;
    for (int x : nums) {
        auto it = lower_bound(tails.begin(), tails.end(), x);
        if (it == tails.end()) tails.push_back(x);
        else *it = x;
    }
    return tails.size();
}

// LIS reconstruction using O(n²) DP + parent tracking
vector<int> lisReconstruct(vector<int>& nums) {
    int n = nums.size();
    vector<int> dp(n, 1), parent(n, -1);
    int maxLen = 1, endIdx = 0;

    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (nums[j] < nums[i] && dp[j] + 1 > dp[i]) {
                dp[i] = dp[j] + 1;
                parent[i] = j;
            }
        }
        if (dp[i] > maxLen) { maxLen = dp[i]; endIdx = i; }
    }

    vector<int> lis;
    for (int i = endIdx; i != -1; i = parent[i])
        lis.push_back(nums[i]);
    reverse(lis.begin(), lis.end());
    return lis;
}
```

---

## Why It Works

**O(n²):** For each position `i`, the LIS ending at `i` extends the best LIS ending at any previous position with a smaller value. The recurrence correctly captures all possibilities.

**O(n log n) — tails invariant:** `tails[k]` is the minimum possible ending element of any IS of length `k+1`. This invariant is maintained because we replace the first element in `tails` that is ≥ the new element (not extending — just improving the tail of an existing-length IS). The length of `tails` equals the LIS length because every extension adds to `tails`.

---

## Important Notes

- The `tails` array in the O(n log n) approach does **not** represent an actual LIS — it's a cleverly maintained structure for computing the length. Use the O(n²) approach with parent tracking for actual reconstruction.
- For **Longest Non-Decreasing Subsequence**, replace `lower_bound` with `upper_bound` (allows equal elements to extend the subsequence).
- **Russian Doll Envelopes** (LeetCode 354): sort by width ascending and height descending, then run LIS on heights. The descending height sort prevents multiple same-width envelopes from being chained.
- LIS can be reduced to LCS: `LIS(nums) = LCS(nums, sorted(nums))`. Useful for understanding but not practically faster.
