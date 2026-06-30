// 1. kahn's algorithm (bfs topological sort + cycle detection)
vector<int> topologicalSort(int n, vector<vector<int>> &adj)
{
    vector<int> indegree(n, 0), topo;
    for (auto &edges : adj)
        for (int v : edges)
            indegree[v]++;

    queue<int> q;
    for (int i = 0; i < n; i++)
        if (indegree[i] == 0)
            q.push(i);

    while (!q.empty())
    {
        int u = q.front();
        q.pop();
        topo.push_back(u);
        for (int v : adj[u])
            if (--indegree[v] == 0)
                q.push(v);
    }
    return (topo.size() == n ? topo : vector<int>()); // empty => cycle
}