# Stack Using Queues / Queue Using Stacks

## Purpose

> Implements a stack using queues (or a queue using stacks) — a design/interview problem testing understanding of both data structures.

---

## When to Use

- System design / OOP interview questions.
- Understanding amortized cost.

---

## Time Complexity

### Stack Using Two Queues (push-costly)

| Operation | Complexity |
|-----------|------------|
| push      | O(n)       |
| pop       | O(1)       |
| top       | O(1)       |

### Stack Using One Queue (push-costly)

| Operation | Complexity |
|-----------|------------|
| push      | O(n)       |
| pop       | O(1)       |
| top       | O(1)       |

### Queue Using Two Stacks (pop-costly)

| Operation | Complexity (amortized) |
|-----------|------------------------|
| enqueue   | O(1)                   |
| dequeue   | O(1) amortized         |

**Space Complexity:** O(n) in all cases.

---

## Core Idea

### Stack Using Two Queues

- On `push(x)`: enqueue `x` to `q2`, then move all elements from `q1` to `q2`, then swap `q1` and `q2`.
- `q1` always has elements in LIFO order — front of `q1` = top of stack.
- `pop()` and `top()` just use `q1.front()`.

### Stack Using One Queue

- On `push(x)`: enqueue `x`, then rotate the queue `(size-1)` times — the new element becomes the front.
- `pop()` and `top()` use `q.front()`.

### Queue Using Two Stacks

- `inbox` stack for pushes, `outbox` stack for pops.
- On `dequeue`: if `outbox` is empty, move all from `inbox` to `outbox` (this reverses order = FIFO).
- Each element moves at most twice total → O(1) amortized.

---

## Critical Code Walkthrough

### Queue Using Two Stacks — Lazy Transfer

```cpp
int dequeue() {
    if (outbox.empty()) {
        while (!inbox.empty()) {
            outbox.push(inbox.top());
            inbox.pop();
        }
    }
    int val = outbox.top();
    outbox.pop();
    return val;
}
```

Transfer only happens when `outbox` is exhausted. Each element is transferred exactly once → amortized O(1).

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Stack using single queue
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

    int top() { return q.front(); }

    bool empty() { return q.empty(); }
};

// Queue using two stacks (pop-costly variant)
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

## Notes

- Stack from queue: rotating the queue `(size-1)` times after each push keeps the newest element at front.
- Queue from two stacks: never transfer while `outbox` still has elements — this preserves FIFO order.
- Amortized O(1) for queue-from-stacks dequeue: each element moves from `inbox` to `outbox` exactly once.
- In interviews, the queue-using-two-stacks variant is more commonly asked.
