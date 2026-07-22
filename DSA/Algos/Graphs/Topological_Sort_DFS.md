# Topological Sort — DFS-based

## Introduction

Produces a topological ordering of a DAG by running DFS and collecting vertices in reverse post-order: a vertex is added to the result only after all vertices reachable from it have been fully processed.

---

## Intuition

In any valid topological order, a vertex must appear before all vertices it has edges to. DFS naturally visits descendants before returning — so if you record a vertex when DFS finishes it (post-order), vertices with no outgoing edges finish first. Reversing this post-order gives exactly the topological order: vertices with more dependents finish later, so after reversal they appear first.

---

## When to Use

- Same use cases as Kahn's algorithm: DAG ordering, dependency resolution, course scheduling.
- Prefer DFS-based when you also need to detect back edges for cycle detection alongside ordering.
- Base for Kosaraju's SCC (the first pass is exactly this DFS post-order).

---

## Recognition Pattern

Same as Kahn's:
```
Directed graph + ordering with dependencies + DAG
```
→ Either Kahn's (BFS, easier cycle detection) or DFS topological sort.

Use DFS variant when: you need post-order finish times for other purposes, or you're building on top of DFS already.

---

## Complexity Analysis

| Case    | Time     | Space |
|---------|----------|-------|
| Best    | O(V + E) | O(V)  |
| Average | O(V + E) | O(V)  |
| Worst   | O(V + E) | O(V)  |

---

## Core Idea

Run DFS. Use a three-color visited state: **white** (unvisited), **grey** (in current DFS path), **black** (fully processed). When a node finishes (all descendants processed), push it onto a stack. After full DFS, the stack from top to bottom is a valid topological order. A back edge (grey → grey) indicates a cycle.

---

## Visualization

```
Graph: 5 → 2 → 3
       5 → 0
       4 → 0
       4 → 1
       2 → 3
       3 → 1

DFS from 5:
  visit(5) → visit(2) → visit(3) → visit(1)
    finish(1), push 1
  finish(3), push 3
  finish(2), push 2
  visit(0), finish(0), push 0
finish(5), push 5

DFS from 4:
  visit(4) → visit(0): already visited
  visit(1): already visited
  finish(4), push 4

Stack (top = front): [4, 5, 0, 2, 3, 1]
Topological order:    4, 5, 0, 2, 3, 1
```

---

## Critical Code Explanation

### Three-Color State for Cycle Detection

```cpp
// 0 = unvisited, 1 = in stack (grey), 2 = done (black)
vector<int> state(n, 0);

bool dfs(int u, ...) {
    state[u] = 1;   // grey — currently on DFS path
    for (int v : adj[u]) {
        if (state[v] == 1) return false;   // back edge → CYCLE
        if (state[v] == 0 && !dfs(v, ...)) return false;
    }
    state[u] = 2;   // black — fully processed
    order.push_back(u);
    return true;
}
```

The grey state catches back edges — if DFS reaches a node already on the current path, there is a cycle. A cross edge (reaching a black node) is safe to ignore.

### Post-Order Push Then Reverse

```cpp
order.push_back(u);   // push when done
// After all DFS calls:
reverse(order.begin(), order.end());
```

Or use a stack and collect from top. Either way, the node that finishes last ends up first in the topological order.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

bool dfs(int u, vector<vector<int>>& adj, vector<int>& state, vector<int>& order) {
    state[u] = 1;  // grey
    for (int v : adj[u]) {
        if (state[v] == 1) return false;        // cycle
        if (state[v] == 0 && !dfs(v, adj, state, order)) return false;
    }
    state[u] = 2;  // black
    order.push_back(u);
    return true;
}

// Returns topological order; empty vector if cycle exists
vector<int> topoSortDFS(int n, vector<vector<int>>& adj) {
    vector<int> state(n, 0), order;

    for (int i = 0; i < n; i++) {
        if (state[i] == 0) {
            if (!dfs(i, adj, state, order)) return {};  // cycle detected
        }
    }

    reverse(order.begin(), order.end());
    return order;
}
```

---

## Why It Works

A vertex is added to `order` only after all vertices reachable from it are already in `order`. So when the list is reversed, this vertex appears before all its reachable vertices — the topological order property. Back edge detection works because a grey node is an ancestor on the current DFS path. Reaching it again means there is a path from the current node back to an ancestor, forming a directed cycle.

---

## Important Notes

- Kahn's algorithm is generally preferred for cycle detection because the count check (`result.size() != n`) is simpler than managing three-color state.
- Use DFS-based topological sort when you also need post-order finish times (e.g., Kosaraju's first pass).
- The two-color approach (`visited` bool) is sufficient if you only need topological ordering without cycle detection — just push on return.
- Both DFS and Kahn's produce valid but potentially different topological orderings for the same graph.
