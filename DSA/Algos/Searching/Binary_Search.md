# Binary Search

## Introduction

Finds a target in a sorted array in O(log n) by repeatedly halving the search space. The most fundamental searching algorithm — also the foundation for lower/upper bound queries and binary search on answer.

---

## Intuition

In a sorted array, comparing with the middle element eliminates half the remaining search space. If target equals mid, found. If target is larger, it must be in the right half. If smaller, in the left half. Each step cuts the problem in half — O(log n) total.

---

## When to Use

- Searching in a sorted array.
- Finding insertion position (lower/upper bound).
- "Is it possible to achieve X?" — binary search on the answer when the feasibility check is monotone.
- Finding peak, rotation point, or any "first/last satisfying condition" in sorted/rotated arrays.

---

## Recognition Pattern

```
"Find target in sorted array"
"Minimum/maximum value satisfying a condition"
"The answer space is monotone" (feasible/infeasible has a clear boundary)
"First/last position of X"
```
→ Think Binary Search.

---

## Complexity Analysis

| Case    | Time    | Space |
|---------|---------|-------|
| Best    | O(1)    | O(1)  |
| Average | O(log n)| O(1)  |
| Worst   | O(log n)| O(1)  |

---

## Core Idea

Maintain `[lo, hi]` as the search range. At each step, compute `mid = lo + (hi-lo)/2`. If `arr[mid] == target`, return `mid`. If `arr[mid] < target`, search right: `lo = mid + 1`. If `arr[mid] > target`, search left: `hi = mid - 1`. Loop until `lo > hi`.

---

## Visualization

```
arr = [1, 3, 5, 7, 9, 11, 13]   target = 7

lo=0, hi=6 → mid=3 → arr[3]=7 == target → FOUND at index 3

arr = [1, 3, 5, 7, 9, 11, 13]   target = 6

lo=0, hi=6 → mid=3 → arr[3]=7 > 6 → hi=2
lo=0, hi=2 → mid=1 → arr[1]=3 < 6 → lo=2
lo=2, hi=2 → mid=2 → arr[2]=5 < 6 → lo=3
lo=3 > hi=2 → NOT FOUND, return -1
```

---

## Critical Code Explanation

### Overflow-Safe Mid Calculation

```cpp
int mid = lo + (hi - lo) / 2;   // correct
// NOT: int mid = (lo + hi) / 2; // overflows when lo+hi > INT_MAX
```

`lo + (hi - lo) / 2` is mathematically equivalent but never overflows since `hi - lo` is always non-negative and at most n-1.

### Loop Condition: `lo <= hi` vs `lo < hi`

```cpp
while (lo <= hi) { ... }   // for exact match search
while (lo < hi)  { ... }   // for finding a boundary (lower/upper bound)
```

`lo <= hi` — processes the element when `lo == hi`, necessary for exact match. `lo < hi` — terminates with `lo == hi` pointing to the answer boundary, used in lower/upper bound patterns.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Exact match — returns index or -1
int binarySearch(vector<int>& arr, int target) {
    int lo = 0, hi = arr.size() - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid] == target) return mid;
        else if (arr[mid] < target) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

// Lower bound — first index where arr[i] >= target
int lowerBound(vector<int>& arr, int target) {
    int lo = 0, hi = arr.size();
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid] < target) lo = mid + 1;
        else hi = mid;
    }
    return lo;   // arr.size() if target > all elements
}

// Upper bound — first index where arr[i] > target
int upperBound(vector<int>& arr, int target) {
    int lo = 0, hi = arr.size();
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid] <= target) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}

// Binary Search on Answer template
// pred(x) is true for all x >= answer, false for all x < answer
int bsOnAnswer(int lo, int hi, auto pred) {
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (pred(mid)) hi = mid;
        else lo = mid + 1;
    }
    return lo;
}
```

---

## Why It Works

In a sorted array, the comparison with `arr[mid]` is deterministic: it rules out exactly half of the remaining elements. After k comparisons, the search space is n/2^k. When n/2^k < 1, i.e., k > log₂n, the element is either found or confirmed absent.

---

## Important Notes

- Always use `lo + (hi - lo) / 2` to compute mid — `(lo + hi) / 2` overflows for large indices.
- STL provides `lower_bound` and `upper_bound` — prefer them over manual implementation in production code.
- **Binary search on answer**: applies when you can efficiently check "is X achievable?" and the answer has a monotone structure (all feasible values on one side, all infeasible on the other).
- For finding the last occurrence of a target: find upper bound and check `arr[upperBound - 1]`.
- Binary search works on any **monotone predicate**, not just sorted arrays — e.g., finding the peak of a unimodal function, first bad version, etc.
