# Two Pointers

## Introduction

Uses two indices moving through an array (inward, same direction, or at different speeds) to solve problems in O(n) that would naively require O(n²) nested loops.

---

## Intuition

The key observation: in many array problems, moving both pointers together still guarantees we explore all relevant pairs or subarrays. The sorted structure (or problem constraint) lets us make a greedy decision at each step — if the current pair doesn't work, we know which pointer to move. This systematic elimination of bad candidates without backtracking gives O(n).

---

## When to Use

- Two sum in a sorted array.
- Three sum / k sum.
- Trapping rain water.
- Container with most water.
- Removing duplicates from sorted array.
- Palindrome check.
- Merging two sorted arrays.
- Linked list cycle detection (fast/slow pointers).
- Finding the middle of a linked list.

---

## Recognition Pattern

```
Sorted array
+ "pair with property X"
+ "two elements summing to target"
+ O(n) required where brute force is O(n²)

OR:

Same array traversal
+ "fast pointer / slow pointer"
+ linked list cycle, middle, nth from end
```
→ Think Two Pointers.

---

## Complexity Analysis

| Case    | Time | Space |
|---------|------|-------|
| Best    | O(n) | O(1)  |
| Average | O(n) | O(1)  |
| Worst   | O(n) | O(1)  |

Each pointer moves at most n steps → O(n) total.

---

## Core Patterns

### Pattern 1: Opposite Ends (sorted array)

```cpp
int lo = 0, hi = n - 1;
while (lo < hi) {
    if (condition(arr[lo], arr[hi])) { /* use pair */ hi--; lo++; }
    else if (tooSmall) lo++;
    else hi--;
}
```

### Pattern 2: Same Direction (fast/slow)

```cpp
int slow = 0;
for (int fast = 0; fast < n; fast++)
    if (shouldKeep(arr[fast])) arr[slow++] = arr[fast];
```

### Pattern 3: Fast/Slow on Linked List

```cpp
ListNode* slow = head, *fast = head;
while (fast && fast->next) {
    slow = slow->next;
    fast = fast->next->next;
}
// slow is now at the middle
```

---

## Visualization

### Two Sum in Sorted Array

```
arr = [1, 2, 4, 6, 8, 9],  target = 10

lo=0(1), hi=5(9): sum=10 == target → FOUND (1,9)

arr = [1, 2, 4, 6, 8, 9],  target = 7

lo=0(1), hi=5(9): sum=10 > 7 → hi--
lo=0(1), hi=4(8): sum=9 > 7 → hi--
lo=0(1), hi=3(6): sum=7 == target → FOUND (1,6)
```

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Two sum in sorted array
pair<int,int> twoSum(vector<int>& arr, int target) {
    int lo = 0, hi = arr.size() - 1;
    while (lo < hi) {
        int sum = arr[lo] + arr[hi];
        if (sum == target) return {lo, hi};
        else if (sum < target) lo++;
        else hi--;
    }
    return {-1, -1};   // not found
}

// Three sum — all unique triplets summing to 0
vector<vector<int>> threeSum(vector<int>& nums) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> result;
    int n = nums.size();

    for (int i = 0; i < n - 2; i++) {
        if (i > 0 && nums[i] == nums[i-1]) continue;  // skip duplicates
        int lo = i + 1, hi = n - 1;
        while (lo < hi) {
            int sum = nums[i] + nums[lo] + nums[hi];
            if (sum == 0) {
                result.push_back({nums[i], nums[lo], nums[hi]});
                while (lo < hi && nums[lo] == nums[lo+1]) lo++;
                while (lo < hi && nums[hi] == nums[hi-1]) hi--;
                lo++; hi--;
            } else if (sum < 0) lo++;
            else hi--;
        }
    }
    return result;
}

// Remove duplicates from sorted array in-place
int removeDuplicates(vector<int>& nums) {
    if (nums.empty()) return 0;
    int slow = 0;
    for (int fast = 1; fast < (int)nums.size(); fast++)
        if (nums[fast] != nums[slow]) nums[++slow] = nums[fast];
    return slow + 1;
}

// Trapping Rain Water
int trap(vector<int>& height) {
    int lo = 0, hi = height.size() - 1;
    int maxL = 0, maxR = 0, water = 0;
    while (lo < hi) {
        if (height[lo] < height[hi]) {
            if (height[lo] >= maxL) maxL = height[lo];
            else water += maxL - height[lo];
            lo++;
        } else {
            if (height[hi] >= maxR) maxR = height[hi];
            else water += maxR - height[hi];
            hi--;
        }
    }
    return water;
}
```

---

## Why It Works

**Opposite ends:** In a sorted array, if `arr[lo] + arr[hi] > target`, the sum is too large — decreasing `hi` is the only way to reduce it. If too small, increasing `lo` is the only way. No valid pair is ever skipped because every move is forced by the constraint. All O(n²) pairs are implicitly eliminated in O(n) moves.

**Fast/slow:** The slow pointer marks the "write head" for valid elements; the fast pointer scans all elements. This ensures exactly the valid elements are retained in-place.

---

## Important Notes

- Two Pointers on sorted arrays is almost always the right approach when you see "find a pair" + O(n) required.
- For **3Sum**, fix one element with the outer loop and run two pointers on the rest — O(n²) total.
- **Duplicate handling in 3Sum:** Skip duplicate values for all three pointers to avoid duplicate triplets in the output.
- Trapping Rain Water's two-pointer approach works because the water at any position is bounded by `min(maxLeft, maxRight)`. The pointer on the shorter side is the limiting factor.
- Fast/slow pointers for **cycle detection** in linked lists (Floyd's Tortoise and Hare): if fast ever equals slow (after the start), a cycle exists.
