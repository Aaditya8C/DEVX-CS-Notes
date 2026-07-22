# STL Stack, Queue, Deque, Priority Queue

## Purpose

> Quick reference for all stack-like STL containers in C++.

---

## Stack

```cpp
stack<int> st;

st.push(x);        // push element
st.pop();          // remove top (no return)
st.top();          // peek top
st.empty();        // true if empty
st.size();         // number of elements
```

---

## Queue

```cpp
queue<int> q;

q.push(x);         // enqueue (back)
q.pop();           // dequeue (front, no return)
q.front();         // peek front
q.back();          // peek back
q.empty();         // true if empty
q.size();          // number of elements
```

---

## Deque (Double-Ended Queue)

```cpp
deque<int> dq;

dq.push_back(x);   // insert at back
dq.push_front(x);  // insert at front
dq.pop_back();     // remove from back
dq.pop_front();    // remove from front
dq.front();        // peek front
dq.back();         // peek back
dq.empty();        // true if empty
dq.size();         // number of elements
dq[i];             // random access by index
```

---

## Priority Queue

```cpp
// Max-heap (default)
priority_queue<int> pq;

// Min-heap
priority_queue<int, vector<int>, greater<int>> minpq;

// Custom comparator (min-heap on pair by first element)
priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq2;

pq.push(x);        // insert
pq.pop();          // remove top
pq.top();          // peek max/min
pq.empty();        // true if empty
pq.size();         // number of elements
```

---

## Notes

- `stack`, `queue` do NOT support random access or iteration.
- `deque` supports O(1) push/pop at both ends and O(1) random access.
- `priority_queue` is a max-heap by default; use `greater<T>` for min-heap.
- `pop()` on all containers returns `void` — always `top()`/`front()` before popping.
- Underlying container for `stack` and `queue` is `deque` by default.
