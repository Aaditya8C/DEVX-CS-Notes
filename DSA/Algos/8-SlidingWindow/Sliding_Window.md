# Sliding Window

## Introduction

Maintains a contiguous subarray of variable or fixed size by sliding a window across the array, adding from the right and removing from the left. Converts O(n²) brute-force subarray problems to O(n).

---

## Intuition

Instead of recomputing from scratch for every subarray, maintain the result incrementally. When the window slides one position right, the only changes are: the new element enters from the right, and one element may leave from the left. If the window invariant is violated (size exceeded, constraint broken), shrink from the left until it's restored.

---

## When to Use

- Maximum/minimum sum/length of subarray with constraint.
- Longest subarray with at most K distinct elements.
- Minimum window substring containing all required characters.
- Fixed-size window: maximum sum of k consecutive elements.
- Any problem: "longest/shortest contiguous subarray satisfying condition X".

---

## Recognition Pattern

```
"Subarray" or "substring" (contiguous)
+ "longest" / "shortest" / "maximum" / "minimum"
+ constraint that can be checked incrementally
+ "at most K" / "exactly K" / "sum ≤ X"
```
→ Think Sliding Window.

---

## Complexity Analysis

| Case    | Time | Space |
|---------|------|-------|
| Best    | O(n) | O(1) or O(k) |
| Average | O(n) | O(1) or O(k) |
| Worst   | O(n) | O(1) or O(k) |

Both pointers together traverse the array at most once each → O(n) total.

---

## Core Idea

Two pointers `lo` and `hi` define the current window `[lo, hi]`. Advance `hi` to include the new element. While the window violates the constraint, advance `lo` to shrink from the left. At each valid window, update the answer. Both pointers only move forward → O(n).

---

## Visualization

```
nums = [2, 3, 1, 2, 4, 3],  target sum ≤ 7, find shortest subarray with sum ≥ 7

lo=0, hi=-1, sum=0

hi=0: sum=2, sum<7, expand
hi=1: sum=5, sum<7, expand
hi=2: sum=6, sum<7, expand
hi=3: sum=8 ≥ 7 → ans=4, shrink: lo=1, sum=6
hi=4: sum=10 ≥ 7 → ans=4, shrink: lo=2, sum=7 ≥ 7 → ans=3, shrink: lo=3, sum=6
hi=5: sum=9 ≥ 7 → ans=3, shrink: lo=4, sum=7 ≥ 7 → ans=2, shrink: lo=5, sum=3

Answer: 2 (subarray [4,3])
```

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Fixed window: max sum of exactly k elements
int maxSumFixed(vector<int>& arr, int k) {
    int n = arr.size();
    int windowSum = 0;
    for (int i = 0; i < k; i++) windowSum += arr[i];

    int maxSum = windowSum;
    for (int i = k; i < n; i++) {
        windowSum += arr[i] - arr[i - k];   // slide: add right, remove left
        maxSum = max(maxSum, windowSum);
    }
    return maxSum;
}

// Variable window: longest subarray with sum ≤ target
int longestSubarraySum(vector<int>& arr, int target) {
    int lo = 0, sum = 0, maxLen = 0;
    for (int hi = 0; hi < (int)arr.size(); hi++) {
        sum += arr[hi];
        while (sum > target) sum -= arr[lo++];   // shrink until valid
        maxLen = max(maxLen, hi - lo + 1);
    }
    return maxLen;
}

// Variable window: longest substring with at most K distinct characters
int longestKDistinct(string s, int k) {
    unordered_map<char, int> freq;
    int lo = 0, maxLen = 0;
    for (int hi = 0; hi < (int)s.size(); hi++) {
        freq[s[hi]]++;
        while ((int)freq.size() > k) {
            freq[s[lo]]--;
            if (freq[s[lo]] == 0) freq.erase(s[lo]);
            lo++;
        }
        maxLen = max(maxLen, hi - lo + 1);
    }
    return maxLen;
}

// Minimum window substring containing all chars of t
string minWindow(string s, string t) {
    unordered_map<char, int> need, have;
    for (char c : t) need[c]++;
    int formed = 0, required = need.size();
    int lo = 0, minLen = INT_MAX, minStart = 0;

    for (int hi = 0; hi < (int)s.size(); hi++) {
        char c = s[hi];
        have[c]++;
        if (need.count(c) && have[c] == need[c]) formed++;

        while (formed == required) {
            if (hi - lo + 1 < minLen) { minLen = hi - lo + 1; minStart = lo; }
            char lc = s[lo++];
            have[lc]--;
            if (need.count(lc) && have[lc] < need[lc]) formed--;
        }
    }
    return minLen == INT_MAX ? "" : s.substr(minStart, minLen);
}
```

---

## Why It Works

Each element is added to the window exactly once (when `hi` passes it) and removed at most once (when `lo` passes it). The total work across all iterations of both loops is O(n). Correctness follows from the monotone property: if the window `[lo, hi]` violates the constraint, making it longer by advancing `hi` will only make it worse, so advancing `lo` is the only option.

---

## Important Notes

- **Shrink condition is a `while`, not an `if`** — after removing the leftmost element, the window may still violate the constraint, so you must keep shrinking.
- For "exactly K" problems, use `atMostK(k) - atMostK(k-1)` — this converts an exact-count constraint into two sliding window calls.
- The window validity check must be **efficient** — if shrinking the window restores validity monotonically, sliding window works. If not (e.g., contains negative numbers in a sum constraint), sliding window may fail; use prefix sums + binary search or deques instead.
- Monotonic deque (sliding window maximum): combine with a `deque` to get the maximum/minimum within the window in O(1) per step.
