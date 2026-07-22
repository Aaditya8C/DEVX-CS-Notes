# Bellman-Ford Algorithm

## Introduction

Finds the shortest path from a single source to all other vertices, handling graphs with negative edge weights. Also detects negative weight cycles, which Dijkstra cannot.

---

## Intuition

The key observation: in any graph with V vertices and no negative cycles, the shortest path between any two vertices uses at most V-1 edges. Bellman-Ford exploits this by relaxing all edges V-1 times. After the k-th round, the shortest paths using at most k edges are correctly computed. After V-1 rounds, all shortest paths are finalized — unless a negative cycle exists, in which case a V-th round would still improve some distances.

---

## When to Use

- Single-source shortest path with negative edge weights.
- Detecting negative weight cycles.
- Graph is given as an edge list rather than adjacency list.
- Arbitrage detection in currency exchange problems.

---

## Recognition Pattern

```
Shortest path
+ negative edge weights present
+ "detect negative cycle"
+ edge list input
+ arbitrage / currency exchange
```
→ Think Bellman-Ford.

---

## Complexity Analysis

| Case    | Time    | Space |
|---------|---------|-------|
| Best    | O(V·E)  | O(V)  |
| Average | O(V·E)  | O(V)  |
| Worst   | O(V·E)  | O(V)  |

No best-case optimization without early termination. Space is O(V) for the distance array.

---

## Core Idea

Initialize `dist[src] = 0`, all others `∞`. Run V-1 rounds. In each round, iterate over every edge `(u, v, w)` and relax: if `dist[u] + w < dist[v]`, update `dist[v]`. After V-1 rounds, run one more round — if any distance still improves, a negative cycle is reachable from the source.

---

## Visualization

```
Graph: 0 --(-1)--> 1 --(4)--> 3
       |                      ↑
       (2)         (3)        |
       ↓           ↓          |
       2 --(3)--> 1 (-2)---> 3

Simplified: edges = {(0,1,-1), (0,2,2), (1,2,3), (1,3,4), (2,3,-2)}

src=0, dist = [0, INF, INF, INF]

Round 1 (relax all edges once):
  (0,1,-1): dist[1] = 0+(-1) = -1
  (0,2, 2): dist[2] = 0+2 = 2
  (1,2, 3): dist[2] = min(2, -1+3) = 2
  (1,3, 4): dist[3] = -1+4 = 3
  (2,3,-2): dist[3] = min(3, 2+(-2)) = 0
  dist = [0, -1, 2, 0]

Round 2: no improvements → already converged

Final dist = [0, -1, 2, 0]
```

---

## Critical Code Explanation

### The Relaxation Loop

```cpp
for (int i = 0; i < n - 1; i++)           // V-1 rounds
    for (auto [u, v, w] : edges)           // every edge
        if (dist[u] != INT_MAX && dist[u] + w < dist[v])
            dist[v] = dist[u] + w;
```

The `dist[u] != INT_MAX` guard prevents overflow when adding `w` to an unreachable vertex. Without it, `INT_MAX + (-1)` wraps around and produces a garbage value that passes the comparison.

### Negative Cycle Detection

```cpp
for (auto [u, v, w] : edges)
    if (dist[u] != INT_MAX && dist[u] + w < dist[v])
        return true;   // negative cycle exists
```

After V-1 rounds, shortest paths are finalized unless there is a negative cycle. A V-th relaxation pass that still improves distances confirms a cycle — distances in a negative cycle can be reduced infinitely, so they never stabilize.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Returns shortest distances; detects negative cycles
// edges: vector of (u, v, w)
pair<vector<int>, bool> bellmanFord(int src, int n, vector<tuple<int,int,int>>& edges) {
    vector<int> dist(n, INT_MAX);
    dist[src] = 0;

    // V-1 relaxation rounds
    for (int i = 0; i < n - 1; i++) {
        bool updated = false;
        for (auto [u, v, w] : edges) {
            if (dist[u] != INT_MAX && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                updated = true;
            }
        }
        if (!updated) break;   // early termination
    }

    // Check for negative cycle (V-th round)
    bool hasNegCycle = false;
    for (auto [u, v, w] : edges) {
        if (dist[u] != INT_MAX && dist[u] + w < dist[v]) {
            hasNegCycle = true;
            break;
        }
    }

    return {dist, hasNegCycle};
}
```

---

## Why It Works

After k rounds of relaxation, `dist[v]` holds the shortest distance from `src` to `v` using at most k edges. This is proved by induction: in round 1, all single-edge shortest paths are found. In round k, all k-edge shortest paths are found by relaxing through nodes whose (k-1)-edge shortest paths were already finalized in the previous round. Since the shortest path in a V-node graph has at most V-1 edges (with no negative cycles), V-1 rounds suffice.

---

## Important Notes

- **Negative cycles make shortest paths undefined** — if a negative cycle is reachable from the source, you can keep traversing it to decrease the path length without bound.
- Bellman-Ford is **O(V·E)**, significantly slower than Dijkstra's O((V+E) log V). Use it only when negative weights are present.
- Early termination (`if (!updated) break`) turns the best case into O(E) when the graph has short relaxation chains.
- The edge order matters for early termination but **not** for correctness — any ordering of edges works given V-1 rounds.
- For detecting whether a specific vertex is on a negative cycle (not just reachable from one), track which vertices were updated in the V-th round and then do DFS from them.
- In competitive programming, Bellman-Ford on dense graphs is sometimes too slow. SPFA (Shortest Path Faster Algorithm) is an optimized queue-based variant that is faster in practice but has the same worst-case complexity.
