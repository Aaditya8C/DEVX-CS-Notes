struct TreeNode
{
    int val;
    TreeNode *left, *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

TreeNode *root = new TreeNode(x); // create root node
root->left = new TreeNode(y);     // insert left child
root->right = new TreeNode(z);    // insert right child
root = root->left;                // move to left child
delete root;                      // delete node

// ----------------BFS Traversal----------------
queue<TreeNode *> q;
q.push(root); // start bfs
while (!q.empty())
{
    TreeNode *node = q.front();
    q.pop(); // process node
    if (node->left)
        q.push(node->left); // enqueue left
    if (node->right)
        q.push(node->right); // enqueue right
}