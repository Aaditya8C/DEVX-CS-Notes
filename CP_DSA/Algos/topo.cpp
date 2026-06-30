vector<int> topoSort(int n, vector<vector<int>> &adj)
{
    vector<int> inDeg(n, 0), res;
    for (auto &u : adj)
        for (int v : u)
            inDeg[v]++;
    queue<int> q;
    for (int i = 0; i < n; i++)
        if (inDeg[i] == 0)
            q.push(i);
    while (!q.empty())
    {
        int u = q.front();
        q.pop();
        res.push_back(u);
        for (int v : adj[u])
            if (--inDeg[v] == 0)
                q.push(v);
    }
    return res;
}
