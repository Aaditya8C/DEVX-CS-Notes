# Bubble Sort

## Introduction

Repeatedly compares adjacent elements and swaps them if they are in the wrong order. After each pass, the largest unsorted element "bubbles" to its correct position at the end. Simple but O(n²) — only useful for educational purposes or nearly-sorted small arrays.

---

## Intuition

In each pass, the largest element among the unsorted elements inevitably gets swapped all the way to its correct position at the right end — like a bubble floating to the surface. After k passes, the k largest elements are correctly placed. The swapped flag optimization exits early if no swaps occurred, giving O(n) on an already-sorted array.

---

## When to Use

- Almost-sorted data with very few swaps needed.
- Educational purposes / interview explanation.
- Rarely used in practice — prefer Insertion Sort for small or nearly-sorted inputs.

---

## Recognition Pattern

```
Rarely appropriate. If you see this in an interview, check whether
they actually want you to implement it or just explain it.
```

---

## Complexity Analysis

| Case    | Time  | Space |
|---------|-------|-------|
| Best    | O(n)  | O(1)  |
| Average | O(n²) | O(1)  |
| Worst   | O(n²) | O(1)  |

Best case O(n) only with the early-termination `swapped` flag.

---

## Core Idea

Outer loop runs n-1 times. Inner loop compares adjacent pairs from index 0 to `n-i-1` (the last `i` elements are already sorted). After each inner pass, the largest unsorted element is in its correct position. Track whether any swap happened; if not, the array is sorted — break early.

---

## Visualization

```
[5, 3, 8, 1, 2]

Pass 1: [3,5,8,1,2] → [3,5,8,1,2] → [3,5,1,8,2] → [3,5,1,2,8]  ← 8 settled
Pass 2: [3,5,1,2,8] → [3,1,5,2,8] → [3,1,2,5,8]                 ← 5 settled
Pass 3: [1,3,2,5,8] → [1,2,3,5,8]                                ← 3 settled
Pass 4: [1,2,3,5,8] → swapped=false → DONE
```

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;   // already sorted
    }
}
```

---

## Why It Works

After pass k, the k largest elements are in their final positions. The inner loop guarantees the current maximum propagates to the end by swapping with every adjacent smaller element.

---

## Important Notes

- Bubble Sort is **stable** — equal elements are never swapped.
- **In-place** — O(1) extra space.
- The `swapped` flag is essential for the O(n) best case — without it, all passes always run.
- In practice, always prefer `std::sort` or Insertion Sort for small arrays. Bubble Sort's only pedagogical value is its simplicity.
