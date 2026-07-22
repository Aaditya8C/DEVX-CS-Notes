# Stack Using Queues / Queue Using Stacks

## Introduction

These are classic data structure design problems: implement a stack using only queue operations, or implement a queue using only stack operations. They test your understanding of how these two structures fundamentally differ and how you can simulate one with the other.

---

## Intuition

A stack is LIFO — the last element pushed is the first out. A queue is FIFO — the first in is the first out. They are opposites, which is exactly what makes this problem interesting. To simulate LIFO behavior using a FIFO structure, you need to reverse the order at some point. The trick for stack-from-queue is to rotate all previously enqueued elements to the back after every push, so the newest element always sits at the front. For queue-from-stacks, you use one stack as an inbox and another as an outbox: pushing inverts order once (LIFO), and popping inverts it again, and double inversion restores FIFO.

---

## When to Use

- System design or object-oriented design interviews asking you to implement one data structure using another.
- Problems that test understanding of amortized complexity.
- Whenever you need to reason about LIFO vs FIFO ordering and inversion.

---

## Complexity Analysis

### Stack Using One Queue — Push-Costly

| Operation | Time Complexity |
|-----------|----------------|
| `push`    | O(n)           |
| `pop`     | O(1)           |
| `top`     | O(1)           |
| `empty`   | O(1)           |

### Queue Using Two Stacks — Pop-Amortized

| Operation | Time Complexity     |
|-----------|---------------------|
| `push`    | O(1)                |
| `pop`     | O(n) worst, O(1) amortized |
| `peek`    | O(n) worst, O(1) amortized |
| `empty`   | O(1)                |

**Space Complexity:** O(n) in all variants — every element is stored exactly once.

---

## Core Idea

### Stack Using a Single Queue

Push `x` into the queue, then rotate all previously enqueued elements — dequeue each one from the front and re-enqueue at the back, doing this exactly `(size - 1)` times. After the rotation, `x` sits at the front, making it the next element to be dequeued — which mimics `top()` and `pop()` of a stack perfectly.

### Queue Using Two Stacks

Use an `inbox` stack for all pushes and an `outbox` stack for all pops. When a dequeue is requested and `outbox` is empty, drain `inbox` into `outbox`. Pushing to `inbox` produces elements in LIFO order; reversing them into `outbox` flips the order back to FIFO. Critically, the transfer only happens when `outbox` is completely empty — if there are still elements in `outbox`, they represent earlier enqueues and must be dequeued first.

---

## Visualization

### Stack Using One Queue — push(1), push(2), push(3)

```
After push(1):  front → [1]
After push(2):  enqueue 2 → [1, 2], rotate once → [2, 1]   front = 2
After push(3):  enqueue 3 → [2, 1, 3], rotate twice → [3, 2, 1] front = 3

top()  → 3  (front of queue)
pop()  → 3, queue = [2, 1]
top()  → 2
```

The front of the queue always holds the most recently pushed element.

---

### Queue Using Two Stacks — enqueue(1, 2, 3), then dequeue twice

```
push(1): inbox = [1]
push(2): inbox = [2, 1]
push(3): inbox = [3, 2, 1]

pop() called, outbox is empty → transfer:
    inbox = []    outbox = [1, 2, 3]   (top = 1, the oldest element)

pop() → returns 1,  outbox = [2, 3]
pop() → returns 2,  outbox = [3]       (no transfer needed)
```

---

## Critical Code Explanation

### Queue Rotation in Stack-From-Queue

```cpp
void push(int x) {
    q.push(x);
    for (int i = 0; i < (int)q.size() - 1; i++) {
        q.push(q.front());
        q.pop();
    }
}
```

After `q.push(x)`, `x` sits at the back. The loop dequeues and re-enqueues every element that was there before `x`, cycling them behind `x`. The loop runs exactly `size - 1` times — one pass per pre-existing element. After the loop, `x` is at the front. This is O(n) per push.

---

### Lazy Transfer in Queue-From-Stacks

```cpp
void transfer() {
    if (outbox.empty()) {
        while (!inbox.empty()) {
            outbox.push(inbox.top());
            inbox.pop();
        }
    }
}
```

The `if (outbox.empty())` guard is the key. If `outbox` still has elements, they are older than anything in `inbox` and must be served first. Transfer only happens when `outbox` is exhausted. Because each element crosses from `inbox` to `outbox` exactly once in its lifetime, the total cost across all dequeue operations is O(n) for n elements — making each dequeue O(1) amortized.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// ─── Stack using a single queue ──────────────────────────────────────────────

class MyStack {
    queue<int> q;

public:
    void push(int x) {
        q.push(x);
        // Rotate all previous elements behind x
        for (int i = 0; i < (int)q.size() - 1; i++) {
            q.push(q.front());
            q.pop();
        }
    }

    int pop() {
        int val = q.front();
        q.pop();
        return val;
    }

    int top()     { return q.front(); }
    bool empty()  { return q.empty(); }
};


// ─── Queue using two stacks ───────────────────────────────────────────────────

class MyQueue {
    stack<int> inbox, outbox;

    void transfer() {
        if (outbox.empty()) {
            while (!inbox.empty()) {
                outbox.push(inbox.top());
                inbox.pop();
            }
        }
    }

public:
    void push(int x) {
        inbox.push(x);
    }

    int pop() {
        transfer();
        int val = outbox.top();
        outbox.pop();
        return val;
    }

    int peek() {
        transfer();
        return outbox.top();
    }

    bool empty() {
        return inbox.empty() && outbox.empty();
    }
};
```

---

## Why It Works

**Stack from queue:** A queue preserves insertion order. By rotating every existing element to the back after a push, the invariant becomes: *the front of the queue is always the most recently pushed element*. Since `pop()` removes from the front, this exactly matches LIFO behavior.

**Queue from stacks:** Two reversals cancel each other out. Pushing to a stack reverses insertion order (LIFO). Popping from that stack into a second stack reverses it again, restoring the original insertion order — which is FIFO. The lazy transfer ensures we only reverse when necessary, amortizing the cost across multiple dequeue operations.

---

## Important Notes

- The `if (outbox.empty())` condition in the transfer must NOT be `while` — transferring when `outbox` still has elements would corrupt the ordering and break FIFO.
- For the stack-from-queue, `q.size()` changes during the loop — compute the iteration count before the loop or use the formulation shown above carefully.
- The queue-from-stacks dequeue is **O(n) worst case** for a single call, but **O(1) amortized** across a sequence of operations. Know both when asked in interviews.
- In practice, a stack-from-two-queues variant also exists: push to `q2`, drain `q1` into `q2`, then swap. It has identical complexity to the single-queue approach and is a bit more verbose.
- When asked in interviews, the **queue-from-two-stacks** problem appears far more frequently than stack-from-queue.
