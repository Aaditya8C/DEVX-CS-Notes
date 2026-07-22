# Selection Sort

## Introduction

Repeatedly finds the minimum element from the unsorted portion and places it at the beginning. Makes exactly n-1 swaps regardless of input — useful when write cost is high.

---

## Intuition

Divide the array into a sorted left portion and an unsorted right portion. In each pass, scan the unsorted portion for the minimum and swap it into the next position of the sorted portion. The sorted portion grows by one element each pass.

---

## When to Use

- When the **number of swaps** must be minimized (Selection Sort always does at most n-1 swaps).
- Write-expensive memory (e.g., flash storage).
- Educational purposes.

---

## Recognition Pattern

```
"Minimize number of writes/swaps"
+ sorting required
```
→ Consider Selection Sort.

---

## Complexity Analysis

| Case    | Time  | Space |
|---------|-------|-------|
| Best    | O(n²) | O(1)  |
| Average | O(n²) | O(1)  |
| Worst   | O(n²) | O(1)  |

Always exactly n(n-1)/2 comparisons and at most n-1 swaps regardless of input.

---

## Visualization

```
[5, 3, 8, 1, 2]

i=0: min=1 at idx 3 → swap(0,3) → [1, 3, 8, 5, 2]
i=1: min=2 at idx 4 → swap(1,4) → [1, 2, 8, 5, 3]
i=2: min=3 at idx 4 → swap(2,4) → [1, 2, 3, 5, 8]
i=3: min=5 at idx 3 → swap(3,3) → [1, 2, 3, 5, 8]
Done
```

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

void selectionSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < n; j++)
            if (arr[j] < arr[minIdx]) minIdx = j;
        if (minIdx != i) swap(arr[i], arr[minIdx]);
    }
}
```

---

## Why It Works

After pass i, the i smallest elements are in their correct positions (sorted left portion). The minimum of the remaining elements is the (i+1)-th smallest overall, so placing it at position i is correct.

---

## Important Notes

- Selection Sort is **not stable** — swapping a minimum element to the front can displace an equal element that was earlier. Can be made stable with insertion instead of swap.
- **In-place** — O(1) extra space.
- Unlike Bubble Sort, no early termination is possible — even a sorted array requires all comparisons.
- Makes at most n-1 swaps — the key distinguishing property from other O(n²) sorts.
