# Min Stack

## Introduction

A stack that supports `push`, `pop`, `top`, and `getMin` — all in O(1) time. The challenge is that a standard stack has no memory of past states, so tracking the minimum requires extra bookkeeping.

---

## Intuition

The problem with a single global `min` variable is that when you pop the current minimum, you have no way to recover what the previous minimum was. The fix is to snapshot the minimum at every stack level. A second parallel stack (`minSt`) stores the current minimum *as of* each push. When you pop the main stack, you also pop `minSt`, which automatically restores the previous minimum. The two stacks are always in sync.

---

## When to Use

- Design problems: "implement a stack with O(1) getMin/getMax".
- Any situation where you need the minimum/maximum of a dynamic set with push-pop operations.
- Subproblems inside larger algorithms (e.g., tracking running min during a sliding window).

---

## Recognition Pattern

```
"Stack with O(1) minimum"
"Stack with O(1) maximum"
"getMin() in constant time"
"Track minimum/maximum without scanning"
```
→ Think Min Stack (parallel tracking stack).

---

## Complexity Analysis

### Two-Stack Approach

| Operation | Time | Space |
|-----------|------|-------|
| `push`    | O(1) | O(1) amortized |
| `pop`     | O(1) | —     |
| `top`     | O(1) | —     |
| `getMin`  | O(1) | —     |

**Total Space:** O(n) — `minSt` mirrors `st` in size.

### Single-Stack Encoded Approach

Same time complexity, O(1) extra space beyond the single stack, but requires `long long` and careful decoding.

---

## Core Idea

Maintain a parallel `minSt` stack. On every `push(x)`, push to `st` normally and push `min(x, minSt.top())` to `minSt`. The `minSt` top always reflects the global minimum of everything currently in `st`. On `pop()`, pop both stacks. `getMin()` is simply `minSt.top()`.

---

## Visualization

```
Operations: push(5), push(3), push(7), push(2), pop()

push(5):  st=[5]       minSt=[5]       getMin=5
push(3):  st=[5,3]     minSt=[5,3]     getMin=3
push(7):  st=[5,3,7]   minSt=[5,3,3]   getMin=3  ← min stays 3
push(2):  st=[5,3,7,2] minSt=[5,3,3,2] getMin=2
pop():    st=[5,3,7]   minSt=[5,3,3]   getMin=3  ← correctly restored
```

When `2` is popped, `minSt` pops too and reveals that the previous minimum was `3`. No scanning required.

---

## Critical Code Explanation

### Paired Push — The Core Mechanism

```cpp
void push(int x) {
    st.push(x);
    int curMin = minSt.empty() ? x : min(x, minSt.top());
    minSt.push(curMin);
}
```

`minSt.top()` represents "what is the minimum of everything below the current element in `st`". By pushing `min(x, minSt.top())`, the new `minSt.top()` represents "the minimum of everything in `st` including `x`". When `x` is later popped, `minSt.pop()` restores the previous minimum automatically.

### Why a Global Variable Fails

```cpp
// WRONG approach
int globalMin;
void pop() {
    st.pop();
    // If we just popped globalMin, what is the new minimum?
    // We'd have to scan the entire stack → O(n)
}
```

A single variable loses history. The parallel stack is a history of minimums at each stack depth.

### Space-Optimized: Encoding Trick

```cpp
void push(long long x) {
    if (x <= curMin) {
        st.push(2LL * x - curMin);  // encode: value < curMin signals "this was a min update"
        curMin = x;
    } else {
        st.push(x);
    }
}

void pop() {
    if (st.top() < curMin)          // encoded value → recover previous min
        curMin = 2 * curMin - st.top();
    st.pop();
}
```

When `x <= curMin`, store `2x - prevMin` (which is always `< x <= curMin`, so it's a sentinel). On pop, detect the sentinel by checking `top < curMin` and decode `prevMin = 2*curMin - top`.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// ── Two-stack approach (clear and interview-safe) ─────────────────────────────
class MinStack {
    stack<int> st, minSt;

public:
    void push(int x) {
        st.push(x);
        minSt.push(minSt.empty() ? x : min(x, minSt.top()));
    }

    void pop() {
        st.pop();
        minSt.pop();
    }

    int top()    { return st.top(); }
    int getMin() { return minSt.top(); }
    bool empty() { return st.empty(); }
};


// ── Single-stack encoded approach (O(1) extra space) ─────────────────────────
class MinStackOptimized {
    stack<long long> st;
    long long curMin = LLONG_MAX;

public:
    void push(long long x) {
        if (st.empty()) {
            st.push(x);
            curMin = x;
        } else if (x <= curMin) {
            st.push(2LL * x - curMin);   // encode previous min info
            curMin = x;
        } else {
            st.push(x);
        }
    }

    void pop() {
        if (st.top() < curMin)           // encoded → restore previous min
            curMin = 2LL * curMin - st.top();
        st.pop();
    }

    long long top() {
        return st.top() < curMin ? curMin : st.top();
    }

    long long getMin() { return curMin; }
};
```

---

## Why It Works

The two-stack approach maintains the invariant: *`minSt[i]` = minimum of `st[0..i]`*. This invariant is established on every push and preserved on every pop because both stacks are always updated together. `getMin()` reads `minSt.top()`, which by the invariant is the minimum of all current elements.

The single-stack encoding works because `2x - prevMin < x` whenever `x <= prevMin` (since `x - prevMin <= 0`). This guarantees encoded values are always strictly less than `curMin`, creating a detectable sentinel that carries enough information to recover `prevMin = 2*curMin - encodedValue`.

---

## Important Notes

- In the two-stack approach, **both stacks must always be popped together** — if you forget to pop `minSt`, it desynchronizes and gives stale minimums.
- The space-optimized version must use `long long` to avoid overflow in `2LL * x - curMin`. With `int`, inputs near `INT_MIN` will overflow silently.
- A **Max Stack** is identical — replace `min` with `max` everywhere.
- The problem often appears in LeetCode as "Min Stack" (155) and is a common warm-up in system design rounds.
- If asked to also support `getMax()` alongside `getMin()`, simply add a third parallel `maxSt` stack.
