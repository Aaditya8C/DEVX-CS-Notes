# Kruskal's Algorithm

## Introduction

Builds a Minimum Spanning Tree (MST) by greedily adding the cheapest edge that does not form a cycle. Uses Disjoint Set Union (DSU) for efficient cycle detection.

---

## Intuition

Sort all edges by weight. Then consider each edge in order — if its two endpoints are already connected (in the same component), adding it would create a cycle, so skip it. Otherwise, add it to the MST and merge the two components. DSU answers "are these two nodes connected?" in near O(1), making the cycle check fast.

---

## When to Use

- Minimum Spanning Tree on a sparse or edge-list graph.
- Minimum cost to connect all nodes.
- Network design: cheapest way to connect all cities/nodes.
- When the graph is given as a list of edges (vs adjacency list).

---

## Recognition Pattern

```
Connect all vertices
+ minimum total cost
+ no cycles
"Minimum cost to connect all"
"Cheapest network"
```
→ Think Kruskal (or Prim's).

Kruskal is preferred when: graph is sparse, edges are already sorted, or edges are given as a list.
Prim's is preferred when: graph is dense, or adjacency list is given.

---

## Complexity Analysis

| Case    | Time               | Space |
|---------|--------------------|-------|
| Best    | O(E log E)         | O(V)  |
| Average | O(E log E)         | O(V)  |
| Worst   | O(E log E · α(V))  | O(V)  |

Dominated by the sort. DSU operations are nearly O(1) with path compression and union by rank. α(V) is the inverse Ackermann function — effectively constant.

---

## Core Idea

1. Sort all edges by weight (ascending).
2. Initialize DSU with V components.
3. For each edge `(u, v, w)` in sorted order: if `find(u) != find(v)` (different components), include the edge in MST and call `unite(u, v)`.
4. Stop when V-1 edges are added (MST is complete).

---

## Visualization

```
Vertices: 0, 1, 2, 3
Edges: (0-1, 1), (0-2, 3), (1-2, 2), (1-3, 4), (2-3, 5)

Sorted: (0-1,1), (1-2,2), (0-2,3), (1-3,4), (2-3,5)

DSU: [0,1,2,3]  (each node is its own parent)

Edge (0-1, w=1): find(0)=0, find(1)=1 → different → ADD, unite(0,1)
  DSU: [0,0,2,3]   MST weight = 1

Edge (1-2, w=2): find(1)=0, find(2)=2 → different → ADD, unite(0,2)
  DSU: [0,0,0,3]   MST weight = 3

Edge (0-2, w=3): find(0)=0, find(2)=0 → SAME → SKIP (cycle)

Edge (1-3, w=4): find(1)=0, find(3)=3 → different → ADD, unite(0,3)
  DSU: [0,0,0,0]   MST weight = 7

3 edges added = V-1 → MST complete
```

---

## Critical Code Explanation

### DSU with Path Compression

```cpp
int find(int x) {
    return parent[x] == x ? x : parent[x] = find(parent[x]);
}
```

Path compression flattens the tree during every `find` call — all nodes on the path are made to point directly to the root. This amortizes the cost to nearly O(1) per operation.

### Union by Rank

```cpp
void unite(int x, int y) {
    x = find(x); y = find(y);
    if (x == y) return;
    if (rank[x] < rank[y]) swap(x, y);
    parent[y] = x;
    if (rank[x] == rank[y]) rank[x]++;
}
```

Always attach the shorter tree under the taller one. This keeps the tree flat and ensures O(log V) height without path compression (O(α(V)) with it).

### Edge Tuple Format

```cpp
// Edges stored as (weight, u, v) — sorted by weight first
sort(edges.begin(), edges.end());
for (auto [w, u, v] : edges) { ... }
```

Storing `(w, u, v)` instead of `(u, v, w)` means `sort()` automatically sorts by weight without a custom comparator.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

struct DSU {
    vector<int> parent, rank_;

    DSU(int n) : parent(n), rank_(n, 0) {
        iota(parent.begin(), parent.end(), 0);
    }

    int find(int x) {
        return parent[x] == x ? x : parent[x] = find(parent[x]);
    }

    bool unite(int x, int y) {
        x = find(x); y = find(y);
        if (x == y) return false;   // already connected
        if (rank_[x] < rank_[y]) swap(x, y);
        parent[y] = x;
        if (rank_[x] == rank_[y]) rank_[x]++;
        return true;
    }

    bool connected(int x, int y) { return find(x) == find(y); }
};

// Returns MST weight; -1 if graph is disconnected
// edges: vector of (weight, u, v)
int kruskal(int n, vector<tuple<int,int,int>>& edges) {
    sort(edges.begin(), edges.end());
    DSU dsu(n);
    int totalWeight = 0, edgesAdded = 0;

    for (auto [w, u, v] : edges) {
        if (dsu.unite(u, v)) {
            totalWeight += w;
            edgesAdded++;
            if (edgesAdded == n - 1) break;   // MST complete
        }
    }

    return edgesAdded == n - 1 ? totalWeight : -1;
}
```

---

## Why It Works

The correctness follows from the **Cut Property**: for any cut of the graph into two non-empty sets S and V-S, the minimum weight edge crossing the cut is always part of some MST. Kruskal's algorithm, by processing edges in ascending weight order, always picks the globally cheapest edge that connects two previously disconnected components — which is exactly the minimum edge across the cut between those components. Therefore, every edge Kruskal selects is safe to include in the MST.

---

## Important Notes

- **MST exists iff the graph is connected.** If fewer than V-1 edges are added, the graph is disconnected and has no MST (it has a minimum spanning forest instead).
- Kruskal's produces a **minimum** spanning tree. For **maximum** spanning tree, sort edges in descending order.
- The `rank_` field is named with underscore to avoid conflict with `std::rank`.
- In competitive programming, it's common to see the DSU `find` written iteratively to avoid recursion depth issues on large inputs.
- For second-best MST or k-th MST problems, Kruskal's serves as the foundation but requires additional bookkeeping.
