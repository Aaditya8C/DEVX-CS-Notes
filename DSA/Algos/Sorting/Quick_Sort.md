# Quick Sort

## Introduction

A divide-and-conquer sorting algorithm that picks a pivot, partitions the array so all smaller elements are left and larger are right, then recursively sorts each side. Average O(n log n), in-place, but O(n²) worst case.

---

## Intuition

If you could find the median instantly, you'd split the array perfectly in half every time — O(n log n). Quick Sort gambles on this: pick any element as a pivot, rearrange so everything smaller is to its left and everything larger is to its right. The pivot is now in its final sorted position. Recurse on the two sides. If the pivot is consistently near the median, it's O(n log n); if it's always the min or max, it degrades to O(n²).

---

## When to Use

- General-purpose in-place sorting where average performance matters.
- When extra memory is not available (O(log n) stack space only).
- Cache-friendly access patterns are important (better cache behavior than Merge Sort in practice).

---

## Recognition Pattern

```
In-place sort
+ average-case performance is the priority
+ no stability requirement
```
→ Think Quick Sort. (But `std::sort` uses introsort — Quick Sort + Heap Sort fallback — so prefer that in practice.)

---

## Complexity Analysis

| Case    | Time       | Space     |
|---------|------------|-----------|
| Best    | O(n log n) | O(log n)  |
| Average | O(n log n) | O(log n)  |
| Worst   | O(n²)      | O(n)      |

Worst case occurs when the pivot is always the minimum or maximum (e.g., sorted array with last-element pivot). Space is O(log n) average for the call stack; O(n) worst case.

---

## Core Idea

1. Choose a pivot (last element, random, or median-of-three).
2. Partition: rearrange so elements `< pivot` are left, `> pivot` are right. Place the pivot in its final position.
3. Recursively sort the left and right sub-arrays around the pivot.

---

## Visualization

```
[5, 3, 8, 1, 2, 7]   pivot = 7 (last element)

Partition:
i starts at low-1 = -1

j=0: arr[0]=5 < 7 → i=0, swap(0,0) → [5, 3, 8, 1, 2, 7]
j=1: arr[1]=3 < 7 → i=1, swap(1,1) → [5, 3, 8, 1, 2, 7]
j=2: arr[2]=8 ≥ 7 → skip
j=3: arr[3]=1 < 7 → i=2, swap(2,3) → [5, 3, 1, 8, 2, 7]
j=4: arr[4]=2 < 7 → i=3, swap(3,4) → [5, 3, 1, 2, 8, 7]

Place pivot: swap(i+1=4, high=5) → [5, 3, 1, 2, 7, 8]
                                              ↑
                                       pivot at index 4

Recurse on [5,3,1,2] and [8]
```

---

## Critical Code Explanation

### Lomuto Partition

```cpp
int partition(vector<int>& arr, int low, int high) {
    int pivot = arr[high];  // last element as pivot
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(arr[i], arr[j]);  // grow the "less than" region
        }
    }
    swap(arr[i + 1], arr[high]);   // place pivot in correct position
    return i + 1;                  // pivot index
}
```

`i` tracks the boundary of elements smaller than the pivot. Every element `< pivot` gets swapped into the "less than" region. At the end, the pivot swaps into position `i+1`.

### Randomized Pivot — Avoiding O(n²)

```cpp
int randomPartition(vector<int>& arr, int low, int high) {
    int r = low + rand() % (high - low + 1);
    swap(arr[r], arr[high]);      // move random element to last position
    return partition(arr, low, high);
}
```

Randomizing the pivot selection makes the worst case extremely unlikely (probability 1/n! for any specific bad input). This is why `std::sort` uses introsort with randomization.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

int partition(vector<int>& arr, int low, int high) {
    int pivot = arr[high], i = low - 1;
    for (int j = low; j < high; j++)
        if (arr[j] < pivot) swap(arr[++i], arr[j]);
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

int randomPartition(vector<int>& arr, int low, int high) {
    swap(arr[low + rand() % (high - low + 1)], arr[high]);
    return partition(arr, low, high);
}

void quickSort(vector<int>& arr, int low, int high) {
    if (low < high) {
        int pi = randomPartition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}
```

---

## Why It Works

After partitioning, the pivot is in its exact final sorted position — nothing will ever move it again. Elements to its left are all smaller; elements to its right are all larger. Recursive application of this argument to each sub-array means every element eventually becomes a pivot and lands in its correct position.

---

## Important Notes

- Quick Sort is **not stable** — equal elements may be reordered during partitioning.
- **In-place** — O(log n) stack space for recursion, O(1) extra memory for data.
- Worst case O(n²) on already-sorted or reverse-sorted arrays with last-element pivot. Always randomize in production.
- **3-way partition** (Dutch National Flag): handle duplicate elements in O(n) instead of O(n²) when many equal elements exist. Partition into `< pivot`, `== pivot`, `> pivot`.
- `std::sort` in C++ uses **introsort**: Quick Sort with a depth limit that switches to Heap Sort when depth exceeds 2·log₂n, guaranteeing O(n log n) worst case.
- Tail call optimization: recurse on the smaller partition first to limit stack depth to O(log n).
