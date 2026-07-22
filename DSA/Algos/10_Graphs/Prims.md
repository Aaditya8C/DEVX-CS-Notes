# Prim's Algorithm

## Introduction

Builds a Minimum Spanning Tree (MST) by greedily growing a connected tree from a starting vertex, always adding the cheapest edge that connects the current tree to an unvisited vertex.

---

## Intuition

Start with any vertex. At each step, look at all edges crossing the boundary between the current tree and unvisited vertices, and pick the cheapest one. Add its endpoint to the tree and repeat. A min-heap makes the cheapest boundary edge extraction efficient.

---

## When to Use

- Minimum Spanning Tree on a **dense** graph (adjacency matrix, E ≈ V²).
- Graph given as adjacency list and you want to grow the MST incrementally.
- Same problem class as Kruskal — prefer Prim's for dense graphs.

---

## Recognition Pattern

Same as Kruskal:
```
"Minimum cost to connect all vertices"
"Minimum spanning tree"
```
→ Prim's for dense graphs or adjacency list. Kruskal's for sparse graphs or edge lists.

---

## Complexity Analysis

| Implementation          | Time           | Space |
|-------------------------|----------------|-------|
| Binary heap + adj list  | O(E log V)     | O(V)  |
| Adjacency matrix (no heap) | O(V²)       | O(V)  |

For dense graphs (E ≈ V²), the V² implementation is equivalent and avoids heap overhead.

---

## Core Idea

Maintain a `dist[]` array: `dist[v]` = cheapest edge weight connecting `v` to the current MST. Initialize `dist[src] = 0`, all others `∞`. Use a min-heap. While unvisited vertices remain: extract the minimum-`dist` vertex `u`, mark it visited, add `dist[u]` to MST weight, then relax all edges from `u`: for each neighbor `v`, if `w < dist[v]`, update `dist[v]` and push to heap.

---

## Visualization

```
Graph: 0-1(2), 0-3(6), 1-2(3), 1-3(8), 1-4(5), 2-4(7), 3-4(9)
src=0

heap: [(0,0)]   dist=[0,∞,∞,∞,∞]

Pop(0,0): add to MST, relax neighbors
  dist[1]=2, dist[3]=6
heap: [(2,1),(6,3)]

Pop(2,1): MST weight+=2, relax
  dist[2]=3, dist[3]=min(6,8)=6, dist[4]=5
heap: [(3,2),(5,4),(6,3)]

Pop(3,2): MST weight+=3
  dist[4]=min(5,7)=5
heap: [(5,4),(6,3)]

Pop(5,4): MST weight+=5
  dist[3]=min(6,9)=6
heap: [(6,3)]

Pop(6,3): MST weight+=6

Total MST weight = 0+2+3+5+6 = 16
```

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

int prims(int src, int n, vector<vector<pair<int,int>>>& adj) {
    vector<int> dist(n, INT_MAX);
    vector<bool> inMST(n, false);
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;

    dist[src] = 0;
    pq.push({0, src});
    int mstWeight = 0;

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();

        if (inMST[u]) continue;   // stale or already in MST
        inMST[u] = true;
        mstWeight += d;

        for (auto [v, w] : adj[u]) {
            if (!inMST[v] && w < dist[v]) {
                dist[v] = w;
                pq.push({w, v});
            }
        }
    }
    return mstWeight;
}
```

---

## Why It Works

Prim's correctness also follows from the **Cut Property**: at every step, the algorithm picks the cheapest edge crossing the cut between the current MST tree and the rest of the graph. By the cut property, this edge is always safe to include in an MST.

---

## Important Notes

- `dist[v]` stores the **edge weight** to connect `v` to the MST, not the path distance from the source (unlike Dijkstra which stores total path distance).
- The stale entry check `if (inMST[u]) continue` is analogous to Dijkstra's `d > dist[u]` check.
- For dense graphs (V ≤ 1000), implement without a heap: scan all vertices to find the minimum `dist` each time — O(V²) total, often faster in practice.
- Prim's and Kruskal's always produce the same MST weight, but may produce different MST edge sets when there are ties.
