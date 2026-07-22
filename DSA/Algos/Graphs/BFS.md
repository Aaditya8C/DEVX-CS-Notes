# BFS — Breadth-First Search

## Introduction

BFS explores a graph level by level, visiting all neighbors of the current node before moving to the next level. It is the canonical algorithm for finding the shortest path in an unweighted graph.

---

## Intuition

The key observation is that a queue naturally enforces level-order processing. Every node is enqueued exactly when it is first discovered, and since the queue is FIFO, nodes discovered earlier (closer to the source) are always processed first. This means the first time BFS reaches any node, it has taken the fewest possible hops to get there — which is why BFS gives shortest paths in unweighted graphs.

---

## When to Use

- Shortest path in an unweighted graph (edges have equal cost).
- Level-order traversal of a tree or graph.
- Finding all nodes within distance k from a source.
- Multi-source shortest paths (start BFS from multiple sources simultaneously).
- Bipartite graph checking.
- Connected components in an undirected graph.

---

## Recognition Pattern

```
Unweighted graph
+ shortest path / minimum hops / minimum steps
+ "level by level" / "nearest" / "reachable in k moves"
```
→ Think BFS.

---

## Complexity Analysis

| Case    | Time     | Space    |
|---------|----------|----------|
| Best    | O(V + E) | O(V)     |
| Average | O(V + E) | O(V)     |
| Worst   | O(V + E) | O(V)     |

- V = vertices, E = edges.
- Space: O(V) for the visited array and the queue.

---

## Core Idea

Start from the source, mark it visited, and enqueue it. In each iteration, dequeue a node, process it, then enqueue all its unvisited neighbors while marking them visited immediately — not when dequeued. Marking on enqueue is critical: it prevents the same node from being enqueued multiple times.

---

## Visualization

```
Graph:  0 - 1 - 3
        |   |
        2   4

BFS from 0:

Queue: [0]          visited: {0}
  → process 0, enqueue 1, 2
Queue: [1, 2]       visited: {0,1,2}
  → process 1, enqueue 3, 4
Queue: [2, 3, 4]    visited: {0,1,2,3,4}
  → process 2 (no unvisited neighbors)
Queue: [3, 4]
  → process 3, process 4

Level order: 0 → 1, 2 → 3, 4
Distances:   0 → 1   → 2
```

---

## Critical Code Explanation

### Mark on Enqueue, Not on Dequeue

```cpp
if (!vis[v]) {
    vis[v] = true;   // mark HERE, not after dequeue
    q.push(v);
}
```

If you mark visited on dequeue instead of enqueue, the same node can be added to the queue multiple times (once by each of its neighbors that discovers it first). This causes O(E) enqueue operations and potentially wrong distance calculations.

### Distance Tracking

```cpp
dist[v] = dist[u] + 1;
```

Since BFS processes nodes level by level, when `v` is first discovered from `u`, `dist[u] + 1` is guaranteed to be the shortest distance to `v`.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Basic BFS — returns visited order
vector<int> bfs(int src, int n, vector<vector<int>>& adj) {
    vector<bool> vis(n, false);
    vector<int> order;
    queue<int> q;

    vis[src] = true;
    q.push(src);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        order.push_back(u);

        for (int v : adj[u]) {
            if (!vis[v]) {
                vis[v] = true;
                q.push(v);
            }
        }
    }
    return order;
}

// BFS with shortest distance from source
vector<int> bfsDist(int src, int n, vector<vector<int>>& adj) {
    vector<int> dist(n, -1);
    queue<int> q;

    dist[src] = 0;
    q.push(src);

    while (!q.empty()) {
        int u = q.front(); q.pop();

        for (int v : adj[u]) {
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                q.push(v);
            }
        }
    }
    return dist;  // dist[i] = -1 means unreachable
}

// BFS across all components (disconnected graph)
void bfsAll(int n, vector<vector<int>>& adj) {
    vector<bool> vis(n, false);
    for (int i = 0; i < n; i++) {
        if (!vis[i]) {
            // start a new BFS component from i
            queue<int> q;
            vis[i] = true;
            q.push(i);
            while (!q.empty()) {
                int u = q.front(); q.pop();
                for (int v : adj[u])
                    if (!vis[v]) { vis[v] = true; q.push(v); }
            }
        }
    }
}
```

---

## Why It Works

BFS maintains the invariant that the queue always contains nodes in non-decreasing order of their distance from the source. Nodes at distance d are fully processed before any node at distance d+1 is dequeued. Because each node is marked visited on enqueue, no node enters the queue more than once. Together, these two properties guarantee that the first time BFS reaches any node, it has taken the minimum number of edges to get there.

---

## Important Notes

- BFS does **not** give shortest paths when edges have different weights — use Dijkstra for that.
- For a disconnected graph, wrap BFS in a loop over all vertices to cover every component.
- `dist[v] == -1` as the initial value doubles as the "not visited" check, eliminating a separate boolean array.
- Multi-source BFS: push all source nodes into the queue at the start with `dist = 0`. The same algorithm then finds shortest distances from the nearest source to every node.
- BFS on a tree is simply level-order traversal.
