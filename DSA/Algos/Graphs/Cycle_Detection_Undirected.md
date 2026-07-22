# Cycle Detection — Undirected Graph

## Introduction

Detects whether an undirected graph contains a cycle using either DFS with parent tracking or Union-Find. Unlike directed graphs, any non-tree edge in an undirected graph indicates a cycle.

---

## Intuition

In an undirected DFS, every edge is either a tree edge (goes to an unvisited node) or a back edge (goes to a visited node). But every tree edge `u-v` appears in both directions — when at `u` looking at `v` and when at `v` looking at `u`. The edge back to the parent is not a cycle, it's just the tree edge revisited. A cycle exists only when DFS reaches a visited node that is **not** the immediate parent.

---

## When to Use

- Checking if an undirected graph is a tree (connected + no cycle).
- Network topology validation.
- Checking if adding an edge creates a cycle (DSU approach, online).

---

## Recognition Pattern

```
Undirected graph
+ "is it a tree?"
+ "does adding this edge create a cycle?"
+ "connected and acyclic?"
```
→ Think undirected cycle detection (DFS or DSU).

---

## Complexity Analysis

| Case    | Time     | Space |
|---------|----------|-------|
| Best    | O(V + E) | O(V)  |
| Average | O(V + E) | O(V)  |
| Worst   | O(V + E) | O(V)  |

DSU approach is also O(E · α(V)) ≈ O(E).

---

## Core Idea

**DFS:** Track the parent of each node. When visiting neighbors, skip the parent. If a visited neighbor is encountered that isn't the parent, a cycle exists.

**DSU:** For each edge `(u, v)`, check if `u` and `v` are already in the same component. If yes → adding this edge creates a cycle. If no → `unite(u, v)`.

---

## Visualization

```
Graph: 0 - 1 - 2
           |   |
           3 - 4 - 2  (edge 4-2 creates cycle 1-2-4-3-1)

DFS from 0 (parent tracking):
  visit(0, parent=-1) → visit(1, parent=0)
    → visit(3, parent=1)
      → visit(4, parent=3)
        → visit(2, parent=4)
          → sees 1 (visited, not parent 4) → CYCLE
```

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// DFS approach
bool dfsHasCycle(int u, int parent, vector<vector<int>>& adj, vector<bool>& vis) {
    vis[u] = true;
    for (int v : adj[u]) {
        if (!vis[v]) {
            if (dfsHasCycle(v, u, adj, vis)) return true;
        } else if (v != parent) {
            return true;   // visited node that isn't parent → cycle
        }
    }
    return false;
}

bool hasCycleUndirected(int n, vector<vector<int>>& adj) {
    vector<bool> vis(n, false);
    for (int i = 0; i < n; i++)
        if (!vis[i] && dfsHasCycle(i, -1, adj, vis))
            return true;
    return false;
}

// DSU approach
struct DSU {
    vector<int> parent, rank_;
    DSU(int n) : parent(n), rank_(n, 0) { iota(parent.begin(), parent.end(), 0); }
    int find(int x) { return parent[x] == x ? x : parent[x] = find(parent[x]); }
    bool unite(int x, int y) {
        x = find(x); y = find(y);
        if (x == y) return false;
        if (rank_[x] < rank_[y]) swap(x, y);
        parent[y] = x;
        if (rank_[x] == rank_[y]) rank_[x]++;
        return true;
    }
};

bool hasCycleDSU(int n, vector<pair<int,int>>& edges) {
    DSU dsu(n);
    for (auto [u, v] : edges)
        if (!dsu.unite(u, v)) return true;   // already connected → cycle
    return false;
}
```

---

## Why It Works

**DFS:** In an undirected graph, the only non-tree edges are back edges to ancestors. The parent edge is excluded explicitly. Any other visited node encountered is an ancestor that forms a cycle.

**DSU:** Edges are added one by one. If both endpoints already share a root (connected), the edge forms a cycle. Otherwise, merge the components.

---

## Important Notes

- **Multi-edges:** The parent check `v != parent` fails with parallel edges. Track parent by edge index, or use a `parentEdge` variable.
- The DFS approach with `visited` bool (two-color) is sufficient for undirected graphs — no need for three-color states like directed cycle detection.
- DSU is preferred for **online** cycle detection (adding edges one at a time) or when you don't have the full adjacency list upfront.
- A connected undirected graph with V nodes and V-1 edges is a tree (no cycles). V or more edges guarantee at least one cycle.
