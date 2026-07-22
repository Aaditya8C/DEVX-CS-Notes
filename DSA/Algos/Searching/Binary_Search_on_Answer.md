# Binary Search on Answer

## Introduction

When the answer to a problem is monotone — every value below the answer is infeasible and every value above is feasible (or vice versa) — binary search can find the exact boundary in O(log(range) × check_cost) instead of checking every value linearly.

---

## Intuition

Binary search on answer works whenever you can convert "find the optimal X" into "is X achievable?" — and that achievability has a monotone structure: if X is feasible, then X+1 might be too (or isn't). This is like binary searching on a sorted boolean array of `[false, false, ..., true, true, true]` (or reversed). The answer is the first `true`.

---

## When to Use

- "Minimize the maximum" or "Maximize the minimum".
- "Is it possible to achieve X?" has a yes/no answer monotone in X.
- Allocating tasks to workers with minimum time.
- Finding the kth element in a sorted matrix.
- Cutting logs to get at least k pieces.

---

## Recognition Pattern

```
"Minimize the maximum"
"Maximize the minimum"
"Allocate X to Y agents such that max/min is minimized"
"Can we achieve X?" where yes/no flips exactly once
"Kth smallest/largest in a structure"
```
→ Think Binary Search on Answer.

---

## Complexity Analysis

O(log(hi - lo) × cost of `feasible(mid)`)

Where `feasible(mid)` is typically O(n) for most problems → total O(n log n).

---

## Visualization

```
"Koko Eating Bananas" — minimum speed k such that she can eat all bananas in h hours.

piles = [3,6,7,11], h = 8

Answer space: [1..11]  (max speed = max pile = 11)
feasible(k): ceil(3/k) + ceil(6/k) + ceil(7/k) + ceil(11/k) <= 8?

k=6: 1+1+2+2=6 ≤ 8 → feasible
k=3: 1+2+3+4=10 > 8 → infeasible
k=4: 1+2+2+3=8 ≤ 8 → feasible
k=5: 1+2+2+3=8 ≤ 8 → feasible (wait, ceil(6/5)=2, ceil(11/5)=3, total=1+2+2+3=8 ≤ 8)

Binary search finds k=4 as minimum feasible.
```

---

## Critical Code Explanation

### Template

```cpp
int lo = min_possible_answer, hi = max_possible_answer;
while (lo < hi) {
    int mid = lo + (hi - lo) / 2;
    if (feasible(mid)) hi = mid;   // mid might be the answer, don't exclude it
    else lo = mid + 1;             // mid is too small, exclude it
}
return lo;
```

When `feasible(mid)` is true: `hi = mid` (not `hi = mid - 1`) because `mid` itself could be the answer. When false: `lo = mid + 1`. This template finds the **minimum feasible value**.

For **maximum feasible value**: flip the condition: `if (feasible(mid)) lo = mid + 1 else hi = mid`, return `lo - 1`.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Minimum speed to eat all bananas in h hours
int minEatingSpeed(vector<int>& piles, int h) {
    auto feasible = [&](long long k) {
        long long hours = 0;
        for (int p : piles) hours += (p + k - 1) / k;  // ceil(p/k)
        return hours <= h;
    };

    int lo = 1, hi = *max_element(piles.begin(), piles.end());
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (feasible(mid)) hi = mid;
        else lo = mid + 1;
    }
    return lo;
}

// Allocate books to m students — minimize maximum pages assigned to any student
int allocateBooks(vector<int>& pages, int m) {
    auto feasible = [&](long long maxPages) {
        int students = 1;
        long long curSum = 0;
        for (int p : pages) {
            if (curSum + p > maxPages) { students++; curSum = 0; }
            curSum += p;
        }
        return students <= m;
    };

    long long lo = *max_element(pages.begin(), pages.end());
    long long hi = 0; for (int p : pages) hi += p;

    while (lo < hi) {
        long long mid = lo + (hi - lo) / 2;
        if (feasible(mid)) hi = mid;
        else lo = mid + 1;
    }
    return lo;
}
```

---

## Why It Works

The feasibility function defines a monotone boundary in the answer space. If the answer space looks like `[F, F, F, T, T, T]` (infeasible below, feasible above), the first `T` is exactly what we want. Binary search finds this boundary in O(log(range)) evaluations of `feasible()`.

---

## Important Notes

- Define `lo` and `hi` as the **actual possible answer range**, not an arbitrary large range. Wrong bounds cause wrong answers or infinite loops.
- The `feasible` function must be **purely monotone** — if mid is feasible, all values beyond it (in the direction of the answer) must also be feasible. Any non-monotone feasibility breaks binary search.
- Use `long long` for lo/hi when the answer space is large (>10⁸) to avoid overflow in `lo + hi`.
- Always double-check: does `feasible(lo)` being false mean `lo + 1` is definitely the answer? Is the answer guaranteed to be within `[lo, hi]`?
- The "minimize maximum / maximize minimum" pattern is extremely common in system design and interview questions.
