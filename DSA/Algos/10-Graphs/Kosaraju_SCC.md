# Kosaraju's Algorithm — Strongly Connected Components

## Introduction

Finds all Strongly Connected Components (SCCs) in a directed graph in O(V + E). An SCC is a maximal set of vertices where every vertex is reachable from every other vertex.

---

## Intuition

The key observation: if you reverse all edges in a graph, the SCCs stay the same, but the condensation graph (where each SCC is a supernode) gets its edges reversed. In the original graph, a DFS that finishes last must be in a "source" SCC (one with no incoming edges from other SCCs). If you start a DFS on the reversed graph from this "source" SCC, you can reach exactly the nodes in that SCC. Process nodes in reverse finishing order, and each DFS on the reversed graph carves out exactly one SCC.

---

## When to Use

- Finding strongly connected components in a directed graph.
- Condensing a directed graph into a DAG of SCCs.
- Detecting if all nodes in a directed graph can reach each other.
- 2-SAT (the SCC structure of the implication graph encodes the solution).

---

## Recognition Pattern

```
Directed graph
+ "strongly connected"
+ "every node reachable from every other node"
+ "condense graph to DAG"
+ 2-SAT
```
→ Think Kosaraju's (or Tarjan's SCC).

---

## Complexity Analysis

| Case    | Time     | Space |
|---------|----------|-------|
| Best    | O(V + E) | O(V)  |
| Average | O(V + E) | O(V)  |
| Worst   | O(V + E) | O(V)  |

Two complete DFS traversals (once on original, once on reversed graph).

---

## Core Idea

**Pass 1:** Run DFS on the original graph. Push each node to a stack when it finishes (post-order). Nodes that finish last belong to "source" SCCs in the condensation.

**Pass 2:** Reverse all edges. Pop nodes from the stack (highest finish time first) and run DFS on the reversed graph from each unvisited node. Each DFS in pass 2 explores exactly one SCC.

---

## Visualization

```
Graph:  0 → 1 → 2 → 0    (SCC: {0,1,2})
        2 → 3 → 4 → 3    (SCC: {3,4})

Pass 1 DFS on original (from 0):
  visit 0 → 1 → 2 → 0 (already visited, back edge)
  finish order: 2, 1, 0, then 4, 3
  Stack (bottom to top): 4, 3, 0, 1, 2  ← 2 finishes last

Reversed edges: 1→0, 2→1, 0→2, 3→2, 4→3, 3→4

Pass 2 DFS on reversed graph, processing stack top-first:
  Pop 2: DFS from 2 on reversed → visits 2, 1, 0 → SCC: {0, 1, 2}
  Pop 3: DFS from 3 on reversed → visits 3, 4 → SCC: {3, 4}

SCCs: {0,1,2}, {3,4}
```

---

## Critical Code Explanation

### Pass 1 — Fill Stack with Post-Order

```cpp
void dfs1(int u, vector<vector<int>>& g, vector<bool>& vis, stack<int>& s) {
    vis[u] = true;
    for (int v : g[u])
        if (!vis[v]) dfs1(v, g, vis, s);
    s.push(u);   // push AFTER all descendants are done
}
```

Pushing after all descendants ensures the node with the highest finish time is at the top. The node that finishes last in the entire DFS is a node in a source SCC.

### Reversing the Graph

```cpp
vector<vector<int>> rev(n);
for (int u = 0; u < n; u++)
    for (int v : g[u])
        rev[v].push_back(u);   // flip every edge direction
```

Reversing edges is O(V + E). The SCC structure is preserved, but source SCCs in the original become sink SCCs in the reversed graph — exactly what we want to explore from.

### Pass 2 — Collect Components

```cpp
void dfs2(int u, vector<vector<int>>& rev, vector<bool>& vis, vector<int>& comp) {
    vis[u] = true;
    comp.push_back(u);
    for (int v : rev[u])
        if (!vis[v]) dfs2(v, rev, vis, comp);
}
```

Starting DFS on the reversed graph from the highest-finish-time node explores exactly the SCC containing that node, because in the reversed graph there are no edges leaving this SCC (it was a source SCC in the original).

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

void dfs1(int u, vector<vector<int>>& g, vector<bool>& vis, stack<int>& s) {
    vis[u] = true;
    for (int v : g[u])
        if (!vis[v]) dfs1(v, g, vis, s);
    s.push(u);
}

void dfs2(int u, vector<vector<int>>& rev, vector<bool>& vis, vector<int>& comp) {
    vis[u] = true;
    comp.push_back(u);
    for (int v : rev[u])
        if (!vis[v]) dfs2(v, rev, vis, comp);
}

vector<vector<int>> kosaraju(int n, vector<vector<int>>& g) {
    // Pass 1: fill stack with post-order
    stack<int> s;
    vector<bool> vis(n, false);
    for (int i = 0; i < n; i++)
        if (!vis[i]) dfs1(i, g, vis, s);

    // Build reversed graph
    vector<vector<int>> rev(n);
    for (int u = 0; u < n; u++)
        for (int v : g[u])
            rev[v].push_back(u);

    // Pass 2: collect SCCs from reversed graph
    fill(vis.begin(), vis.end(), false);
    vector<vector<int>> sccs;
    while (!s.empty()) {
        int u = s.top(); s.pop();
        if (!vis[u]) {
            vector<int> comp;
            dfs2(u, rev, vis, comp);
            sccs.push_back(comp);
        }
    }
    return sccs;
}
```

---

## Why It Works

After pass 1, the stack contains nodes in increasing post-order finish time. The node at the top finished last and therefore belongs to a source SCC in the condensation. In the reversed graph, a source SCC becomes a sink SCC — DFS from this node in the reversed graph cannot escape the SCC (because all outgoing edges in the reversed graph lead back into the SCC). This is why each DFS in pass 2 collects exactly one SCC.

---

## Important Notes

- **Tarjan's SCC** is an alternative that uses a single DFS pass with a stack and low values — slightly more code but only one graph traversal.
- Both Kosaraju and Tarjan are O(V + E); Kosaraju is easier to understand and implement correctly.
- The order of SCCs returned by Kosaraju is in reverse topological order of the condensation DAG.
- A graph is strongly connected iff Kosaraju returns exactly one SCC containing all vertices.
- SCCs reduce any directed graph problem to a DAG problem — once you have the condensation, you can apply DP or topological sort on the DAG.
