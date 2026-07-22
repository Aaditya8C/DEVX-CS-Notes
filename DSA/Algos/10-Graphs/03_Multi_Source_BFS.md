# Multi-Source BFS & 0-1 BFS

## Introduction

Two BFS variants for specialized shortest-path problems. Multi-source BFS starts from multiple sources simultaneously and finds minimum distances from any source. 0-1 BFS handles graphs with edge weights of only 0 or 1, finding shortest paths in O(V+E) using a deque.

---

## Intuition

**Multi-Source BFS:** Instead of running BFS from each source separately (O(k·(V+E))), seed the queue with all sources at distance 0 and run a single BFS pass — O(V+E). Every node is assigned the distance to its nearest source automatically.

**0-1 BFS:** Standard BFS uses a queue (FIFO) and works for unit-weight edges. For 0-weight edges, the destination should have the same distance — it should go to the **front** of the queue, not the back. A deque achieves this: push to front for 0-weight edges, push to back for 1-weight edges. This is Dijkstra with a deque instead of a heap.

---

## When to Use

**Multi-Source BFS:**
- Distance to the nearest gate/source/land/node.
- Spread problems: fire/virus spread from multiple starting points.
- "Fill from all boundary cells."
- Nearest 0 to every cell in a binary matrix.

**0-1 BFS:**
- Weighted graph where all edge weights are 0 or 1.
- Sliding puzzle, grid with "free" and "cost-1" transitions.
- Problems with "pass through or skip" type costs.

---

## Recognition Pattern

```
Multi-Source BFS:
"Distance to nearest [X]"
"Multiple starting points"
"Spread from multiple sources simultaneously"

0-1 BFS:
"Shortest path, edge weight is 0 or 1"
"Grid with free moves and cost-1 moves"
```

---

## Complexity Analysis

| Algorithm         | Time     | Space |
|-------------------|----------|-------|
| Multi-Source BFS  | O(V + E) | O(V)  |
| 0-1 BFS (deque)   | O(V + E) | O(V)  |

Both are linear — equivalent to BFS/Dijkstra on unit-weight graphs.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Multi-Source BFS — minimum distance from any source
// sources: initial set of nodes at distance 0
vector<int> multiSourceBFS(int n, vector<vector<int>>& adj, vector<int>& sources) {
    vector<int> dist(n, INT_MAX);
    queue<int> q;

    for (int s : sources) {
        dist[s] = 0;
        q.push(s);
    }

    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : adj[u]) {
            if (dist[v] == INT_MAX) {
                dist[v] = dist[u] + 1;
                q.push(v);
            }
        }
    }
    return dist;
}

// Grid multi-source BFS example: distance to nearest 0 in binary matrix
vector<vector<int>> nearestZero(vector<vector<int>>& mat) {
    int m = mat.size(), n = mat[0].size();
    vector<vector<int>> dist(m, vector<int>(n, INT_MAX));
    queue<pair<int,int>> q;

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            if (mat[i][j] == 0) { dist[i][j] = 0; q.push({i, j}); }

    int dirs[4][2] = {{0,1},{0,-1},{1,0},{-1,0}};
    while (!q.empty()) {
        auto [r, c] = q.front(); q.pop();
        for (auto& d : dirs) {
            int nr = r + d[0], nc = c + d[1];
            if (nr >= 0 && nr < m && nc >= 0 && nc < n && dist[nr][nc] == INT_MAX) {
                dist[nr][nc] = dist[r][c] + 1;
                q.push({nr, nc});
            }
        }
    }
    return dist;
}

// 0-1 BFS — shortest path with edge weights 0 or 1
vector<int> bfs01(int src, int n, vector<vector<pair<int,int>>>& adj) {
    vector<int> dist(n, INT_MAX);
    deque<int> dq;
    dist[src] = 0;
    dq.push_back(src);

    while (!dq.empty()) {
        int u = dq.front(); dq.pop_front();
        for (auto [v, w] : adj[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                if (w == 0) dq.push_front(v);   // free edge → front
                else        dq.push_back(v);    // cost-1 edge → back
            }
        }
    }
    return dist;
}
```

---

## Important Notes

- **Multi-Source BFS:** Seed all sources at distance 0 before the loop — do not run BFS separately from each source. The single-pass version processes each node at most once.
- **0-1 BFS stale check:** Unlike Dijkstra, a `dist[v]` in 0-1 BFS might be updated multiple times. The `dist[u] + w < dist[v]` check ensures only improvements trigger updates.
- **0-1 BFS vs Dijkstra:** For 0/1 weights, 0-1 BFS with a deque is O(V+E). Dijkstra with a heap is O((V+E) log V). Use 0-1 BFS when weights are strictly 0 or 1.
- Multi-source BFS is a direct optimization over "run BFS from each source and take min" — avoids O(k) factor.
- 0-1 BFS naturally extends to k-value BFS using a bucket queue (k queues, one per cost level) — useful when edge weights are small integers.
