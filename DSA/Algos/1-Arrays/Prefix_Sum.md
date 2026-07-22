# Prefix Sum & Difference Array

## Introduction

Prefix Sum enables O(1) range sum queries after O(n) preprocessing. Difference Array enables O(1) range update operations with O(n) final reconstruction. They are dual techniques — one optimizes queries, the other optimizes updates.

---

## Intuition

**Prefix Sum:** If you precompute `prefix[i] = sum of arr[0..i]`, then any range sum `arr[l..r] = prefix[r] - prefix[l-1]` in O(1). You pay O(n) once to answer all future queries in O(1).

**Difference Array:** Instead of updating every element in a range (O(n) per update), record only the start and end of the update in a difference array. After all updates, reconstruct the final array with a single prefix-sum pass.

---

## When to Use

**Prefix Sum:**
- Multiple range sum queries on a static array.
- Subarray sum equals target (use `prefix[j] - prefix[i] == target → find prefix[j] - target in a hashmap`).
- 2D prefix sums for rectangle sum queries.

**Difference Array:**
- Multiple range increment/decrement operations followed by a single read.
- "Add v to all elements in [l, r]" repeated many times.

---

## Recognition Pattern

```
Prefix Sum:
"Sum of subarray [l, r]"
"Multiple range queries, static array"
"Subarray sum equals K"
→ Prefix Sum

Difference Array:
"Add value to range [l, r]" repeated
"Range updates, then query each element once"
→ Difference Array
```

---

## Complexity Analysis

### Prefix Sum

| Operation       | Time | Space |
|-----------------|------|-------|
| Build           | O(n) | O(n)  |
| Range query     | O(1) | —     |

### Difference Array

| Operation       | Time | Space |
|-----------------|------|-------|
| Build           | O(n) | O(n)  |
| Range update    | O(1) | —     |
| Reconstruct     | O(n) | —     |

---

## Visualization

### Prefix Sum

```
arr    = [3, 1, 4, 1, 5, 9, 2]
prefix = [0, 3, 4, 8, 9, 14, 23, 25]
          ↑ (prefix[0]=0 as sentinel)

Sum(2,5) = prefix[6] - prefix[2] = 23 - 4 = 19
           (arr[2]+arr[3]+arr[4]+arr[5] = 4+1+5+9 = 19 ✓)
```

### Difference Array

```
arr = [0, 0, 0, 0, 0]
Updates: add 3 to [1,3], add 2 to [2,4]

diff = [0, 0, 0, 0, 0, 0]  ← size n+1

add 3 to [1,3]: diff[1]+=3, diff[4]-=3 → diff=[0,3,0,0,-3,0]
add 2 to [2,4]: diff[2]+=2, diff[5]-=2 → diff=[0,3,2,0,-3,-2]

Prefix sum of diff:
result = [0, 3, 5, 5, 2, 0]
           ↑ skip index 0, result = [3,5,5,2]
```

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// ── Prefix Sum ────────────────────────────────────────────────────────────────

struct PrefixSum {
    vector<long long> pre;

    PrefixSum(vector<int>& arr) {
        int n = arr.size();
        pre.resize(n + 1, 0);
        for (int i = 0; i < n; i++)
            pre[i + 1] = pre[i] + arr[i];
    }

    long long query(int l, int r) {   // [l, r] 0-indexed
        return pre[r + 1] - pre[l];
    }
};

// Subarray sum equals K — count of subarrays
int subarraySumK(vector<int>& nums, int k) {
    unordered_map<long long, int> freq;
    freq[0] = 1;
    long long prefSum = 0;
    int count = 0;

    for (int x : nums) {
        prefSum += x;
        count += freq[prefSum - k];   // subarrays ending here with sum k
        freq[prefSum]++;
    }
    return count;
}

// ── Difference Array ───────────────────────────────────────────────────────────

struct DifferenceArray {
    vector<long long> diff;
    int n;

    DifferenceArray(int n) : n(n), diff(n + 1, 0) {}

    void update(int l, int r, long long val) {   // add val to arr[l..r]
        diff[l] += val;
        diff[r + 1] -= val;
    }

    vector<long long> build() {   // reconstruct final array
        vector<long long> result(n);
        result[0] = diff[0];
        for (int i = 1; i < n; i++)
            result[i] = result[i - 1] + diff[i];
        return result;
    }
};
```

---

## Why It Works

**Prefix Sum:** `prefix[r+1] - prefix[l]` cancels out all elements before index `l`, leaving exactly the sum of `arr[l..r]`.

**Difference Array:** Incrementing `diff[l]` and decrementing `diff[r+1]` means that when prefix sum is taken, the `val` is added to all positions `l..r` and canceled afterward. This is the dual of prefix sum — update in O(1), query via reconstruction.

---

## Important Notes

- Use `prefix[0] = 0` as a sentinel to handle `l = 0` uniformly: `query(0, r) = prefix[r+1] - prefix[0]` without special-casing.
- For the subarray-sum-equals-K problem, the hashmap stores prefix sums seen so far. `prefSum - k` was seen earlier means there exists a subarray ending here with sum k.
- **2D Prefix Sum:** `prefix[i][j] = sum of rectangle (0,0) to (i,j)`. Query `(r1,c1,r2,c2) = prefix[r2][c2] - prefix[r1-1][c2] - prefix[r2][c1-1] + prefix[r1-1][c1-1]` (inclusion-exclusion).
- Difference array: the sentinel at `diff[r+1]` ensures the update doesn't affect positions beyond `r`. Size the diff array as `n+1` to safely write to `diff[n]`.
