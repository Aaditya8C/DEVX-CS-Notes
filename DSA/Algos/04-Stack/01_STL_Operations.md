# STL Stack, Queue, Deque & Priority Queue

## Introduction

A quick-reference cheatsheet for all stack-like STL containers in C++. Every competitive programming and interview problem involving LIFO, FIFO, or priority-ordered access uses one of these four.

---

## Intuition

Each container gives you a restricted view of a sequence. The restriction is what makes it efficient — by limiting access to one or two ends, you get O(1) operations. Choosing the right container comes down to: do you need LIFO (`stack`), FIFO (`queue`), both ends (`deque`), or always the max/min (`priority_queue`)?

---

## When to Use

| Container        | Use When                                                     |
|------------------|--------------------------------------------------------------|
| `stack`          | LIFO access — undo, backtracking, DFS, expression parsing    |
| `queue`          | FIFO access — BFS, task scheduling, level-order traversal    |
| `deque`          | Both ends, or when you need random access + fast front ops   |
| `priority_queue` | Always need max/min — Dijkstra, Prim's, greedy scheduling    |

---

## API Reference

### stack

```cpp
stack<int> st;

st.push(x);     // push to top
st.pop();       // remove top  (returns void)
st.top();       // peek top    (does NOT remove)
st.empty();     // true if empty
st.size();      // number of elements
```

### queue

```cpp
queue<int> q;

q.push(x);      // enqueue at back
q.pop();        // dequeue from front  (returns void)
q.front();      // peek front
q.back();       // peek back
q.empty();      // true if empty
q.size();       // number of elements
```

### deque

```cpp
deque<int> dq;

dq.push_back(x);    // insert at back
dq.push_front(x);   // insert at front
dq.pop_back();      // remove from back
dq.pop_front();     // remove from front
dq.front();         // peek front
dq.back();          // peek back
dq[i];              // random access — O(1)
dq.empty();
dq.size();
```

### priority_queue

```cpp
// Max-heap (default) — top() gives largest
priority_queue<int> maxpq;

// Min-heap — top() gives smallest
priority_queue<int, vector<int>, greater<int>> minpq;

// Min-heap on pairs (sorted by first element)
priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;

// Custom comparator (lambda not directly supported; use struct)
struct Cmp { bool operator()(int a, int b) { return a > b; } };
priority_queue<int, vector<int>, Cmp> custompq;

pq.push(x);     // insert
pq.pop();       // remove top  (returns void)
pq.top();       // peek max or min
pq.empty();
pq.size();
```

---

## Important Notes

- `pop()` on every STL container returns **void** — always call `top()` or `front()` before popping.
- `stack` and `queue` do **not** support iteration or random access.
- `deque` supports random access (`dq[i]`) unlike `stack` and `queue`.
- `priority_queue` is a **max-heap** by default. Forgetting `greater<T>` for min-heap is a very common bug.
- To iterate a `stack` or `queue`, you must pop elements one by one (destructive) — consider using `deque` directly if you need both iteration and LIFO/FIFO behavior.
- The underlying container for `stack` and `queue` is `deque` by default. You can change it: `stack<int, vector<int>> st;`
- `priority_queue` does **not** support `decrease-key` — use `lazy deletion` (push updated value, skip stale entries on pop) as a workaround.
