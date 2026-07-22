# Cycle Detection — Directed Graph

## Introduction

Detects whether a directed graph contains a cycle. Both DFS (three-color) and Kahn's algorithm (in-degree) can detect cycles as a side effect of topological sort.

---

## Intuition

In a directed graph, a cycle exists when DFS reaches a node that is currently on the active DFS call stack (a "grey" node). This is different from undirected graphs, where any non-tree edge is a cycle — in directed graphs, only back edges (to ancestors on the current path) form cycles; forward and cross edges do not.

---

## When to Use

- Validating that task dependencies have no circular references.
- Checking if a topological sort is possible (only possible in a DAG).
- Prerequisite chain validation (e.g., course scheduling).

---

## Recognition Pattern

```
Directed graph
+ "circular dependency"
+ "can all tasks be completed?"
+ "is this a valid order?"
```
→ Think cycle detection in directed graph.

---

## Complexity Analysis

| Case    | Time     | Space |
|---------|----------|-------|
| Best    | O(V + E) | O(V)  |
| Average | O(V + E) | O(V)  |
| Worst   | O(V + E) | O(V)  |

---

## Core Idea

**DFS approach:** Use three states — 0 (unvisited), 1 (in current path / grey), 2 (fully processed / black). If DFS reaches a node with state 1, a back edge is found → cycle exists.

**Kahn's approach:** Run BFS topological sort. If the result doesn't include all vertices, a cycle prevents some from reaching zero in-degree.

---

## Visualization

```
Graph with cycle: 0 → 1 → 2 → 1 (cycle: 1→2→1)

DFS from 0:
  state[0]=1 → visit 1
  state[1]=1 → visit 2
  state[2]=1 → visit 1
  state[1] == 1 → CYCLE FOUND
```

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// DFS approach — O(V+E)
bool dfsHasCycle(int u, vector<vector<int>>& adj, vector<int>& state) {
    state[u] = 1;  // mark grey
    for (int v : adj[u]) {
        if (state[v] == 1) return true;             // back edge → cycle
        if (state[v] == 0 && dfsHasCycle(v, adj, state)) return true;
    }
    state[u] = 2;  // mark black
    return false;
}

bool hasCycleDirected(int n, vector<vector<int>>& adj) {
    vector<int> state(n, 0);
    for (int i = 0; i < n; i++)
        if (state[i] == 0 && dfsHasCycle(i, adj, state))
            return true;
    return false;
}

// Kahn's approach — O(V+E)
bool hasCycleKahns(int n, vector<vector<int>>& adj) {
    vector<int> inDeg(n, 0);
    for (int u = 0; u < n; u++)
        for (int v : adj[u]) inDeg[v]++;

    queue<int> q;
    for (int i = 0; i < n; i++)
        if (inDeg[i] == 0) q.push(i);

    int processed = 0;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        processed++;
        for (int v : adj[u])
            if (--inDeg[v] == 0) q.push(v);
    }
    return processed != n;  // cycle if not all vertices processed
}
```

---

## Why It Works

**DFS:** State 1 (grey) marks nodes on the current DFS path. A back edge to a grey node means the current path forms a loop — a directed cycle.

**Kahn's:** Vertices in a cycle always depend on each other circularly, so their in-degrees never reach 0. They never enter the queue and are never counted — so `processed < n` confirms a cycle.

---

## Important Notes

- **Do not use a simple boolean `visited` array** for directed graphs. A visited black node reached via a cross edge is not a cycle — you need the grey state to distinguish "currently on path" from "already processed".
- Kahn's approach is simpler to implement and remember; DFS approach is useful when you also need the topological order or the specific cycle path.
- To find the actual cycle (not just detect it), track the parent and stop when a back edge is found, then trace back through parents.
