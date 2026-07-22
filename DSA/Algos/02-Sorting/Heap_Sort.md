# Heap Sort

## Introduction

Sorts an array by first building a max-heap, then repeatedly extracting the maximum element and placing it at the end. In-place, O(n log n) guaranteed, but not stable.

---

## Intuition

A max-heap keeps the largest element at index 0. Swap it with the last element (it's now in its final sorted position), reduce the heap size by 1, and re-heapify the root. Repeat. The sorted portion grows from right to left. Building the heap takes O(n) with the Floyd bottom-up method — not O(n log n) as naive insertion would suggest.

---

## When to Use

- In-place O(n log n) sort with guaranteed worst case.
- When you need a sort with no extra memory and no randomization.
- Implementing partial sorts: finding the k largest elements using a heap.

---

## Recognition Pattern

```
"Sort in-place"
+ O(n log n) guaranteed worst case
+ no extra memory
```
→ Heap Sort (or introsort in practice).

---

## Complexity Analysis

| Case    | Time       | Space |
|---------|------------|-------|
| Best    | O(n log n) | O(1)  |
| Average | O(n log n) | O(1)  |
| Worst   | O(n log n) | O(1)  |

Build heap: O(n). Each of n extractions: O(log n). Total: O(n log n).

---

## Visualization

```
[4, 10, 3, 5, 1]

Build max-heap (bottom-up):
Heapify from n/2-1=1 down to 0:
  heapify(1): children at 3,4 → max=10→arr[1]=10, swap(1,3): [4,5,3,10,1]
  Oops—correcting: arr=[4,10,3,5,1], heapify(1): left=arr[3]=5, right=arr[4]=1, max=10 already → no swap
  heapify(0): left=arr[1]=10 > arr[0]=4 → swap: [10,4,3,5,1] → heapify(1): arr[1]=4 < arr[3]=5 → swap: [10,5,3,4,1]

Max-heap: [10, 5, 3, 4, 1]

Extract phase:
  swap(0,4): [1,5,3,4,10] → heapify(0) → [5,4,3,1,|10]
  swap(0,3): [1,4,3,5,10] → heapify(0) → [4,1,3,|5,10]
  swap(0,2): [3,1,4,5,10] → heapify(0) → [3,1,|4,5,10]
  swap(0,1): [1,3,4,5,10] → heapify → [1,|3,4,5,10]
  Result: [1, 3, 4, 5, 10]
```

---

## Critical Code Explanation

### Heapify — Sift Down

```cpp
void heapify(vector<int>& arr, int n, int i) {
    int largest = i;
    int l = 2*i + 1, r = 2*i + 2;

    if (l < n && arr[l] > arr[largest]) largest = l;
    if (r < n && arr[r] > arr[largest]) largest = r;

    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);   // sift down recursively
    }
}
```

`heapify` assumes both subtrees are already valid heaps and fixes the root. The key: swap root with the larger child, then recursively fix the affected subtree.

### Floyd's Build-Heap — O(n)

```cpp
for (int i = n/2 - 1; i >= 0; i--)
    heapify(arr, n, i);
```

Start from the last non-leaf node (`n/2 - 1`) and heapify downward. Leaves don't need heapification. This is O(n) because lower levels (most nodes) do very little work, and the O(log n) work happens only near the root (few nodes).

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

void heapify(vector<int>& arr, int n, int i) {
    int largest = i, l = 2*i+1, r = 2*i+2;
    if (l < n && arr[l] > arr[largest]) largest = l;
    if (r < n && arr[r] > arr[largest]) largest = r;
    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSort(vector<int>& arr) {
    int n = arr.size();

    // Build max-heap — O(n)
    for (int i = n/2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // Extract elements one by one
    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);   // move current max to end
        heapify(arr, n - 1 - (n-1-i), 0);  // simplified: heapify(arr, i, 0)
    }
}

// Cleaner version
void heapSortClean(vector<int>& arr) {
    int n = arr.size();
    for (int i = n/2 - 1; i >= 0; i--) heapify(arr, n, i);
    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}
```

---

## Why It Works

The max-heap property ensures `arr[0]` is always the largest element in the current heap. Swapping it to the end places it in its final sorted position. Reducing heap size by 1 and re-heapifying from the root restores the heap property for the remaining elements. This process is repeated until only one element remains.

---

## Important Notes

- Heap Sort is **not stable** — the extraction and heapify process can displace equal elements.
- **In-place** — O(1) extra space (recursion stack for heapify is O(log n), but iterative heapify is O(1)).
- In practice, Heap Sort has poor cache performance compared to Quick Sort because heap access patterns jump around in memory. This is why `std::sort` prefers Quick Sort + fallback over pure Heap Sort.
- Floyd's O(n) build-heap is a classic interview question. The key insight: leaf nodes don't need heapification; levels near the bottom do minimal work.
