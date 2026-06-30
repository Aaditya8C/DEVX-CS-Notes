vector<int> dijkstra(int n, int src, vector<vector<pair<int, int>>> &adj)
{
    vector<int> dist(n, INT_MAX);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
    dist[src] = 0;
    pq.push({0, src});
    while (!pq.empty())
    {
        auto [d, u] = pq.top();
        pq.pop();
        if (d > dist[u])
            continue;
        for (auto [v, w] : adj[u])
            if (dist[v] > d + w)
                dist[v] = d + w, pq.push({dist[v], v});
    }
    return dist;
}
