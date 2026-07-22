# DFS — Depth-First Search

## Introduction

DFS explores a graph by going as deep as possible along each branch before backtracking. It is the foundation for cycle detection, topological sort, connected components, SCC algorithms, and bridge/articulation point finding.

---

## Intuition

Pick a node, go deep until you hit a dead end, then backtrack and try the next unvisited neighbor. The recursion call stack (or an explicit stack) naturally handles the backtracking. The key insight is that DFS produces a DFS tree with meaningful edge classifications — tree edges, back edges, forward edges, cross edges — and these classifications are what power most graph algorithms built on top of DFS.

---

## When to Use

- Cycle detection (directed and undirected)
- Topological sort (DFS-based)
- Strongly Connected Components (Kosaraju, Tarjan)
- Finding bridges and articulation points
- Connected components / flood fill
- Path existence between two nodes
- Generating permutations, combinations (backtracking)

---

## Recognition Pattern

```
Graph traversal where order/depth matters
+ "Is there a path?"
+ "Detect cycle"
+ "Find all components"
+ "Post-order processing" (topological sort, SCC)
```
→ Think DFS.

---

## Complexity Analysis

| Case    | Time     | Space    |
|---------|----------|----------|
| Best    | O(V + E) | O(V)     |
| Average | O(V + E) | O(V)     |
| Worst   | O(V + E) | O(V)     |

Space is O(V) for the visited array and the recursion call stack (worst case O(V) depth for a linear chain graph).

---

## Core Idea

Mark the current node visited, then recursively visit every unvisited neighbor. After all neighbors are processed, the current node is "finished" — this post-order finish time is what topological sort and Kosaraju's algorithm exploit. Iterative DFS replaces the call stack with an explicit stack, but the traversal order may differ slightly.

---

## Visualization

```
Graph:  0 → 1 → 3
        ↓   ↓
        2   4

DFS from 0 (recursive):

visit(0) → vis={0}
  visit(1) → vis={0,1}
    visit(3) → vis={0,1,3}
      no unvisited neighbors
    finish(3)
    visit(4) → vis={0,1,3,4}
      no unvisited neighbors
    finish(4)
  finish(1)
  visit(2) → vis={0,1,2,3,4}
    no unvisited neighbors
  finish(2)
finish(0)

DFS order:     0, 1, 3, 4, 2
Finish order:  3, 4, 1, 2, 0  ← used in topological sort / Kosaraju
```

---

## Critical Code Explanation

### Recursive vs Iterative

```cpp
// Recursive — natural, uses call stack
void dfs(int u, vector<vector<int>>& adj, vector<bool>& vis) {
    vis[u] = true;
    for (int v : adj[u])
        if (!vis[v]) dfs(v, adj, vis);
}
```

```cpp
// Iterative — avoids stack overflow on deep graphs
void dfsIter(int src, vector<vector<int>>& adj, vector<bool>& vis) {
    stack<int> st;
    st.push(src); vis[src] = true;
    while (!st.empty()) {
        int u = st.top(); st.pop();
        for (int v : adj[u])
            if (!vis[v]) { vis[v] = true; st.push(v); }
    }
}
```

Warning: the iterative version visits neighbors in reverse order compared to recursive DFS, and does not naturally give post-order finish times. For algorithms that depend on finish order (Kosaraju, topological sort), use the recursive version or simulate post-order explicitly.

### Capturing Finish Order

```cpp
void dfs(int u, vector<vector<int>>& adj, vector<bool>& vis, vector<int>& order) {
    vis[u] = true;
    for (int v : adj[u])
        if (!vis[v]) dfs(v, adj, vis, order);
    order.push_back(u);   // push AFTER all descendants are done
}
```

`order` reversed gives a valid topological sort. Used directly in Kosaraju's first pass.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

// Recursive DFS
void dfs(int u, vector<vector<int>>& adj, vector<bool>& vis) {
    vis[u] = true;
    for (int v : adj[u])
        if (!vis[v]) dfs(v, adj, vis);
}

// DFS with post-order finish tracking
void dfsOrder(int u, vector<vector<int>>& adj, vector<bool>& vis, vector<int>& order) {
    vis[u] = true;
    for (int v : adj[u])
        if (!vis[v]) dfsOrder(v, adj, vis, order);
    order.push_back(u);
}

// DFS over all components
void dfsAll(int n, vector<vector<int>>& adj) {
    vector<bool> vis(n, false);
    for (int i = 0; i < n; i++)
        if (!vis[i]) dfs(i, adj, vis);
}

// Iterative DFS
void dfsIter(int src, vector<vector<int>>& adj) {
    int n = adj.size();
    vector<bool> vis(n, false);
    stack<int> st;
    st.push(src); vis[src] = true;

    while (!st.empty()) {
        int u = st.top(); st.pop();
        for (int v : adj[u])
            if (!vis[v]) { vis[v] = true; st.push(v); }
    }
}
```

---

## Why It Works

DFS works because every node is visited exactly once (guarded by `vis[]`). The recursion naturally explores all nodes reachable from the current node before returning, which exhausts all paths from any starting point. The call stack implicitly stores the current path from root to the node being explored — this is why backtracking and cycle detection fall naturally out of DFS.

---

## Important Notes

- For disconnected graphs, always loop over all nodes and start DFS from each unvisited one.
- Recursion depth can hit stack limits on large inputs (n > ~10⁵). Use iterative DFS or increase stack size in competitive programming.
- DFS on an undirected graph: pass the parent to avoid treating the edge back to the parent as a back edge.
- DFS tree edge classifications: **tree edge** (unvisited neighbor), **back edge** (ancestor in DFS tree — indicates a cycle in directed graphs), **forward/cross edges** (only in directed graphs).
- Back edge → cycle in directed graph. In undirected graphs, any non-tree edge is a back edge.
