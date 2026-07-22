# Monotonic Stack

## Purpose

> Maintains a stack of elements in strictly increasing or decreasing order to answer Next/Previous Greater/Smaller queries in O(n).

---

## When to Use

- Next Greater Element (NGE)
- Previous Greater Element (PGE)
- Next Smaller Element (NSE)
- Previous Smaller Element (PSE)
- Largest Rectangle in Histogram
- Stock Span Problem
- Trapping Rain Water
- Sum of Subarray Minimums / Maximums

---

## Time Complexity

| Case    | Complexity |
|---------|------------|
| Best    | O(n)       |
| Average | O(n)       |
| Worst   | O(n)       |

**Space Complexity:** O(n) — stack can hold at most n elements.

---

## Core Idea

- Traverse the array left to right (or right to left depending on query).
- Maintain a stack of indices (not values).
- Before pushing the current index, pop all elements that violate the monotonic property.
- Every element is pushed and popped at most once → O(n) total.
- The stack stores candidates; popping resolves their answer.

---

## Critical Code Walkthrough

### Next Greater Element

```cpp
vector<int> nextGreater(vector<int>& nums) {
    int n = nums.size();
    vector<int> res(n, -1);
    stack<int> st;  // stores indices

    for (int i = 0; i < n; i++) {
        while (!st.empty() && nums[st.top()] < nums[i]) {
            res[st.top()] = nums[i];  // nums[i] is the next greater for st.top()
            st.pop();
        }
        st.push(i);
    }
    return res;
}
```

**Why indices, not values?** — Indices let you write the answer into `res[index]` and also compute widths (needed in histogram problems).

**Why pop when `nums[st.top()] < nums[i]`?** — `nums[i]` is the first element greater than the element at `st.top()`, so the answer for `st.top()` is resolved.

---

### Previous Smaller Element

```cpp
vector<int> prevSmaller(vector<int>& nums) {
    int n = nums.size();
    vector<int> res(n, -1);
    stack<int> st;

    for (int i = 0; i < n; i++) {
        while (!st.empty() && nums[st.top()] >= nums[i])
            st.pop();
        res[i] = st.empty() ? -1 : st.top();
        st.push(i);
    }
    return res;
}
```

**Traverse left to right.** Stack top is always the most recent smaller element seen so far.

---

### Variant Summary

| Query              | Traverse | Pop Condition         | Answer Timing |
|--------------------|----------|-----------------------|---------------|
| Next Greater       | L → R    | `st.top() < nums[i]`  | While popping |
| Next Smaller       | L → R    | `st.top() > nums[i]`  | While popping |
| Previous Greater   | L → R    | `st.top() <= nums[i]` | Before push   |
| Previous Smaller   | L → R    | `st.top() >= nums[i]` | Before push   |

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Next Greater Element
vector<int> nextGreater(vector<int>& nums) {
    int n = nums.size();
    vector<int> res(n, -1);
    stack<int> st;
    for (int i = 0; i < n; i++) {
        while (!st.empty() && nums[st.top()] < nums[i]) {
            res[st.top()] = nums[i];
            st.pop();
        }
        st.push(i);
    }
    return res;
}

// Previous Smaller Element
vector<int> prevSmaller(vector<int>& nums) {
    int n = nums.size();
    vector<int> res(n, -1);
    stack<int> st;
    for (int i = 0; i < n; i++) {
        while (!st.empty() && nums[st.top()] >= nums[i])
            st.pop();
        res[i] = st.empty() ? -1 : nums[st.top()];
        st.push(i);
    }
    return res;
}

// Next Greater in circular array
vector<int> nextGreaterCircular(vector<int>& nums) {
    int n = nums.size();
    vector<int> res(n, -1);
    stack<int> st;
    for (int i = 2 * n - 1; i >= 0; i--) {
        while (!st.empty() && nums[st.top() % n] <= nums[i % n])
            st.pop();
        if (i < n)
            res[i] = st.empty() ? -1 : nums[st.top() % n];
        st.push(i % n);
    }
    return res;
}
```

---

## Notes

- Store **indices**, not values — needed for distance calculations and writing results.
- Decreasing monotonic stack → answers NGE / NSE queries.
- Increasing monotonic stack → answers PSE / PGE queries.
- For circular arrays, iterate 2*n and use modulo.
- All NGE/NSE/PGE/PSE problems reduce to this pattern.
- Stock Span = distance to previous greater = monotonic stack traversing left to right.
