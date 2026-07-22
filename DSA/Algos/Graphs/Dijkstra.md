# Dijkstra's Algorithm

## Introduction

Finds the shortest path from a single source to every other vertex in a weighted graph with non-negative edge weights. It is the go-to algorithm whenever you see "minimum cost path" on a graph with positive weights.

---

## Intuition

Maintain a set of "finalized" vertices whose shortest distance from the source is known. At each step, greedily pick the unfinalized vertex with the smallest known distance, finalize it, and relax all its outgoing edges. This greedy choice is safe because, with non-negative edge weights, you can never find a shorter path to a finalized vertex by going through a later vertex. A min-heap makes the greedy extraction efficient.

---

## When to Use

- Single-source shortest path in a weighted graph.
- All edge weights are non-negative.
- Minimum cost to reach a node from a source.
- Grid shortest path with varying step costs.

---

## Recognition Pattern

```
Weighted graph
+ shortest path from one source
+ all weights ≥ 0
+ "minimum cost", "minimum distance", "cheapest path"
```
→ Think Dijkstra.

---

## Complexity Analysis

| Implementation  | Time           | Space |
|-----------------|----------------|-------|
| Binary heap PQ  | O((V+E) log V) | O(V)  |
| Fibonacci heap  | O(E + V log V) | O(V)  |

Binary heap with lazy deletion (standard competitive programming approach) is O((V+E) log V). Space: O(V) for `dist[]` + O(E) for the PQ in worst case.

---

## Core Idea

Initialize `dist[src] = 0`, all others `INT_MAX`. Push `(0, src)` into a min-heap. While the heap is non-empty: extract the minimum-distance node `u`. If `dist[u]` has already been improved since `u` was pushed (stale entry), skip. Otherwise, relax all edges `(u, v, w)`: if `dist[u] + w < dist[v]`, update `dist[v]` and push `(dist[v], v)` into the heap.

---

## Visualization

```
Graph: 0 --1-- 1 --4-- 3
       |       |
       2       2
       |       |
       2 --3-- 3

Edge list: (0,1,1), (0,2,2), (1,3,4), (1,2,2), (2,3,3)

src = 0
dist = [0, INF, INF, INF]

Step 1: pop (0,0), relax neighbors
  dist[1] = 1, push (1,1)
  dist[2] = 2, push (2,2)
  dist = [0, 1, 2, INF]

Step 2: pop (1,1), relax neighbors
  dist[2]: 1+2=3 > 2, no update
  dist[3]: 1+4=5, push (5,3)
  dist = [0, 1, 2, 5]

Step 3: pop (2,2), relax neighbors
  dist[3]: 2+3=5 = 5, no update
  dist = [0, 1, 2, 5]

Step 4: pop (5,3), stale check: dist[3]=5 = 5, process
  no unprocessed neighbors

Final dist = [0, 1, 2, 5]
```

---

## Critical Code Explanation

### Stale Entry Check

```cpp
auto [d, u] = pq.top(); pq.pop();
if (d > dist[u]) continue;   // stale — a better path was already found
```

Dijkstra with a binary heap uses **lazy deletion**: when a node's distance is improved, the old entry in the heap is not removed — a new, better entry is pushed instead. When the old entry is later popped, `d > dist[u]` reveals it's stale and should be ignored. This avoids implementing a decrease-key operation.

### Relaxation

```cpp
for (auto [v, w] : adj[u]) {
    if (dist[u] + w < dist[v]) {
        dist[v] = dist[u] + w;
        pq.push({dist[v], v});
    }
}
```

This is the core of Dijkstra. When a shorter path to `v` is found via `u`, update `dist[v]` and push the new `(distance, vertex)` pair. The heap will serve the best known distance first.

### Min-Heap Declaration

```cpp
priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;
```

`greater<>` makes it a min-heap so the smallest distance is always at the top. The default `priority_queue` is a max-heap and will give wrong answers.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

vector<int> dijkstra(int src, int n, vector<vector<pair<int,int>>>& adj) {
    vector<int> dist(n, INT_MAX);
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;

    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();

        if (d > dist[u]) continue;   // stale entry

        for (auto [v, w] : adj[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }
    return dist;   // dist[i] = INT_MAX means unreachable
}

// With path reconstruction
vector<int> dijkstraPath(int src, int dst, int n, vector<vector<pair<int,int>>>& adj) {
    vector<int> dist(n, INT_MAX), parent(n, -1);
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<>> pq;

    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;

        for (auto [v, w] : adj[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    // Reconstruct path from src to dst
    vector<int> path;
    for (int v = dst; v != -1; v = parent[v])
        path.push_back(v);
    reverse(path.begin(), path.end());
    return path;
}
```

---

## Why It Works

The greedy invariant: when a node `u` is popped from the min-heap with distance `d`, `d` is the true shortest distance from `src` to `u`. This holds because all edge weights are non-negative — any path that goes through an unfinalized node must be at least as long (since unfinalized nodes have distances ≥ the current minimum). Therefore, no future relaxation can improve the distance to `u` once it's popped.

---

## Important Notes

- **Does not work with negative edge weights.** A negative edge can create a shorter path to an already-finalized node, breaking the greedy invariant. Use Bellman-Ford for negative weights.
- The stale entry check `d > dist[u]` is critical — forgetting it causes redundant relaxations and incorrect results in some implementations.
- For dense graphs (E ≈ V²), an adjacency matrix + linear scan for minimum is O(V²), which can be faster than the heap-based O(E log V).
- To find shortest distances from a single source to **all** destinations, Dijkstra is more efficient than running it once per pair. For all-pairs shortest paths, use Floyd-Warshall.
- On a grid with unit costs, BFS is equivalent to Dijkstra and faster by a constant factor.
