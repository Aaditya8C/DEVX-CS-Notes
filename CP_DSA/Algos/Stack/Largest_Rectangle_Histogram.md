# Largest Rectangle in Histogram

## Purpose

> Finds the area of the largest rectangle that can be formed using contiguous bars in a histogram.

---

## When to Use

- Largest rectangle in histogram (classic)
- Maximal rectangle in a binary matrix (2D extension)
- Any problem involving "maximum area bounded by heights"

---

## Time Complexity

| Case    | Complexity |
|---------|------------|
| Best    | O(n)       |
| Average | O(n)       |
| Worst   | O(n)       |

**Space Complexity:** O(n) — stack + PSE/NSE arrays.

---

## Core Idea

- For each bar, find the **Previous Smaller Element (PSE)** and **Next Smaller Element (NSE)** index.
- Width of rectangle with bar `i` as the shortest bar = `NSE[i] - PSE[i] - 1`.
- Area = `heights[i] * width`.
- Maximum across all bars is the answer.
- PSE and NSE are computed using a monotonic stack in O(n).

---

## Critical Code Walkthrough

### PSE and NSE Arrays

```cpp
// PSE[i] = index of previous bar strictly smaller than heights[i]
// NSE[i] = index of next bar strictly smaller than heights[i]
```

Boundaries: PSE defaults to `-1` (no smaller on left), NSE defaults to `n` (no smaller on right).

```cpp
int width = nse[i] - pse[i] - 1;
int area  = heights[i] * width;
```

The bar at index `i` is the minimum in the range `(pse[i], nse[i])`, so it can extend the full width.

---

### Single-Pass Stack Solution

```cpp
// Push a sentinel 0 at the end to flush remaining stack elements
heights.push_back(0);
```

When the sentinel is processed, all remaining bars in the stack are popped and their areas are computed.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Two-pass: PSE + NSE
int largestRectangle(vector<int>& heights) {
    int n = heights.size();
    vector<int> pse(n), nse(n);
    stack<int> st;

    // Previous Smaller Element
    for (int i = 0; i < n; i++) {
        while (!st.empty() && heights[st.top()] >= heights[i])
            st.pop();
        pse[i] = st.empty() ? -1 : st.top();
        st.push(i);
    }

    while (!st.empty()) st.pop();

    // Next Smaller Element
    for (int i = n - 1; i >= 0; i--) {
        while (!st.empty() && heights[st.top()] >= heights[i])
            st.pop();
        nse[i] = st.empty() ? n : st.top();
        st.push(i);
    }

    int maxArea = 0;
    for (int i = 0; i < n; i++) {
        int width = nse[i] - pse[i] - 1;
        maxArea = max(maxArea, heights[i] * width);
    }
    return maxArea;
}

// Single-pass with sentinel
int largestRectangleSinglePass(vector<int>& heights) {
    heights.push_back(0);  // sentinel to flush stack
    int n = heights.size();
    stack<int> st;
    int maxArea = 0;

    for (int i = 0; i < n; i++) {
        while (!st.empty() && heights[st.top()] > heights[i]) {
            int h = heights[st.top()];
            st.pop();
            int w = st.empty() ? i : i - st.top() - 1;
            maxArea = max(maxArea, h * w);
        }
        st.push(i);
    }

    heights.pop_back();  // restore original
    return maxArea;
}

// 2D Extension: Maximal Rectangle in Binary Matrix
int maximalRectangle(vector<vector<char>>& matrix) {
    if (matrix.empty()) return 0;
    int rows = matrix.size(), cols = matrix[0].size();
    vector<int> heights(cols, 0);
    int maxArea = 0;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++)
            heights[c] = (matrix[r][c] == '1') ? heights[c] + 1 : 0;
        maxArea = max(maxArea, largestRectangleSinglePass(heights));
    }
    return maxArea;
}
```

---

## Notes

- Treat `>=` vs `>` carefully in PSE/NSE for duplicate heights — both approaches work, just be consistent.
- Single-pass is more interview-friendly; two-pass is easier to reason about.
- The 2D maximal rectangle problem reduces to running the 1D histogram problem on each row.
- Width computation in single-pass: `i - st.top() - 1` uses the new stack top after popping as the left boundary.
