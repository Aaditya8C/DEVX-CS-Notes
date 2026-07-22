# Insertion Sort

## Introduction

Builds the sorted array one element at a time by inserting each new element into its correct position among the already-sorted elements. Excellent for small or nearly-sorted arrays — the algorithm of choice for small subarrays in hybrid sorts like TimSort and IntroSort.

---

## Intuition

Think of sorting playing cards in your hand. You pick up cards one at a time and slide each card left until it's in the right position among the cards already sorted in your hand. The key insight: the left portion is always maintained as sorted. When inserting the next element, shift right everything larger than it, then place it.

---

## When to Use

- Small arrays (n ≤ 20–30).
- Nearly-sorted arrays — O(n) if very few elements are out of place.
- Online sorting — elements arrive one at a time and you maintain a sorted structure.
- As the base case of Merge Sort or Quick Sort (TimSort switches to Insertion Sort below a threshold).

---

## Recognition Pattern

```
"Nearly sorted"
"Online" (elements arrive one at a time)
"Small subarray base case"
```
→ Think Insertion Sort.

---

## Complexity Analysis

| Case    | Time  | Space |
|---------|-------|-------|
| Best    | O(n)  | O(1)  |
| Average | O(n²) | O(1)  |
| Worst   | O(n²) | O(1)  |

Best case O(n) when the array is already sorted — the inner loop never executes. Number of operations = number of inversions.

---

## Visualization

```
[5, 3, 8, 1, 2]

i=1: key=3. Shift 5 right → [5,5,8,1,2] → insert 3 → [3,5,8,1,2]
i=2: key=8. 8>5, no shift → [3,5,8,1,2]
i=3: key=1. Shift 8,5,3 → insert 1 → [1,3,5,8,2]
i=4: key=2. Shift 8,5,3 → insert 2 → [1,2,3,5,8]
```

---

## Critical Code Explanation

### The Shift Instead of Swap

```cpp
int key = arr[i];
int j = i - 1;
while (j >= 0 && arr[j] > key) {
    arr[j + 1] = arr[j];   // shift right — not swap
    j--;
}
arr[j + 1] = key;          // insert in correct position
```

Shifting (overwrite) rather than swapping is more efficient — each shift is one write, while a swap is three writes. The `key` is saved, the gap is opened, then `key` is placed in one write.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

void insertionSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Binary Insertion Sort — O(n log n) comparisons, O(n²) shifts
void binaryInsertionSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int pos = lower_bound(arr.begin(), arr.begin() + i, key) - arr.begin();
        for (int j = i; j > pos; j--) arr[j] = arr[j - 1];
        arr[pos] = key;
    }
}
```

---

## Why It Works

The invariant: after processing index `i`, `arr[0..i]` is sorted. Base case: `arr[0..0]` is trivially sorted. When processing `i+1`, we find the correct position for `arr[i+1]` within the sorted `arr[0..i]` by scanning left and shifting, then place it — maintaining the invariant.

---

## Important Notes

- Insertion Sort is **stable** — the `arr[j] > key` condition (`>` not `>=`) ensures equal elements don't get shifted past the new element.
- **In-place** — O(1) extra space.
- Best when the number of inversions is small — O(n + inversions) operations.
- TimSort (Python's and Java's default sort) uses Insertion Sort for runs shorter than ~64 elements.
- Binary Insertion Sort reduces comparisons to O(n log n) but not the shift operations — total is still O(n²) moves.
