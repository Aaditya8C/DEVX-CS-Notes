# Min Stack

## Purpose

> A stack that supports push, pop, top, and retrieving the minimum element — all in O(1) time.

---

## When to Use

- Any problem requiring tracking the running minimum alongside stack operations.
- Design problems asking for O(1) getMin with a stack.

---

## Time Complexity

| Case    | Complexity |
|---------|------------|
| Best    | O(1)       |
| Average | O(1)       |
| Worst   | O(1)       |

**Space Complexity:** O(n) — auxiliary min-stack of same size.

---

## Core Idea

- Maintain a second stack `minSt` that tracks the minimum at each level.
- On `push(x)`: push `x` to main stack, push `min(x, minSt.top())` to `minSt`.
- On `pop()`: pop both stacks simultaneously.
- `getMin()` = `minSt.top()`.
- The min-stack mirrors the main stack in size — every push/pop is paired.

---

## Critical Code Walkthrough

### Paired Push

```cpp
void push(int x) {
    st.push(x);
    int curMin = minSt.empty() ? x : min(x, minSt.top());
    minSt.push(curMin);
}
```

`minSt.top()` always holds the minimum of all elements currently in `st`.

### Why not just track one global min?

If the global minimum is popped, you'd have no way to know what the new minimum is without scanning the stack.

---

### Space-Optimized Variant (Single Stack, Encode Trick)

```cpp
// Store encoded value: push (2*x - prevMin) when x <= prevMin
// Decoding: if top < min, actual min = 2*min - top
```

This avoids a second stack but is harder to reason about — use the two-stack approach in interviews unless space is constrained.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

class MinStack {
    stack<int> st, minSt;

public:
    void push(int x) {
        st.push(x);
        int curMin = minSt.empty() ? x : min(x, minSt.top());
        minSt.push(curMin);
    }

    void pop() {
        st.pop();
        minSt.pop();
    }

    int top() {
        return st.top();
    }

    int getMin() {
        return minSt.top();
    }
};

// Space-optimized (single stack, O(1) space overhead)
class MinStackOptimized {
    stack<long long> st;
    long long curMin;

public:
    void push(long long x) {
        if (st.empty()) {
            st.push(x);
            curMin = x;
        } else if (x <= curMin) {
            st.push(2LL * x - curMin);  // encode
            curMin = x;
        } else {
            st.push(x);
        }
    }

    void pop() {
        if (st.top() < curMin)          // encoded → update min
            curMin = 2 * curMin - st.top();
        st.pop();
    }

    long long top() {
        return st.top() < curMin ? curMin : st.top();
    }

    long long getMin() {
        return curMin;
    }
};
```

---

## Notes

- Two-stack solution is always safe and easy to explain in interviews.
- Space-optimized version uses `long long` to prevent overflow in `2*x - prevMin`.
- `getMin()` is O(1) in both variants.
- Similarly, a **Max Stack** uses the same pattern with `max` instead of `min`.
