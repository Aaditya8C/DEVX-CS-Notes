# Bipartite Graph Check

## Introduction

Determines whether a graph is bipartite — i.e., its vertices can be split into two groups such that every edge connects a vertex from one group to the other. Equivalently, a graph is bipartite if and only if it contains no odd-length cycle.

---

## Intuition

Try to 2-color the graph: assign alternating colors to nodes as you traverse. Start with any node as color 0. Color all its neighbors color 1, then their neighbors color 0, and so on. If you ever try to color a node that already has the same color as the current node, the graph has an odd cycle and is not bipartite.

---

## When to Use

- Checking if a graph can be divided into two non-overlapping groups.
- Problems involving matching (bipartite matching uses bipartite structure).
- "Can we assign X or Y to every node such that no two adjacent nodes have the same assignment?"
- Detecting odd cycles.

---

## Recognition Pattern

```
"Divide into two groups"
"No two adjacent nodes in the same group"
"2-colorable"
"Team A vs Team B"
"Odd cycle detection"
```
→ Think Bipartite Check (BFS/DFS coloring).

---

## Complexity Analysis

| Case    | Time     | Space |
|---------|----------|-------|
| Best    | O(V + E) | O(V)  |
| Average | O(V + E) | O(V)  |
| Worst   | O(V + E) | O(V)  |

---

## Core Idea

BFS or DFS with a color array (0 or 1). Initialize all colors to -1. For each unvisited node, assign it color 0 and BFS/DFS: assign every unvisited neighbor the opposite color. If a visited neighbor has the same color as the current node, return false.

---

## Visualization

```
Bipartite graph:    Non-bipartite (odd cycle):
0 - 1               0 - 1
|   |               |   |
3 - 2               2 - 0 (triangle: 0-1-2-0)
    |
    4

Color BFS:          Color BFS:
0→color[0]=0        0→color[0]=0
  1→color[1]=1        1→color[1]=1
  3→color[3]=1        2→color[2]=0 (from 1)
    2→color[2]=0      0: color[0]=0 == color[2]=0 → NOT BIPARTITE
      4→color[4]=1
All consistent → BIPARTITE
```

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

bool bfsColor(int src, vector<vector<int>>& adj, vector<int>& color) {
    queue<int> q;
    color[src] = 0;
    q.push(src);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : adj[u]) {
            if (color[v] == -1) {
                color[v] = 1 - color[u];   // opposite color
                q.push(v);
            } else if (color[v] == color[u]) {
                return false;              // same color → not bipartite
            }
        }
    }
    return true;
}

bool isBipartite(int n, vector<vector<int>>& adj) {
    vector<int> color(n, -1);
    for (int i = 0; i < n; i++)
        if (color[i] == -1 && !bfsColor(i, adj, color))
            return false;
    return true;
}
```

---

## Why It Works

A graph is bipartite iff it has no odd cycle. The 2-coloring attempt precisely checks this: BFS ensures that same-level nodes get the same parity, and alternating levels get opposite colors. If an edge connects two same-color nodes, those nodes are at the same BFS level, forming an odd cycle. Conversely, if 2-coloring succeeds, the two color classes form a valid bipartition.

---

## Important Notes

- Handle disconnected graphs by starting a new BFS from each unvisited node.
- `1 - color[u]` is the cleanest way to flip between 0 and 1 without a conditional.
- Trees are always bipartite (no cycles at all).
- For undirected graphs only — bipartiteness is not defined the same way for directed graphs.
- The bipartite check is the foundation for **bipartite matching** problems (Hungarian algorithm, Hopcroft-Karp).
