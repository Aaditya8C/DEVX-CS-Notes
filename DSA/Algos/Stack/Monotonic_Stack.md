# Monotonic Stack

## Introduction

A stack maintained in strictly increasing or decreasing order that resolves Next/Previous Greater/Smaller queries for every element in O(n) total — the key insight being that each element is pushed and popped at most once.

---

## Intuition

Whenever you see a problem asking "for each element, find the nearest element to the left/right that is greater/smaller", the naive O(n²) approach is to scan in both directions for each element. The observation that breaks this down to O(n): you never need to keep elements in the stack that have already been "beaten" by the current element. If the current element is larger than the stack top, the stack top's "next greater" answer is the current element — resolve it immediately and discard it. Everything left in the stack at the end has no answer.

---

## When to Use

- Next Greater Element / Previous Greater Element
- Next Smaller Element / Previous Smaller Element
- Stock Span Problem
- Largest Rectangle in Histogram
- Trapping Rain Water
- Sum of Subarray Minimums/Maximums
- Any problem asking "for each element, find the nearest X in some direction"

---

## Recognition Pattern

```
"For each element, find the nearest element that is greater/smaller"
"Stock span"
"Subarray min/max sum"
"Trapping rain water"
"Histogram rectangle"
```
→ Think Monotonic Stack.

---

## Complexity Analysis

| Case    | Time | Space |
|---------|------|-------|
| Best    | O(n) | O(n)  |
| Average | O(n) | O(n)  |
| Worst   | O(n) | O(n)  |

Each element enters and exits the stack exactly once across the entire traversal, giving amortized O(1) per element.

---

## Core Idea

Traverse left to right. Maintain a stack of indices. Before pushing index `i`, pop every index `j` from the stack where `nums[j]` violates the monotonic property relative to `nums[i]`. When you pop `j`, the current element `i` is the answer for `j` (it's the first element in the scan direction that beats `j`). Push `i` after resolving all pops. Elements remaining in the stack after the full traversal have no answer (-1 or n depending on the query).

---

## Visualization

```
nums = [2, 1, 5, 6, 2, 3]   → Find Next Greater Element

i=0, push 0          stack: [0]          (nums: [2])
i=1, 1 < 2, push 1   stack: [0,1]        (nums: [2,1])
i=2, 5 > 1 → pop 1, NGE[1]=5
     5 > 2 → pop 0, NGE[0]=5
     push 2           stack: [2]          (nums: [5])
i=3, 6 > 5 → pop 2, NGE[2]=6
     push 3           stack: [3]          (nums: [6])
i=4, 2 < 6, push 4   stack: [3,4]        (nums: [6,2])
i=5, 3 > 2 → pop 4, NGE[4]=3
     3 < 6, push 5   stack: [3,5]        (nums: [6,3])

Remaining stack [3,5] → NGE[3]=-1, NGE[5]=-1

Result: [5, 5, 6, -1, 3, -1]
```

---

## Critical Code Explanation

### Store Indices, Not Values

```cpp
stack<int> st;  // stores indices, not nums[i]

while (!st.empty() && nums[st.top()] < nums[i]) {
    res[st.top()] = nums[i];
    st.pop();
}
st.push(i);
```

Storing indices instead of values lets you write directly into `res[index]` and also enables width computations (needed in histogram and rain water problems). If you stored values, you'd lose track of which position to update.

### The Pop Condition Determines the Query

```
NGE: pop when stack top < current   → decreasing stack
NSE: pop when stack top > current   → increasing stack
PGE: stack top is answer when top > current (read before push, no pop on equal)
PSE: stack top is answer when top < current (read before push)
```

Changing the comparison operator flips between the four variants. Every monotonic stack problem is just one of these four patterns.

### Variant Reference Table

| Query            | Pop When              | Answer Source    | Stack Order  |
|------------------|-----------------------|------------------|--------------|
| Next Greater     | `top < nums[i]`       | `nums[i]` on pop | Decreasing   |
| Next Smaller     | `top > nums[i]`       | `nums[i]` on pop | Increasing   |
| Prev Greater     | `top <= nums[i]`      | `top` before push| Decreasing   |
| Prev Smaller     | `top >= nums[i]`      | `top` before push| Increasing   |

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Next Greater Element — O(n)
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

// Previous Smaller Element — O(n)
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

// Next Greater in Circular Array — O(n)
vector<int> nextGreaterCircular(vector<int>& nums) {
    int n = nums.size();
    vector<int> res(n, -1);
    stack<int> st;

    // Traverse 2n to simulate circular wrap
    for (int i = 2 * n - 1; i >= 0; i--) {
        while (!st.empty() && nums[st.top()] <= nums[i % n])
            st.pop();
        if (i < n)
            res[i] = st.empty() ? -1 : nums[st.top()];
        st.push(i % n);
    }
    return res;
}
```

---

## Why It Works

The amortized O(n) bound holds because each index is pushed exactly once and popped at most once. The total number of push + pop operations is at most 2n regardless of input. The correctness follows from the invariant that the stack always holds indices in monotone order — when element `i` pops element `j`, it is the first element to the right of `j` that violates the monotone property, which is precisely the definition of "next greater/smaller".

---

## Important Notes

- Always push **indices**, not values. This is the single most common implementation mistake.
- Circular array variant: iterate `2n` indices using `i % n` to simulate wrapping. Traverse right to left and only record answers for `i < n`.
- The pop condition uses strict `<` for NGE — equal elements should **not** pop each other, or you'll get wrong answers on duplicate inputs. Be deliberate about strict vs non-strict comparison based on what the problem asks.
- Stock Span: for each day, the span is `i - top_of_stack` after popping all days with lower prices. This is just "Previous Greater" with index distance as the answer.
- Sum of Subarray Minimums: combine PSE and NSE to compute how many subarrays each element is the minimum of — then multiply and sum.
