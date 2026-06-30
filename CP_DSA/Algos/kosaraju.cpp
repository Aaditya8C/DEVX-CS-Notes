// dfs1: first DFS pass to fill stack with finishing times
void dfs1(int u, vector<vector<int>> &g, vector<bool> &vis, stack<int> &s)
{
    vis[u] = true;
    for (int v : g[u])
        if (!vis[v])
            dfs1(v, g, vis, s);
    s.push(u); // push node after visiting all descendants
}

// dfs2: second DFS on reversed graph to collect components
void dfs2(int u, vector<vector<int>> &rev, vector<bool> &vis, vector<int> &comp)
{
    vis[u] = true;
    comp.push_back(u);
    for (int v : rev[u])
        if (!vis[v])
            dfs2(v, rev, vis, comp);
}

// kosaraju: returns all strongly connected components using Kosaraju's algorithm
vector<vector<int>> kosaraju(int n, vector<vector<int>> &g)
{
    stack<int> s;
    vector<bool> vis(n, false);

    // 1st pass: fill stack with post-order of original graph
    for (int i = 0; i < n; i++)
        if (!vis[i])
            dfs1(i, g, vis, s);

    // reverse the graph
    vector<vector<int>> rev(n);
    for (int u = 0; u < n; u++)
        for (int v : g[u])
            rev[v].push_back(u);

    // 2nd pass: collect components using DFS in reverse graph
    fill(vis.begin(), vis.end(), false);
    vector<vector<int>> scc;
    while (!s.empty())
    {
        int u = s.top();
        s.pop();
        if (!vis[u])
        {
            vector<int> comp;
            dfs2(u, rev, vis, comp);
            scc.push_back(comp); // store each component
        }
    }
    return scc; // return list of strongly connected components
}
