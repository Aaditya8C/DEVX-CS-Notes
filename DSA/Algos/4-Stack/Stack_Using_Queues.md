# Stack Using Queues / Queue Using Stacks

## Introduction

Implement a stack using only queue operations, or a queue using only stack operations. The key insight in both cases is that the two structures differ only in ordering — and ordering can be reversed.

---

## Intuition

The moment you realize that reversing order twice restores the original order, both problems collapse. A queue is FIFO; a stack is LIFO — they are exact opposites. To fake LIFO from a FIFO structure, you force every new element to the front after insertion. To fake FIFO from a LIFO structure, you use two stacks: pushing reverses order once, and popping from the second stack reverses it again. Double inversion = original order.

---

## When to Use

- Interview design questions that ask you to implement one data structure using another.
- Any setting where you need to reason about amortized cost and lazy evaluation.
- Problems that test whether you understand the structural difference between LIFO and FIFO.

---

## Recognition Pattern

```
"Implement stack using queues"
"Implement queue using stacks"
"Simulate X using Y"
"O(1) amortized dequeue"
```

If you see any of these → this problem.

---

## Complexity Analysis

### Stack Using One Queue

| Operation | Complexity |
|-----------|------------|
| `push`    | O(n)       |
| `pop`     | O(1)       |
| `top`     | O(1)       |
| `empty`   | O(1)       |

### Queue Using Two Stacks

| Operation | Worst Case | Amortized |
|-----------|------------|-----------|
| `push`    | O(1)       | O(1)      |
| `pop`     | O(n)       | O(1)      |
| `peek`    | O(n)       | O(1)      |
| `empty`   | O(1)       | O(1)      |

**Space:** O(n) in both — every element is stored exactly once.

---

## Core Idea

### Stack from One Queue

After pushing `x`, rotate all previously enqueued elements to the back. The queue always holds elements with the most recently pushed one at the front. `pop()` and `top()` just read the front.

### Queue from Two Stacks

`inbox` absorbs all pushes. When a pop is needed and `outbox` is empty, drain `inbox` into `outbox` — this reverses the order, placing the oldest element on top of `outbox`. Never transfer while `outbox` still has elements, or you corrupt FIFO order.

---

## Visualization

### Stack from Queue — push(1), push(2), push(3)

```
push(1):
  enqueue 1 → [1]
  rotate 0 times (size-1 = 0)
  queue: front → [1]

push(2):
  enqueue 2 → [1, 2]
  rotate 1 time: dequeue 1, enqueue 1 → [2, 1]
  queue: front → [2, 1]

push(3):
  enqueue 3 → [2, 1, 3]
  rotate 2 times: → [3, 2, 1]
  queue: front → [3, 2, 1]

top() → 3    pop() → 3    queue: [2, 1]
top() → 2    pop() → 2    queue: [1]
```

---

### Queue from Two Stacks — push(1,2,3), then pop twice

```
push(1): inbox = [1]
push(2): inbox = [2, 1]      ← top = 2 (LIFO order)
push(3): inbox = [3, 2, 1]

pop() → outbox is empty → transfer:
  inbox → outbox: outbox = [1, 2, 3]  ← top = 1 (oldest)

pop() → 1,  outbox = [2, 3]
pop() → 2,  outbox = [3]    ← no transfer needed, outbox not empty
```

---

## Critical Code Explanation

### Queue Rotation — Stack from Queue

```cpp
void push(int x) {
    q.push(x);
    for (int i = 0; i < (int)q.size() - 1; i++) {
        q.push(q.front());
        q.pop();
    }
}
```

After enqueueing `x`, the loop runs exactly `(size - 1)` times — once per element that existed before `x`. Each iteration moves the front element to the back, cycling everything older than `x` behind it. The loop count must be fixed before the loop starts conceptually — in the code above, `q.size()` is read each iteration but because the queue size never changes (one dequeue + one enqueue per iteration), it stays correct.

---

### Lazy Transfer — Queue from Two Stacks

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

The `if (outbox.empty())` guard is everything. If you transferred while `outbox` still had elements, older elements would get buried under newer ones — breaking FIFO. Only transfer when `outbox` is completely drained. Each element crosses from `inbox` to `outbox` exactly once in its lifetime, so the total transfer cost across all operations is O(n), making each individual pop O(1) amortized.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// ── Stack using a single queue ────────────────────────────────────────────────

class MyStack {
    queue<int> q;

public:
    void push(int x) {
        q.push(x);
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

    int top()    { return q.front(); }
    bool empty() { return q.empty(); }
};


// ── Queue using two stacks ─────────────────────────────────────────────────────

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
    void push(int x) { inbox.push(x); }

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

    bool empty() { return inbox.empty() && outbox.empty(); }
};
```

---

## Why It Works

**Stack from queue:** The invariant is: *the front of the queue always holds the most recently pushed element*. The rotation after each push establishes and maintains this invariant. Since `pop()` dequeues from the front, it always removes the most recently pushed element — which is exactly LIFO.

**Queue from two stacks:** Each element transitions from `inbox` to `outbox` exactly once. In `inbox`, elements are stored in push order (newest on top). When drained into `outbox`, this order is reversed, putting the oldest element on top. Popping from `outbox` removes oldest first — FIFO. The guard `if (outbox.empty())` ensures older elements are never buried under newer ones.

---

## Important Notes

- **The `if` vs `while` bug:** Use `if (outbox.empty())`, never `while`. Transferring while `outbox` still has elements destroys FIFO ordering — this is the most common mistake in interviews.
- **Amortized vs worst case:** The queue-from-stacks `pop` is O(n) worst case for a single call. Know when to say "amortized O(1)" and be ready to justify it with the "each element moves at most once" argument.
- **Queue-from-stacks is asked more often** than stack-from-queues in interviews. Prioritize memorizing it.
- **Two-queue variant for stack:** Push `x` into `q2`, drain `q1` into `q2`, then swap `q1` and `q2`. Same complexity, more code. The one-queue rotation is cleaner and preferred.
- **`pop()` never returns a value in STL** — always call `top()` before `pop()` when implementing custom wrappers.
