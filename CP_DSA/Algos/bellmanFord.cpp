// bellman-ford algorithm for single-source shortest path (handles negative weights)
vector<int> bellmanFord(int n, int src, vector<tuple<int, int, int>> &edges)
{
    vector<int> dist(n, INT_MAX); // initialize distances to INF
    dist[src] = 0;                // distance to source is 0

    for (int i = 0; i < n - 1; i++) // relax all edges n-1 times
        for (auto [u, v, w] : edges)
            if (dist[u] != INT_MAX && dist[u] + w < dist[v])
                dist[v] = dist[u] + w; // update if shorter path found

    return dist; // returns shortest distances
}