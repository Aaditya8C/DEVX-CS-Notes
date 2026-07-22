# Union-Find (Disjoint Set Union)

## Introduction

A data structure that efficiently tracks which elements belong to the same group (component), supporting near O(1) union and find operations with path compression and union by rank.

---

## Intuition

Think of each group as a tree where every node points to its parent, and the root represents the group. To find which group a node belongs to, follow parent pointers to the root. To merge two groups, make the root of one point to the root of the other. Path compression flattens the tree during finds so future queries are faster. Union by rank keeps trees shallow by always attaching the smaller tree under the taller one.

---

## When to Use

- Cycle detection in undirected graphs (Kruskal's algorithm).
- Connected components in a dynamic graph (edges added online).
- Grouping elements that share a property.
- Minimum spanning tree (Kruskal).
- Number of connected components after union operations.
- "Are these two nodes in the same group?" with repeated updates.

---

## Recognition Pattern

```
Dynamic connectivity
+ "merge groups"
+ "same component / connected?"
+ edges added one at a time
+ Kruskal's MST
```
→ Think Union-Find.

---

## Complexity Analysis

| Operation     | Time             |
|---------------|------------------|
| `find`        | O(α(n)) ≈ O(1)   |
| `unite`       | O(α(n)) ≈ O(1)   |
| Build (n ops) | O(n · α(n))      |

α(n) is the inverse Ackermann function — less than 5 for any practical n. Effectively constant time.

**Space:** O(n) for parent and rank arrays.

---

## Core Idea

Two arrays: `parent[i]` (i's parent in the tree; `parent[i] = i` at init) and `rank[i]` (approximate tree height). `find(x)` chases parent pointers to the root. `unite(x, y)` finds both roots and attaches one tree to the other.

**Path Compression:** During `find`, set every traversed node's parent directly to the root. Future finds on these nodes are O(1).

**Union by Rank:** Always attach the shorter tree (smaller rank) under the taller tree (larger rank). If ranks are equal, attach either way and increment the rank of the new root.

---

## Visualization

```
Initial: parent = [0,1,2,3,4]  rank = [0,0,0,0,0]

unite(0,1):  root(0)=0, root(1)=1, rank equal → parent[1]=0, rank[0]=1
  parent = [0,0,2,3,4]  rank = [1,0,0,0,0]

unite(2,3):  parent[3]=2, rank[2]=1
  parent = [0,0,2,2,4]  rank = [1,0,1,0,0]

unite(0,2):  root(0)=0, root(2)=2, rank[0]=rank[2]=1 → parent[2]=0, rank[0]=2
  parent = [0,0,0,2,4]  rank = [2,0,1,0,0]

find(3): 3→2→0  (path compression: parent[3]=0, parent[2]=0)
  parent = [0,0,0,0,4]  ← flattened

connected(1,3) = find(1)==find(3) = 0==0 = true
```

---

## Critical Code Explanation

### Path Compression

```cpp
int find(int x) {
    return parent[x] == x ? x : parent[x] = find(parent[x]);
}
```

One-liner: if `x` is the root, return `x`. Otherwise, recursively find the root, and simultaneously set `parent[x]` to the root. Every node on the path gets its parent updated to the root in a single call.

### Union by Rank

```cpp
bool unite(int x, int y) {
    x = find(x); y = find(y);
    if (x == y) return false;         // already same component
    if (rank[x] < rank[y]) swap(x, y);
    parent[y] = x;                    // attach smaller (y) under larger (x)
    if (rank[x] == rank[y]) rank[x]++;
    return true;
}
```

Returns `false` if `x` and `y` were already connected — useful in Kruskal to detect cycle formation.

### Size Tracking (Alternative to Rank)

```cpp
// Replace rank with size, merge smaller into larger
if (size[x] < size[y]) swap(x, y);
parent[y] = x;
size[x] += size[y];
```

Union by size is equivalent to union by rank in practice. Size tracking is useful when you need the component size alongside connectivity.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

struct DSU {
    vector<int> parent, rank_, size_;
    int components;

    DSU(int n) : parent(n), rank_(n, 0), size_(n, 1), components(n) {
        iota(parent.begin(), parent.end(), 0);
    }

    int find(int x) {
        return parent[x] == x ? x : parent[x] = find(parent[x]);
    }

    bool unite(int x, int y) {
        x = find(x); y = find(y);
        if (x == y) return false;
        if (rank_[x] < rank_[y]) swap(x, y);
        parent[y] = x;
        size_[x] += size_[y];
        if (rank_[x] == rank_[y]) rank_[x]++;
        components--;
        return true;
    }

    bool connected(int x, int y) { return find(x) == find(y); }
    int getSize(int x) { return size_[find(x)]; }
    int numComponents() { return components; }
};
```

---

## Why It Works

**Path compression:** redirecting all nodes on a find-path directly to the root does not change which root they belong to — it only flattens the tree structure. Correctness is preserved; future finds on those nodes skip directly to the root.

**Union by rank:** never makes the tree taller than necessary. Without it, a sequence of unions can degenerate into a linked list with O(n) find cost. With union by rank, tree height is bounded by O(log n). Combined with path compression, the amortized cost drops to O(α(n)).

---

## Important Notes

- `rank` is an upper bound on tree height, not the exact height. Path compression can lower the actual height without updating `rank` — this is acceptable because `rank` is only used to guide union decisions.
- When you need the component size, track `size` instead of (or alongside) `rank`.
- **Rollback DSU (offline union-find):** For problems that require undoing unions (e.g., "delete an edge"), you can implement rollback by not using path compression (only union by rank) and maintaining a stack of operations.
- DSU **cannot handle edge deletions** in its basic form — only insertions.
- In competitive programming, the DSU struct is often the most reused code. Keep it in your template.
