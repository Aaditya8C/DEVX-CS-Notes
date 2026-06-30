// floyd-warshall algorithm for all-pairs shortest path
// dist[i][j] is the shortest distance from i to j
void floydWarshall(vector<vector<int>> &dist, int n)
{
    for (int k = 0; k < n; k++)         // try every intermediate node k
        for (int i = 0; i < n; i++)     // for every source node i
            for (int j = 0; j < n; j++) // for every destination node j
                if (dist[i][k] != INT_MAX && dist[k][j] != INT_MAX)
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]); // update with shorter path if possible
}