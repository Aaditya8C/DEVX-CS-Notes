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

### Initial Unsorted Array: `[4, 10, 3, 5, 1]`

```text
       4 (idx 0)
      / \
    10   3
   /  \
  5    1
```

---

### Phase 1: Build Max-Heap (Floyd's Bottom-Up)

**Start at last non-leaf node `i = n/2 - 1 = 1`:**

#### 1. `heapify(i=1)` — Node `10`
- Children of `10` are `5` (left) and `1` (right).
- `10` is already greater than both. No swap needed.

```text
       4
      / \
    10   3
   /  \
  5    1
```

#### 2. `heapify(i=0)` — Node `4`
- Children of `4` are `10` (left) and `3` (right). Max child is `10`.
- Swap `4` and `10`:

```text
      10
      / \
     4   3
    / \
   5   1
```

- Sift-down `4` at index `1`: Children are `5` and `1`. Max child is `5`.
- Swap `4` and `5`:

```text
      10
      / \
     5   3
    / \
   4   1
```

**Max-Heap Array:** `[10, 5, 3, 4, 1]`

---

### Phase 2: Extraction & Sorting

#### Extract 1: Max = `10`
1. Swap root `10` with last element `1` → Array: `[1, 5, 3, 4, | 10]`
2. `heapify(0)` on remaining 4 elements:
   - Swap `1` and `5` → then swap `1` and `4`:

```text
       5
      / \
     4   3       [10] (sorted)
    /
   1
```
**Array:** `[5, 4, 3, 1, | 10]`

#### Extract 2: Max = `5`
1. Swap root `5` with last element `1` → Array: `[1, 4, 3, | 5, 10]`
2. `heapify(0)` on remaining 3 elements:
   - Swap `1` and `4`:

```text
       4
      / \        [5, 10] (sorted)
     1   3
```
**Array:** `[4, 1, 3, | 5, 10]`

#### Extract 3: Max = `4`
1. Swap root `4` with last element `3` → Array: `[3, 1, | 4, 5, 10]`
2. `heapify(0)` on remaining 2 elements:
   - `3` > `1`, no swap needed:

```text
       3
      /          [4, 5, 10] (sorted)
     1
```
**Array:** `[3, 1, | 4, 5, 10]`

#### Extract 4: Max = `3`
1. Swap root `3` with last element `1` → Array: `[1, | 3, 4, 5, 10]`
2. Remaining 1 element is trivially sorted.

```text
      (1)        [3, 4, 5, 10] (sorted)
```

**Final Sorted Array:** `[1, 3, 4, 5, 10]`

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
