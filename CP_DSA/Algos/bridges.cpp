// 2. bridges (tarjan's algorithm)
// finds all bridges (critical edges whose removal increases components)
vector<pair<int, int>> bridges;
int timer = 0;

// dfsBridge: performs DFS traversal while keeping track of discovery time (tin) and lowest reachable ancestor (low)
void dfsBridge(int u, int p, vector<vector<int>> &g, vector<int> &tin, vector<int> &low, vector<bool> &vis)
{
    vis[u] = true;             // mark current node as visited
    tin[u] = low[u] = timer++; // set discovery and low time

    for (int v : g[u])
    {
        if (v == p) // skip parent to avoid trivial cycle
            continue;

        if (vis[v]) // back edge: update low[u] with tin[v]
            low[u] = min(low[u], tin[v]);
        else
        {
            dfsBridge(v, u, g, tin, low, vis); // dfs into child
            low[u] = min(low[u], low[v]);      // update low[u] with low[v]

            if (low[v] > tin[u]) // if v can't reach u or ancestor, it's a bridge
                bridges.emplace_back(u, v);
        }
    }
}