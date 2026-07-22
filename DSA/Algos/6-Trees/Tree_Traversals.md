# Tree Traversals & Operations

## Introduction

Binary tree traversals — inorder, preorder, postorder, and level-order — are the foundation of virtually every tree problem. Each traversal visits nodes in a specific order that makes certain information readily available.

---

## Intuition

The traversal order determines what information you have when you process a node:
- **Preorder (root first):** When you process a node, you haven't seen its subtrees yet — good for constructing trees, serialization.
- **Inorder (left first):** For a BST, this gives nodes in sorted order — good for k-th smallest, sorted output.
- **Postorder (children first):** When you process a node, both subtrees are fully processed — good for computing heights, deleting trees, aggregating from leaves.
- **Level-order (BFS):** Nodes at the same depth together — good for level-wise processing, finding width, zigzag traversal.

---

## When to Use

| Traversal   | Best For                                              |
|-------------|-------------------------------------------------------|
| Preorder    | Tree construction, serialization, root-first problems |
| Inorder     | BST sorted output, k-th smallest, validate BST        |
| Postorder   | Height/depth, subtree aggregation, deletion           |
| Level-order | Level-wise problems, width, zigzag, right-side view   |

---

## Complexity Analysis

All traversals: **O(n) time**, **O(h) space** for recursive (call stack depth = tree height h). Level-order is **O(n) space** (queue can hold an entire level).

---

## Critical Code Explanation

### Morris Inorder — O(1) Space

```cpp
// Without recursion or stack — modifies tree temporarily
while (curr) {
    if (!curr->left) { visit(curr); curr = curr->right; }
    else {
        TreeNode* prev = curr->left;
        while (prev->right && prev->right != curr) prev = prev->right;
        if (!prev->right) { prev->right = curr; curr = curr->left; }
        else { prev->right = nullptr; visit(curr); curr = curr->right; }
    }
}
```

Creates temporary links from the inorder predecessor to the current node to avoid a stack. O(1) space for traversal.

### Iterative Inorder — Interview Favorite

```cpp
stack<TreeNode*> st;
TreeNode* curr = root;
while (curr || !st.empty()) {
    while (curr) { st.push(curr); curr = curr->left; }  // go left
    curr = st.top(); st.pop();
    visit(curr);          // process
    curr = curr->right;   // go right
}
```

Go left as far as possible, process the top, then move right. The stack simulates the call stack.

---

## Code

```cpp
#include <bits/stdc++.h>
using namespace std;

struct TreeNode {
    int val;
    TreeNode *left, *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

// ── Recursive traversals ──────────────────────────────────────────────────────
void inorder(TreeNode* root, vector<int>& res) {
    if (!root) return;
    inorder(root->left, res);
    res.push_back(root->val);
    inorder(root->right, res);
}

void preorder(TreeNode* root, vector<int>& res) {
    if (!root) return;
    res.push_back(root->val);
    preorder(root->left, res);
    preorder(root->right, res);
}

void postorder(TreeNode* root, vector<int>& res) {
    if (!root) return;
    postorder(root->left, res);
    postorder(root->right, res);
    res.push_back(root->val);
}

// ── Iterative inorder ─────────────────────────────────────────────────────────
vector<int> iterativeInorder(TreeNode* root) {
    vector<int> res;
    stack<TreeNode*> st;
    TreeNode* curr = root;
    while (curr || !st.empty()) {
        while (curr) { st.push(curr); curr = curr->left; }
        curr = st.top(); st.pop();
        res.push_back(curr->val);
        curr = curr->right;
    }
    return res;
}

// ── Level-order (BFS) ─────────────────────────────────────────────────────────
vector<vector<int>> levelOrder(TreeNode* root) {
    if (!root) return {};
    vector<vector<int>> res;
    queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        int sz = q.size();
        res.push_back({});
        while (sz--) {
            auto node = q.front(); q.pop();
            res.back().push_back(node->val);
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }
    }
    return res;
}

// ── Tree height ───────────────────────────────────────────────────────────────
int height(TreeNode* root) {
    if (!root) return 0;
    return 1 + max(height(root->left), height(root->right));
}

// ── Diameter of binary tree ──────────────────────────────────────────────────
int diameter = 0;
int dfsHeight(TreeNode* root) {
    if (!root) return 0;
    int l = dfsHeight(root->left), r = dfsHeight(root->right);
    diameter = max(diameter, l + r);   // update diameter at each node
    return 1 + max(l, r);
}

// ── Lowest Common Ancestor ───────────────────────────────────────────────────
TreeNode* lca(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root || root == p || root == q) return root;
    TreeNode* left = lca(root->left, p, q);
    TreeNode* right = lca(root->right, p, q);
    if (left && right) return root;   // p and q split across subtrees
    return left ? left : right;
}

// ── Check if BST is valid ────────────────────────────────────────────────────
bool isValidBST(TreeNode* root, long lo = LLONG_MIN, long hi = LLONG_MAX) {
    if (!root) return true;
    if (root->val <= lo || root->val >= hi) return false;
    return isValidBST(root->left, lo, root->val) &&
           isValidBST(root->right, root->val, hi);
}
```

---

## Why It Works

Recursive traversals work because they naturally decompose the tree problem into: "do something at this node" + "recurse on left subtree" + "recurse on right subtree." The order of these three operations defines the traversal type. The base case (null node) terminates the recursion.

LCA works because if we find `p` in the left subtree and `q` in the right subtree, the current node must be the LCA. If both are on the same side, the ancestor is deeper in that subtree.

---

## Important Notes

- **Iterative preorder:** Use a stack; push right before left so left is processed first.
- **Iterative postorder:** Reverse of modified preorder (root, right, left) — push right then left onto stack, collect result, then reverse.
- **BST validation:** Must pass min/max bounds down the recursion. Checking `root->val > root->left->val` only is incorrect for deeper violations.
- **Diameter:** The diameter path doesn't necessarily pass through the root — compute it at every node as `left_height + right_height` and track the global max.
- **Right Side View:** Level-order traversal — take the last node of each level.
- LCA for BST: simpler — if both `p` and `q` are less than root, go left; if both are greater, go right; otherwise current node is LCA.
