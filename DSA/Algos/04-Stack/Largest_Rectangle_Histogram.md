# Largest Rectangle in Histogram

## Introduction

Given an array of bar heights in a histogram, find the area of the largest rectangle that fits entirely within the bars. The trick is recognizing that each bar is the height-limiting bar for some contiguous range — and finding that range for every bar in O(n) using a monotonic stack.

---

## Intuition

Fix a bar as the shortest bar in the rectangle. The rectangle can expand left and right as long as every bar in that range is at least as tall as the fixed bar. The left boundary is the first bar to the left that is shorter (Previous Smaller Element), and the right boundary is the first bar to the right that is shorter (Next Smaller Element). The width is the gap between these two boundaries. Computing PSE and NSE for every bar is a monotonic stack problem — and that's the entire algorithm.

---

## When to Use

- Largest Rectangle in Histogram (the classic)
- Maximal Rectangle in a Binary Matrix (2D extension)
- Any "maximum area under a height profile" problem

---

## Recognition Pattern

```
"Heights array"
"Maximum area of rectangle"
"Bars / buildings / columns"
"Binary matrix → maximal rectangle of 1s"
```
→ Think Histogram + Monotonic Stack (PSE + NSE).

---

## Complexity Analysis

| Case    | Time | Space |
|---------|------|-------|
| Best    | O(n) | O(n)  |
| Average | O(n) | O(n)  |
| Worst   | O(n) | O(n)  |

Both the two-pass and single-pass approaches are O(n) time and O(n) space (stack).

---

## Core Idea

For every bar `i`, it can be the shortest bar in a rectangle that spans from `PSE[i]+1` to `NSE[i]-1`, giving width `NSE[i] - PSE[i] - 1` and area `heights[i] * width`. Compute PSE (Previous Smaller Element index) and NSE (Next Smaller Element index) for all bars using a monotonic stack in two passes. The answer is the maximum area across all bars.

The single-pass variant avoids computing PSE and NSE explicitly — instead, when bar `j` is popped from the stack (because current bar `i` is smaller), `i` is its NSE and the new stack top is its PSE, so the area can be computed immediately on pop.

---

## Visualization

```
heights = [2, 1, 5, 6, 2, 3]
indices =  0  1  2  3  4  5

PSE:      [-1,-1, 1, 2, 1, 4]   (index of previous smaller)
NSE:      [ 1, 6, 4, 4, 6, 6]   (index of next smaller; 6=n means none)

Bar 0: height=2, width = 1-(-1)-1 = 1,  area = 2
Bar 1: height=1, width = 6-(-1)-1 = 6,  area = 6   ← covers entire histogram
Bar 2: height=5, width = 4-1-1   = 2,   area = 10
Bar 3: height=6, width = 4-2-1   = 1,   area = 6
Bar 4: height=2, width = 6-1-1   = 4,   area = 8
Bar 5: height=3, width = 6-4-1   = 1,   area = 3

Maximum area = 10
```

---

## Critical Code Explanation

### PSE and NSE Boundary Defaults

```cpp
pse[i] = st.empty() ? -1 : st.top();   // no smaller on left → sentinel -1
nse[i] = st.empty() ? n  : st.top();   // no smaller on right → sentinel n
```

Using `-1` and `n` as sentinels lets the width formula `NSE[i] - PSE[i] - 1` work uniformly without special-casing edge bars. A bar with no left boundary gets `PSE = -1`, so its width extends to index `0`.

### Single-Pass Width Computation

```cpp
while (!st.empty() && heights[st.top()] > heights[i]) {
    int h = heights[st.top()]; st.pop();
    int w = st.empty() ? i : i - st.top() - 1;   // left boundary is new top
    maxArea = max(maxArea, h * w);
}
```

After popping `j`, the new stack top `st.top()` is `j`'s PSE (the first bar to the left that is smaller). The current index `i` is `j`'s NSE. So `w = i - st.top() - 1` computes the width in one shot without separate PSE/NSE arrays.

### The Sentinel 0 at the End

```cpp
heights.push_back(0);  // forces all remaining stack elements to be popped
```

After the main traversal, the stack may still hold bars whose NSE is "none" (they extend to the right edge). Appending a zero-height sentinel ensures they all get popped and their areas computed before the function returns.

### 2D Extension — Row-by-Row Histogram

```cpp
heights[c] = (matrix[r][c] == '1') ? heights[c] + 1 : 0;
```

For each row, treat the accumulated column heights as a histogram. A `'1'` increments the height (extending the bar upward), while a `'0'` resets it to zero (the bar is broken). Running the 1D histogram algorithm on each row's heights gives the maximal rectangle.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Two-pass: explicit PSE + NSE arrays
int largestRectangle(vector<int>& heights) {
    int n = heights.size();
    vector<int> pse(n), nse(n);
    stack<int> st;

    for (int i = 0; i < n; i++) {
        while (!st.empty() && heights[st.top()] >= heights[i])
            st.pop();
        pse[i] = st.empty() ? -1 : st.top();
        st.push(i);
    }

    while (!st.empty()) st.pop();

    for (int i = n - 1; i >= 0; i--) {
        while (!st.empty() && heights[st.top()] >= heights[i])
            st.pop();
        nse[i] = st.empty() ? n : st.top();
        st.push(i);
    }

    int maxArea = 0;
    for (int i = 0; i < n; i++)
        maxArea = max(maxArea, heights[i] * (nse[i] - pse[i] - 1));
    return maxArea;
}

// Single-pass with sentinel — preferred in interviews
int largestRectangleSinglePass(vector<int> heights) {
    heights.push_back(0);   // sentinel
    int n = heights.size();
    stack<int> st;
    int maxArea = 0;

    for (int i = 0; i < n; i++) {
        while (!st.empty() && heights[st.top()] > heights[i]) {
            int h = heights[st.top()]; st.pop();
            int w = st.empty() ? i : i - st.top() - 1;
            maxArea = max(maxArea, h * w);
        }
        st.push(i);
    }
    return maxArea;
}

// 2D extension: maximal rectangle in binary matrix
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

## Why It Works

For any bar `i` treated as the minimum-height bar, the rectangle extends exactly as far left and right as bars that are all at least as tall. The first shorter bar on either side defines the hard boundary. PSE and NSE identify these boundaries precisely. Since every bar `i` is the actual minimum in the range `(PSE[i], NSE[i])` — by definition of PSE and NSE — the computed area `heights[i] * width` is achievable and cannot be extended further. Taking the maximum over all bars exhausts all candidates, guaranteeing the global maximum is found.

---

## Important Notes

- Use `>=` (not `>`) in the PSE/NSE pop condition to correctly handle **duplicate heights**. With `>`, two equal bars would each claim the other as their boundary, potentially double-counting. With `>=`, the rightmost of equal bars gets the correct range.
- The single-pass approach passes `heights` **by value** or restores after appending the sentinel — failing to do so modifies the caller's array.
- The 2D maximal rectangle problem (LeetCode 85) is one of the hardest medium problems precisely because the reduction to histograms is non-obvious. Recognizing it collapses the problem.
- Trapping Rain Water is a related problem using the same PSE/NSE structure: `water[i] = min(maxLeft[i], maxRight[i]) - height[i]`.
- This is one of the cleanest examples of how a monotonic stack converts an O(n²) problem into O(n) — understanding it deeply helps with a family of similar problems.
