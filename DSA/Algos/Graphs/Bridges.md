# Bridges (Tarjan's Bridge Finding)

## Introduction

A bridge is an edge whose removal disconnects the graph. Tarjan's algorithm finds all bridges in O(V + E) using DFS and the concept of "lowest reachable ancestor" (low values).

---

## Intuition

During DFS, every edge either goes to an unvisited node (tree edge) or to an already-visited ancestor (back edge). A back edge represents an alternative path — if there's a back edge that bypasses an edge `(u, v)`, then removing `(u, v)` doesn't disconnect the graph. The `low[v]` value tracks the earliest discovery time reachable from the subtree rooted at `v`. If `low[v] > tin[u]`, then the subtree rooted at `v` has no back edge reaching `u` or above — removing edge `(u, v)` isolates `v`'s subtree, making `(u, v)` a bridge.

---

## When to Use

- Finding critical edges in a network.
- Network reliability analysis.
- Problems asking "which edge removals disconnect the graph?"
- Biconnected components.

---

## Recognition Pattern

```
"Critical connections"
"Remove an edge that disconnects the network"
"Bridge edges"
"Network reliability"
```
→ Think Bridges (Tarjan's).

---

## Complexity Analysis

| Case    | Time     | Space |
|---------|----------|-------|
| Best    | O(V + E) | O(V)  |
| Average | O(V + E) | O(V)  |
| Worst   | O(V + E) | O(V)  |

---

## Core Idea

Run DFS, assigning each vertex a discovery time `tin[u]`. Also maintain `low[u]` = the minimum `tin` reachable from `u`'s subtree via at most one back edge. For each DFS tree edge `u → v`:
- After visiting `v`, update `low[u] = min(low[u], low[v])`.
- If `low[v] > tin[u]`, edge `(u, v)` is a bridge.

For back edges `u → ancestor`, update `low[u] = min(low[u], tin[ancestor])`.

---

## Visualization

```
Graph: 0 - 1 - 2 - 3
           |
           4

DFS from 0 (parent tracking):

visit(0): tin[0]=0, low[0]=0
  visit(1): tin[1]=1, low[1]=1
    visit(2): tin[2]=2, low[2]=2
      visit(3): tin[3]=3, low[3]=3
        no more neighbors
      finish(3): low[2]=min(2,low[3])=min(2,3)=2
      low[3]=3 > tin[2]=2 → BRIDGE: (2,3)
    visit(4): tin[4]=4, low[4]=4
      no more neighbors
    finish(4): low[1]=min(1,low[4])=min(1,4)=1
    low[4]=4 > tin[1]=1 → BRIDGE: (1,4)
  finish(2): low[1]=min(1,low[2])=min(1,2)=1
  low[2]=2 > tin[1]=1 → BRIDGE: (1,2)
finish(1): low[0]=min(0,low[1])=min(0,1)=0
low[1]=1 > tin[0]=0 → BRIDGE: (0,1)

Bridges: (0,1), (1,2), (2,3), (1,4)
```

---

## Critical Code Explanation

### Discovery Time and Low Value

```cpp
tin[u] = low[u] = timer++;
```

Both are initialized to the same value. `tin[u]` is fixed; `low[u]` gets updated as DFS progresses — it shrinks whenever a back edge to an earlier-discovered node is found.

### Back Edge vs Tree Edge

```cpp
if (v == parent) continue;       // skip the edge back to parent in undirected graph
if (vis[v]) low[u] = min(low[u], tin[v]);   // back edge: use tin, not low[v]
```

For back edges, use `tin[v]` (discovery time), not `low[v]`. Using `low[v]` here would incorrectly propagate low values through back edges to nodes that aren't true ancestors. For tree edges, propagate `low[v]` after the recursive call.

### Bridge Condition

```cpp
if (low[v] > tin[u])
    bridges.push_back({u, v});
```

`low[v] > tin[u]` means: from `v`'s entire subtree, the earliest reachable node is `v` itself (or below). There is no back edge reaching `u` or any of `u`'s ancestors. Removing `(u, v)` would disconnect `v`'s subtree.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

int timer_val = 0;

void dfs(int u, int parent, vector<vector<int>>& adj,
         vector<int>& tin, vector<int>& low, vector<bool>& vis,
         vector<pair<int,int>>& bridges) {

    vis[u] = true;
    tin[u] = low[u] = timer_val++;

    for (int v : adj[u]) {
        if (v == parent) continue;
        if (vis[v]) {
            low[u] = min(low[u], tin[v]);   // back edge
        } else {
            dfs(v, u, adj, tin, low, vis, bridges);
            low[u] = min(low[u], low[v]);   // tree edge
            if (low[v] > tin[u])
                bridges.push_back({u, v});
        }
    }
}

vector<pair<int,int>> findBridges(int n, vector<vector<int>>& adj) {
    vector<int> tin(n, -1), low(n, -1);
    vector<bool> vis(n, false);
    vector<pair<int,int>> bridges;
    timer_val = 0;

    for (int i = 0; i < n; i++)
        if (!vis[i]) dfs(i, -1, adj, tin, low, vis, bridges);

    return bridges;
}
```

---

## Why It Works

The `low[v]` value represents the earliest point in DFS time that the subtree rooted at `v` can reach. If this earliest point is strictly later than `tin[u]` (the discovery time of `v`'s parent), then `v`'s subtree has no "escape route" back to `u` or above — the edge `(u, v)` is the only connection, making it a bridge.

---

## Important Notes

- **Multi-edges (parallel edges):** In graphs with multiple edges between the same pair of nodes, the `v == parent` check incorrectly skips both parallel edges. Track parent by edge index instead of node id.
- Bridges form a subset of tree edges — a back edge is never a bridge.
- **Articulation points** use the same DFS with slightly different conditions (`low[v] >= tin[u]` for non-root nodes, and a child-count check for the root).
- The `timer_val` global must be reset between calls on different graphs.
- LeetCode 1192 "Critical Connections in a Network" is the exact bridge-finding problem.
