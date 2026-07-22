# 0/1 Knapsack

## Introduction

Given n items each with a weight and value, and a knapsack of capacity W, find the maximum total value that can be packed without exceeding W. Each item can be included at most once.

---

## Intuition

For each item, you face a binary choice: include it or skip it. If you include item `i`, you consume `wt[i]` capacity but gain `val[i]`. Define `dp[i][w]` = maximum value using the first `i` items with capacity `w`. The transition is: either skip item `i` (same as `dp[i-1][w]`) or include it (if it fits: `dp[i-1][w-wt[i]] + val[i]`). Take the max.

---

## When to Use

- "Subset selection with weight/cost constraint".
- "Can we achieve exactly sum S?" (variant: 0/1 knapsack with value = weight).
- Partition equal subset sum (special case).
- Any "include or exclude each item exactly once" problem.

---

## Recognition Pattern

```
Items with weight and value
+ capacity constraint
+ each item used at most once
"Maximum value", "Subset sum", "Partition equal"
```
→ Think 0/1 Knapsack.

---

## Complexity Analysis

| Case    | Time   | Space  |
|---------|--------|--------|
| Best    | O(n·W) | O(n·W) |
| Average | O(n·W) | O(n·W) |
| Worst   | O(n·W) | O(n·W) |

Space can be reduced to O(W) using 1D DP.

---

## Core Idea

`dp[i][w]` = max value from items `1..i` with capacity `w`.

Transition:
```
dp[i][w] = dp[i-1][w]                              // skip item i
          = max(dp[i-1][w], dp[i-1][w-wt[i]] + val[i])  // if wt[i] <= w
```

Base case: `dp[0][w] = 0` for all `w` (no items → no value).

---

## Visualization

```
items: wt=[1,3,4,5], val=[1,4,5,7], W=7

dp table (rows=items 0..4, cols=capacity 0..7):

     0  1  2  3  4  5  6  7
i=0: 0  0  0  0  0  0  0  0
i=1: 0  1  1  1  1  1  1  1   (item1: wt=1, val=1)
i=2: 0  1  1  4  5  5  5  5   (item2: wt=3, val=4)
i=3: 0  1  1  4  5  6  6  9   (item3: wt=4, val=5)
i=4: 0  1  1  4  5  7  8  9   (item4: wt=5, val=7)

Answer: dp[4][7] = 9
```

---

## Critical Code Explanation

### The Transition

```cpp
dp[i][w] = dp[i-1][w];   // skip: same as previous row
if (w >= wt[i-1])         // include: only if item fits
    dp[i][w] = max(dp[i][w], dp[i-1][w - wt[i-1]] + val[i-1]);
```

`w - wt[i-1]` is the remaining capacity after including item `i`. `dp[i-1][...]` ensures each item is only used once (look at the previous row, not the current row).

### 1D Space Optimization — Traverse Right to Left

```cpp
for (int i = 0; i < n; i++)
    for (int w = W; w >= wt[i]; w--)   // RIGHT TO LEFT
        dp[w] = max(dp[w], dp[w - wt[i]] + val[i]);
```

Traversing right to left in the 1D array ensures that when we compute `dp[w]`, `dp[w - wt[i]]` still holds the value from the previous item (not the current item's already-updated value). Left-to-right traversal would allow an item to be used multiple times (Unbounded Knapsack behavior).

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// 2D DP — O(n*W) space
int knapsack2D(vector<int>& wt, vector<int>& val, int W) {
    int n = wt.size();
    vector<vector<int>> dp(n + 1, vector<int>(W + 1, 0));

    for (int i = 1; i <= n; i++)
        for (int w = 0; w <= W; w++) {
            dp[i][w] = dp[i-1][w];
            if (w >= wt[i-1])
                dp[i][w] = max(dp[i][w], dp[i-1][w - wt[i-1]] + val[i-1]);
        }
    return dp[n][W];
}

// 1D DP — O(W) space
int knapsack(vector<int>& wt, vector<int>& val, int W) {
    int n = wt.size();
    vector<int> dp(W + 1, 0);

    for (int i = 0; i < n; i++)
        for (int w = W; w >= wt[i]; w--)   // right to left
            dp[w] = max(dp[w], dp[w - wt[i]] + val[i]);

    return dp[W];
}

// Subset sum — can we form sum S?
bool subsetSum(vector<int>& nums, int S) {
    vector<bool> dp(S + 1, false);
    dp[0] = true;
    for (int x : nums)
        for (int s = S; s >= x; s--)
            dp[s] = dp[s] || dp[s - x];
    return dp[S];
}
```

---

## Why It Works

The DP builds optimal solutions to smaller subproblems (fewer items, smaller capacity) and combines them. The transition correctly captures the binary choice: including or excluding item `i`. The right-to-left iteration in 1D ensures we reference the state before item `i` was considered, preventing item reuse.

---

## Important Notes

- **0/1** means each item can be used at most once. For **unbounded knapsack** (unlimited copies), traverse **left to right** in 1D DP.
- **Subset Sum** is a special case of 0/1 Knapsack where `val[i] = wt[i]` and the goal is `dp[S] == true` (or `== S`).
- **Partition Equal Subset Sum** reduces to subset sum with `S = totalSum / 2`.
- The time complexity is O(n·W) which is pseudo-polynomial — W can be very large (up to 10⁹ in some problems), making this infeasible without approximation.
- When items have fractional weights/values, use the **Fractional Knapsack** (greedy: sort by value/weight ratio).
