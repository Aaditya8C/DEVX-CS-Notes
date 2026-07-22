# Unbounded Knapsack

## Introduction

Like 0/1 Knapsack, but each item can be used **unlimited** times. The only change in the DP is traversal direction: left to right instead of right to left.

---

## Intuition

In 0/1 Knapsack, when filling `dp[w]`, we look at `dp[w - wt[i]]` from the **previous item's row** — ensuring item `i` is used at most once. In Unbounded Knapsack, we allow reuse: `dp[w - wt[i]]` from the **current row** — which may already include item `i`. Left-to-right traversal in 1D DP achieves exactly this: `dp[w]` can use an item multiple times.

---

## When to Use

- Coin Change (minimum coins to make amount).
- Rod Cutting.
- Any "items with unlimited supply" knapsack variant.

---

## Complexity Analysis

| Case    | Time   | Space |
|---------|--------|-------|
| Best    | O(n·W) | O(W)  |
| Average | O(n·W) | O(W)  |
| Worst   | O(n·W) | O(W)  |

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Unbounded knapsack — max value, unlimited items
int unboundedKnapsack(vector<int>& wt, vector<int>& val, int W) {
    int n = wt.size();
    vector<int> dp(W + 1, 0);

    for (int i = 0; i < n; i++)
        for (int w = wt[i]; w <= W; w++)      // LEFT TO RIGHT — allows reuse
            dp[w] = max(dp[w], dp[w - wt[i]] + val[i]);

    return dp[W];
}

// Coin Change — minimum coins to make amount
int coinChange(vector<int>& coins, int amount) {
    vector<int> dp(amount + 1, INT_MAX);
    dp[0] = 0;

    for (int coin : coins)
        for (int w = coin; w <= amount; w++)
            if (dp[w - coin] != INT_MAX)
                dp[w] = min(dp[w], dp[w - coin] + 1);

    return dp[amount] == INT_MAX ? -1 : dp[amount];
}

// Coin Change 2 — number of ways to make amount
int coinChangeWays(vector<int>& coins, int amount) {
    vector<long long> dp(amount + 1, 0);
    dp[0] = 1;

    for (int coin : coins)
        for (int w = coin; w <= amount; w++)
            dp[w] += dp[w - coin];

    return dp[amount];
}
```

---

## Important Notes

- **Left to right = unbounded** (item can be used multiple times in the current solution).
- **Right to left = 0/1** (item used at most once — current row references previous item's state).
- Coin Change minimum: initialize `dp[0] = 0` and all others to `INT_MAX`. Guard `dp[w-coin] != INT_MAX` prevents overflow.
- Coin Change count (ways): initialize `dp[0] = 1` (1 way to make amount 0: use nothing). The order of loops matters here — outer loop over coins, inner over amount, avoids counting permutations as distinct combinations.
