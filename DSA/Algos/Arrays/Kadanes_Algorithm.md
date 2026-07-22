# Kadane's Algorithm

## Introduction

Finds the maximum sum contiguous subarray in O(n). The key insight is that a subarray's maximum sum can either extend the previous maximum subarray or start fresh at the current element.

---

## Intuition

At each index `i`, you have two choices: extend the current subarray (add `nums[i]` to the running sum) or start a new subarray at `nums[i]`. The optimal choice is whichever is larger. If the running sum so far is negative, it only hurts to include it — start fresh. If it's positive, including it can only help. This greedy decision at each step leads to the globally optimal answer.

---

## When to Use

- Maximum sum subarray.
- Maximum product subarray (modified version).
- Circular maximum sum subarray.
- Any problem where you track a running aggregate and reset when it becomes harmful.

---

## Recognition Pattern

```
"Maximum sum subarray"
"Contiguous subarray"
"Running sum that resets"
"Maximum sum / minimum sum of contiguous elements"
```
→ Think Kadane's.

---

## Complexity Analysis

| Case    | Time | Space |
|---------|------|-------|
| Best    | O(n) | O(1)  |
| Average | O(n) | O(1)  |
| Worst   | O(n) | O(1)  |

---

## Core Idea

Maintain `cur` = best sum of a subarray ending at the current index. At each index: `cur = max(nums[i], cur + nums[i])`. Track `maxSum = max(maxSum, cur)`. Initialize both with `nums[0]`.

---

## Visualization

```
nums = [-2, 1, -3, 4, -1, 2, 1, -5, 4]

i=0: cur=-2,       maxSum=-2
i=1: cur=max(1,-2+1)=max(1,-1)=1,   maxSum=1
i=2: cur=max(-3,1-3)=max(-3,-2)=-2, maxSum=1
i=3: cur=max(4,-2+4)=max(4,2)=4,    maxSum=4
i=4: cur=max(-1,4-1)=max(-1,3)=3,   maxSum=4
i=5: cur=max(2,3+2)=max(2,5)=5,     maxSum=5
i=6: cur=max(1,5+1)=max(1,6)=6,     maxSum=6
i=7: cur=max(-5,6-5)=max(-5,1)=1,   maxSum=6
i=8: cur=max(4,1+4)=max(4,5)=5,     maxSum=6

Answer: 6 (subarray [4,-1,2,1])
```

---

## Critical Code Explanation

### The Core Transition

```cpp
cur = max(nums[i], cur + nums[i]);
```

This is the entire algorithm's logic. If `cur + nums[i] < nums[i]`, then `cur` is negative — the running sum is a liability. Discard the previous subarray and start fresh at `nums[i]`. If `cur` is non-negative, extend it.

### All-Negative Arrays

Initialize `maxSum = nums[0]` (not `INT_MIN` or `0`). If all elements are negative, the answer is the least-negative single element — the algorithm handles this correctly because it always considers starting a new subarray at each element.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Maximum sum subarray — returns the sum
int kadane(vector<int>& nums) {
    int cur = nums[0], maxSum = nums[0];
    for (int i = 1; i < (int)nums.size(); i++) {
        cur = max(nums[i], cur + nums[i]);
        maxSum = max(maxSum, cur);
    }
    return maxSum;
}

// With subarray indices
pair<int, pair<int,int>> kadaneWithIndices(vector<int>& nums) {
    int cur = nums[0], maxSum = nums[0];
    int start = 0, end = 0, tempStart = 0;

    for (int i = 1; i < (int)nums.size(); i++) {
        if (nums[i] > cur + nums[i]) {
            cur = nums[i];
            tempStart = i;
        } else {
            cur += nums[i];
        }
        if (cur > maxSum) {
            maxSum = cur;
            start = tempStart;
            end = i;
        }
    }
    return {maxSum, {start, end}};
}

// Circular maximum sum subarray
int kadaneCircular(vector<int>& nums) {
    int n = nums.size();
    // Case 1: max subarray doesn't wrap → standard Kadane
    int maxNormal = kadane(nums);

    // Case 2: max subarray wraps → total sum - minimum subarray
    int totalSum = 0, curMin = nums[0], minSum = nums[0];
    for (int i = 0; i < n; i++) {
        totalSum += nums[i];
        if (i > 0) {
            curMin = min(nums[i], curMin + nums[i]);
            minSum = min(minSum, curMin);
        }
    }

    // If all elements are negative, maxNormal handles it
    if (totalSum == minSum) return maxNormal;
    return max(maxNormal, totalSum - minSum);
}
```

---

## Why It Works

The invariant: `cur` is the maximum sum of any subarray ending exactly at index `i`. This is correct because the best subarray ending at `i` is either just `nums[i]` alone, or `nums[i]` appended to the best subarray ending at `i-1`. Taking the maximum of these two choices at every step, and tracking the global best in `maxSum`, guarantees the global optimum.

---

## Important Notes

- **Do not initialize `maxSum = 0`** — this gives wrong answers when all elements are negative (the empty subarray has sum 0, but the problem asks for at least one element).
- The circular variant's insight: a wrapping subarray = total sum - some non-wrapping subarray. Minimize that non-wrapping subarray = maximum wrapping subarray.
- Kadane's is essentially 1D DP — `cur` is the DP state.
- For **maximum product subarray**, maintain both `curMax` and `curMin` at each step because a negative number flips max and min.
