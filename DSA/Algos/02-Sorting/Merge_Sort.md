# Merge Sort

## Introduction

A divide-and-conquer sorting algorithm that recursively splits the array in half, sorts each half, then merges them. Guarantees O(n log n) in all cases and is stable.

---

## Intuition

Merging two sorted arrays into one is trivial — just compare the front elements and pick the smaller. Merge Sort exploits this: keep halving until subarrays are length 1 (trivially sorted), then merge up the recursion tree. The split is O(1), the merge is O(n), and there are O(log n) levels — total O(n log n).

---

## When to Use

- Guaranteed O(n log n) — when worst-case matters (unlike Quick Sort).
- Sorting linked lists (no random access required; merge is natural).
- External sorting (too large for memory — merge from disk chunks).
- When stability is required.
- Counting inversions (modify the merge step).

---

## Recognition Pattern

```
"Sort stably"
"Count inversions"
"Sort a linked list"
"Guaranteed worst-case O(n log n)"
```
→ Think Merge Sort.

---

## Complexity Analysis

| Case    | Time       | Space |
|---------|------------|-------|
| Best    | O(n log n) | O(n)  |
| Average | O(n log n) | O(n)  |
| Worst   | O(n log n) | O(n)  |

**Space:** O(n) for the temporary merge buffer. Not in-place.

---

## Core Idea

Recursively: `mergeSort(arr, l, r)` → split at `mid = (l+r)/2` → sort left half → sort right half → merge. The merge step uses a temporary array to combine two sorted halves back into `arr[l..r]`.

---

## Visualization

```
[5, 3, 8, 1, 2, 7]

Split:
[5, 3, 8] | [1, 2, 7]
[5,3]|[8] | [1,2]|[7]
[5]|[3]     [1]|[2]

Merge up:
[3,5] ← merge [3],[5]    [1,2] ← merge [1],[2]
[3,5,8] ← merge [3,5],[8]    [1,2,7] ← merge [1,2],[7]

[1,2,3,5,7,8] ← final merge
```

---

## Critical Code Explanation

### The Merge Step

```cpp
void merge(vector<int>& arr, int l, int mid, int r) {
    vector<int> tmp(arr.begin() + l, arr.begin() + r + 1);
    int i = 0, j = mid - l + 1, k = l;

    while (i <= mid - l && j <= r - l) {
        if (tmp[i] <= tmp[j]) arr[k++] = tmp[i++];
        else                  arr[k++] = tmp[j++];
    }
    while (i <= mid - l) arr[k++] = tmp[i++];
    while (j <= r - l)   arr[k++] = tmp[j++];
}
```

`<=` in `tmp[i] <= tmp[j]` ensures stability — equal elements from the left subarray are placed before those from the right.

### Inversion Count (Merge Sort Modification)

```cpp
// During merge, when tmp[j] < tmp[i]:
inversions += (mid - l + 1) - i;  // all remaining elements in left half are > tmp[j]
```

Every element in the left half remaining when we pick from the right forms an inversion. This counts all inversions in O(n log n).

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

void merge(vector<int>& arr, int l, int mid, int r) {
    vector<int> tmp(arr.begin() + l, arr.begin() + r + 1);
    int i = 0, j = mid - l + 1, k = l;

    while (i <= mid - l && j <= r - l) {
        if (tmp[i] <= tmp[j]) arr[k++] = tmp[i++];
        else                  arr[k++] = tmp[j++];
    }
    while (i <= mid - l) arr[k++] = tmp[i++];
    while (j <= r - l)   arr[k++] = tmp[j++];
}

void mergeSort(vector<int>& arr, int l, int r) {
    if (l >= r) return;
    int mid = l + (r - l) / 2;
    mergeSort(arr, l, mid);
    mergeSort(arr, mid + 1, r);
    merge(arr, l, mid, r);
}

// Count inversions
long long mergeCount(vector<int>& arr, int l, int r) {
    if (l >= r) return 0;
    int mid = l + (r - l) / 2;
    long long inv = mergeCount(arr, l, mid) + mergeCount(arr, mid + 1, r);

    vector<int> tmp(arr.begin() + l, arr.begin() + r + 1);
    int i = 0, j = mid - l + 1, k = l;
    while (i <= mid - l && j <= r - l) {
        if (tmp[i] <= tmp[j]) arr[k++] = tmp[i++];
        else {
            inv += (mid - l + 1) - i;  // all left elements > tmp[j]
            arr[k++] = tmp[j++];
        }
    }
    while (i <= mid - l) arr[k++] = tmp[i++];
    while (j <= r - l)   arr[k++] = tmp[j++];
    return inv;
}
```

---

## Why It Works

At every level of recursion, two sorted halves are merged into one sorted array. By induction, both halves are correctly sorted (base case: length 1 is trivially sorted). The merge step is provably correct — it always picks the globally smallest remaining element from either half. After O(log n) levels, the entire array is sorted.

---

## Important Notes

- Merge Sort is **stable** — equal elements maintain their relative order.
- **Not in-place** — requires O(n) extra space for the temporary buffer. In-place merge sort exists but is significantly more complex and rarely used.
- `mid = l + (r-l)/2` avoids integer overflow; `(l+r)/2` can overflow for large indices.
- For sorting linked lists, Merge Sort is preferred over Quick Sort because linked list access is sequential (no random access), and the merge step works naturally without extra memory.
- The inversion count trick is a classic competitive programming technique — any problem that maps to "how many pairs are out of order" is solved with modified Merge Sort.
