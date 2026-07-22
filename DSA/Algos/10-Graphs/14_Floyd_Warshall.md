# Floyd-Warshall Algorithm

## Introduction

Computes shortest paths between every pair of vertices in a weighted graph in O(V³). Handles negative edge weights and detects negative cycles.

---

## Intuition

The central observation: the shortest path from `i` to `j` either passes through some intermediate vertex `k`, or it does not. If we allow only vertices `{0, 1, ..., k}` as intermediates, then the shortest path from `i` to `j` is either the best path using only `{0..k-1}`, or it goes through `k` (shortest path from `i` to `k` + shortest path from `k` to `j`). By iterating `k` from 0 to V-1, we progressively allow more intermediates until all vertices are available — at which point we have all-pairs shortest paths.

---

## When to Use

- All-pairs shortest path on a small dense graph (V ≤ ~500).
- Negative edge weights are present (but no negative cycles).
- Detecting negative cycles globally.
- Transitive closure (reachability for all pairs).
- The graph is given as an adjacency matrix.

---

## Recognition Pattern

```
"Shortest path between ALL pairs"
"Can vertex i reach vertex j?"
"Dense graph, V ≤ 500"
"Negative weights but no negative cycles"
"Transitive closure"
```
→ Think Floyd-Warshall.

---

## Complexity Analysis

| Case    | Time  | Space  |
|---------|-------|--------|
| Best    | O(V³) | O(V²)  |
| Average | O(V³) | O(V²)  |
| Worst   | O(V³) | O(V²)  |

Space is O(V²) for the distance matrix (can be done in-place).

---

## Core Idea

Start with a distance matrix `dist[i][j]` initialized from the edge weights (direct connections). For each intermediate vertex `k`, update every pair `(i, j)`: if going from `i` to `j` via `k` is shorter than the current `dist[i][j]`, update it. After all `k` from 0 to V-1 are processed, `dist[i][j]` holds the shortest path between every pair.

---

## Visualization

```
Graph (adjacency matrix, INF = no direct edge):
    0    1    2
0 [ 0,   3, INF]
1 [INF,  0,  -2]
2 [ 4, INF,   0]

k=0 (allow vertex 0 as intermediate):
  dist[1][2] via 0: dist[1][0]+dist[0][2] = INF → no change
  dist[2][1] via 0: 4+3 = 7 → dist[2][1] = 7

k=1 (allow vertex 1):
  dist[0][2] via 1: 3+(-2) = 1 → dist[0][2] = 1
  dist[2][2] via 1: 7+(-2) = 5 → dist[2][2] = 5? No (diagonal stays 0)

k=2 (allow vertex 2):
  dist[0][0] via 2: 1+4 = 5 → 5 > 0, no change
  dist[1][0] via 2: (-2)+4 = 2 → dist[1][0] = 2

Final dist:
    0   1   2
0 [ 0,  3,  1]
1 [ 2,  0, -2]
2 [ 4,  7,  0]
```

---

## Critical Code Explanation

### The Triple Loop Order

```cpp
for (int k = 0; k < n; k++)         // intermediate vertex — MUST be outermost
    for (int i = 0; i < n; i++)     // source
        for (int j = 0; j < n; j++) // destination
            if (dist[i][k] != INF && dist[k][j] != INF)
                dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
```

**`k` must be the outermost loop.** The DP recurrence says: "allowing vertex k as an intermediate, update all pairs." If `k` were inner, you'd be using partially-updated paths with vertex k already incorporated, which breaks the recurrence.

### Overflow Guard

```cpp
if (dist[i][k] != INF && dist[k][j] != INF)
```

Without this guard, `INF + (-2)` overflows if `INF = INT_MAX`. Either use this guard or set `INF` to a large but not maximum value like `1e9`.

### Negative Cycle Detection

```cpp
for (int i = 0; i < n; i++)
    if (dist[i][i] < 0)
        // negative cycle exists — vertex i is on it
```

A negative cycle causes `dist[i][i] < 0` because you can reach `i` from itself with negative total weight.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

const int INF = 1e9;

// dist[i][j] must be pre-filled: 0 for i==j, edge weight for direct edges, INF otherwise
void floydWarshall(vector<vector<int>>& dist) {
    int n = dist.size();

    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (dist[i][k] < INF && dist[k][j] < INF)
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
}

// With negative cycle detection
bool hasNegativeCycle(vector<vector<int>>& dist) {
    floydWarshall(dist);
    int n = dist.size();
    for (int i = 0; i < n; i++)
        if (dist[i][i] < 0) return true;
    return false;
}

// Transitive closure (reachability)
void transitiveClosure(vector<vector<bool>>& reach) {
    int n = reach.size();
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                reach[i][j] = reach[i][j] || (reach[i][k] && reach[k][j]);
}
```

---

## Why It Works

Floyd-Warshall is a DP over subsets of intermediate vertices. Let `dp[k][i][j]` = shortest path from `i` to `j` using only vertices `{0..k}` as intermediates. The recurrence is `dp[k][i][j] = min(dp[k-1][i][j], dp[k-1][i][k] + dp[k-1][k][j])`. Since `dp[k][i][k] = dp[k-1][i][k]` and `dp[k][k][j] = dp[k-1][k][j]` (you can't make a path to/from `k` shorter by going through `k`), the update can be done in-place on a single 2D matrix.

---

## Important Notes

- Use `1e9` (not `INT_MAX`) as infinity to safely do `dist[i][k] + dist[k][j]` without integer overflow.
- Floyd-Warshall runs in O(V³) and is practical only for V ≤ ~500. For V = 1000, it requires ~1 billion operations.
- Unlike Dijkstra or Bellman-Ford, Floyd-Warshall works on both directed and undirected graphs naturally.
- **Does not work correctly in the presence of negative cycles** — the output is undefined if negative cycles exist. Always detect them before interpreting results.
- For all-pairs shortest paths on sparse graphs, running Dijkstra from every source is faster: O(V · (V+E) log V) vs O(V³).
