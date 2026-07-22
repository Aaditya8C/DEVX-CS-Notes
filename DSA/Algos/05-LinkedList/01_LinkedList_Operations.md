# Linked List Operations

## Introduction

Core linked list manipulations — traversal, reversal, cycle detection, finding midpoint, and merging. Most linked list interview problems are combinations of these primitives.

---

## Intuition

Linked lists have no random access — every operation requires pointer traversal. The key patterns that make list problems tractable: **two pointers** (fast/slow for cycle/middle), **dummy nodes** (eliminate edge cases for head deletion/insertion), and **in-place pointer rewiring** (reversal, merging).

---

## When to Use

- Cycle detection / finding cycle start.
- Finding middle (for merge sort on list).
- Reversing a list or sublists.
- Merging two sorted lists.
- Removing nth node from end.
- Palindrome check on list.

---

## Complexity Analysis

All basic operations: **O(n) time, O(1) space** (in-place pointer manipulation).

---

## Critical Code Explanation

### Dummy Head Node

```cpp
ListNode dummy(0);
dummy.next = head;
ListNode* prev = &dummy;
```

A dummy node before the head means "the node before head" always exists. Operations that might remove the head or insert before it become uniform — no special case for `head == nullptr`.

### Fast/Slow Pointer for Middle

```cpp
ListNode *slow = head, *fast = head;
while (fast->next && fast->next->next) {
    slow = slow->next;
    fast = fast->next->next;
}
// slow is now at the middle (for odd length) or left-middle (for even length)
```

Fast moves 2 steps for every 1 slow step. When fast reaches the end, slow is at the middle. Use `fast->next && fast->next->next` to stop correctly.

### Reversal — Three Pointer

```cpp
ListNode *prev = nullptr, *curr = head, *next = nullptr;
while (curr) {
    next = curr->next;   // save next
    curr->next = prev;   // reverse link
    prev = curr;         // advance prev
    curr = next;         // advance curr
}
// prev is new head
```

At each step: save `next` before overwriting, reverse the pointer, advance both. After the loop, `curr == nullptr` and `prev` is the new head.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

// Reverse entire list
ListNode* reverseList(ListNode* head) {
    ListNode *prev = nullptr, *curr = head, *next = nullptr;
    while (curr) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}

// Find middle — slow is left-middle for even-length lists
ListNode* findMiddle(ListNode* head) {
    ListNode *slow = head, *fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}

// Detect cycle — Floyd's algorithm
bool hasCycle(ListNode* head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return true;
    }
    return false;
}

// Find cycle start
ListNode* detectCycleStart(ListNode* head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            slow = head;               // reset slow to head
            while (slow != fast) {     // advance both at same speed
                slow = slow->next;
                fast = fast->next;
            }
            return slow;               // meeting point = cycle start
        }
    }
    return nullptr;
}

// Remove nth node from end (1-indexed)
ListNode* removeNthFromEnd(ListNode* head, int n) {
    ListNode dummy(0); dummy.next = head;
    ListNode *fast = &dummy, *slow = &dummy;
    for (int i = 0; i <= n; i++) fast = fast->next;  // advance fast by n+1
    while (fast) { slow = slow->next; fast = fast->next; }
    slow->next = slow->next->next;    // delete target
    return dummy.next;
}

// Merge two sorted lists
ListNode* mergeSorted(ListNode* l1, ListNode* l2) {
    ListNode dummy(0);
    ListNode* curr = &dummy;
    while (l1 && l2) {
        if (l1->val <= l2->val) { curr->next = l1; l1 = l1->next; }
        else                    { curr->next = l2; l2 = l2->next; }
        curr = curr->next;
    }
    curr->next = l1 ? l1 : l2;
    return dummy.next;
}

// Check palindrome
bool isPalindrome(ListNode* head) {
    if (!head || !head->next) return true;
    ListNode* mid = findMiddle(head);
    ListNode* second = reverseList(mid->next);
    ListNode *p1 = head, *p2 = second;
    bool result = true;
    while (p2) {
        if (p1->val != p2->val) { result = false; break; }
        p1 = p1->next; p2 = p2->next;
    }
    mid->next = reverseList(second);  // restore list
    return result;
}
```

---

## Why It Works

**Cycle detection:** If a cycle exists, fast will eventually lap slow inside the cycle and they'll meet. If no cycle, fast hits nullptr first. **Cycle start:** After the first meeting, resetting slow to head and advancing both at speed 1 always converges at the cycle entry — this follows from the mathematical relationship between the meeting point distance and the cycle start distance.

**Nth from end:** Advance fast by `n+1` steps. When fast reaches null, slow is at the node before the target (which is `n` steps from the end).

---

## Important Notes

- Always use a **dummy node** for problems involving head deletion, insertion before head, or when the result might be a new head.
- After palindrome check, **restore the list** by reversing the second half back — callers expect the list to be unmodified.
- Cycle start detection relies on the invariant: when fast and slow meet inside the cycle, the distance from `head` to the cycle start equals the distance from the meeting point to the cycle start (advancing along the cycle). Proof requires modular arithmetic.
- For Merge Sort on a linked list: find middle, split, sort each half recursively, merge — O(n log n) with no extra space.
