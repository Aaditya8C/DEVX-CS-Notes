# Topological Sort — Kahn's Algorithm (BFS-based)

## Introduction

Produces a linear ordering of vertices in a Directed Acyclic Graph (DAG) such that for every directed edge `u → v`, vertex `u` appears before `v` in the ordering. Kahn's algorithm does this with BFS using in-degree tracking.

---

## Intuition

A vertex can be placed in the topological order only when all its prerequisites (incoming edges) are already placed. Vertices with zero in-degree have no prerequisites — they can go first. After placing them, their outgoing edges are removed, potentially bringing new vertices to zero in-degree. This is exactly BFS: process all zero-in-degree vertices level by level. If the graph has a cycle, some vertices will never reach zero in-degree and won't appear in the output — which is how you detect cycles.

---

## When to Use

- Linear ordering of tasks with prerequisites.
- Detecting cycles in a directed graph.
- Build systems, dependency resolution, course scheduling.
- Shortest/longest path in a DAG (process in topological order).
- Any problem with "must do X before Y" constraints.

---

## Recognition Pattern

```
Directed graph
+ "ordering with dependencies"
+ "course prerequisites"
+ "task scheduling"
+ "can all tasks be completed?" (cycle detection)
+ DAG processing
```
→ Think Topological Sort (Kahn's).

---

## Complexity Analysis

| Case    | Time     | Space |
|---------|----------|-------|
| Best    | O(V + E) | O(V)  |
| Average | O(V + E) | O(V)  |
| Worst   | O(V + E) | O(V)  |

---

## Core Idea

1. Compute in-degree for every vertex.
2. Push all zero-in-degree vertices into a queue.
3. While the queue is non-empty: dequeue `u`, add to result, and for each neighbor `v` decrement `inDeg[v]` by 1. If `inDeg[v]` becomes 0, push `v` to the queue.
4. If the result contains all V vertices, a valid topological order exists. If fewer than V vertices appear, the graph has a cycle.

---

## Visualization

```
Graph: 0 → 1 → 3
       0 → 2 → 3
       2 → 4

In-degrees: [0, 1, 1, 2, 1]

Queue: [0]   (only vertex with in-degree 0)

Pop 0 → result=[0]
  neighbor 1: inDeg[1] = 0 → enqueue
  neighbor 2: inDeg[2] = 0 → enqueue
Queue: [1, 2]

Pop 1 → result=[0,1]
  neighbor 3: inDeg[3] = 1
Queue: [2]

Pop 2 → result=[0,1,2]
  neighbor 3: inDeg[3] = 0 → enqueue
  neighbor 4: inDeg[4] = 0 → enqueue
Queue: [3, 4]

Pop 3 → result=[0,1,2,3]
Pop 4 → result=[0,1,2,3,4]

All 5 vertices processed → valid topological order: 0,1,2,3,4
```

---

## Critical Code Explanation

### In-Degree Computation

```cpp
vector<int> inDeg(n, 0);
for (int u = 0; u < n; u++)
    for (int v : adj[u])
        inDeg[v]++;
```

Every edge `u → v` contributes one to `v`'s in-degree. This must be computed from the adjacency list before starting BFS.

### Cycle Detection via Count

```cpp
if ((int)result.size() != n)
    // graph has a cycle — not all vertices were processed
```

If a cycle exists, the vertices in the cycle can never reach in-degree 0 (they all depend on each other). They never enter the queue, so the result will be smaller than n.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Returns topological order; empty if cycle exists
vector<int> topoSortKahns(int n, vector<vector<int>>& adj) {
    vector<int> inDeg(n, 0);
    for (int u = 0; u < n; u++)
        for (int v : adj[u])
            inDeg[v]++;

    queue<int> q;
    for (int i = 0; i < n; i++)
        if (inDeg[i] == 0) q.push(i);

    vector<int> result;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        result.push_back(u);

        for (int v : adj[u])
            if (--inDeg[v] == 0) q.push(v);
    }

    if ((int)result.size() != n)
        return {};   // cycle detected

    return result;
}

// Check if directed graph has a cycle
bool hasCycle(int n, vector<vector<int>>& adj) {
    return topoSortKahns(n, adj).empty();
}
```

---

## Why It Works

A vertex enters the queue only when all its predecessors have been removed (in-degree reaches 0). This means when `u` is placed in the result, every vertex that should come before `u` is already in the result — exactly the topological order property. If a cycle exists, the vertices in the cycle keep each other's in-degrees above 0 permanently, preventing them from ever being enqueued.

---

## Important Notes

- Topological sort is only defined for **DAGs**. If the graph has a cycle, no valid ordering exists.
- The ordering is **not unique** — multiple valid topological orderings may exist. Kahn's gives one of them (BFS order from zero-in-degree vertices).
- For **lexicographically smallest** topological order, use a min-priority queue instead of a regular queue.
- Cycle detection via Kahn's is often simpler to implement than the DFS-based approach using visited states.
- The DFS-based topological sort (post-order reversal) is an alternative — both give valid orderings.
- Longest path in a DAG: process vertices in topological order and relax outgoing edges (DP). This is efficient because no vertex is processed before all its predecessors.
