int n;
cin >> n;                   // number of nodes
vector<vector<int>> adj(n); // adjacency list
adj[u].push_back(v);        // add edge u → v (directed)
adj[v].push_back(u);        // add edge v → u (undirected)

// ----------------depth-first search (dfs) – recursive----------------
void dfs(int u, vector<vector<int>> &adj, vector<bool> &vis)
{
    vis[u] = true;
    for (int v : adj[u])
        if (!vis[v])
            dfs(v, adj, vis);
}

// ----------------breadth-first search (bfs)----------------
void bfs(int start, vector<vector<int>> &adj)
{
    queue<int> q;
    vector<bool> vis(adj.size(), false);
    q.push(start);
    vis[start] = true;
    while (!q.empty())
    {
        int u = q.front();
        q.pop();
        for (int v : adj[u])
        {
            if (!vis[v])
            {
                vis[v] = true;
                q.push(v);
            }
        }
    }
}

// --------------------DSU--------------------

// basic setup

vector<int> parent(n), rank(n, 0);     // declare parent & rank
iota(parent.begin(), parent.end(), 0); // initialize parent[i] = i

// find with path compression
int find(int u) { return parent[u] = (parent[u] == u ? u : find(parent[u])); }

// union by rank

void unite(int u, int v)
{
    u = find(u), v = find(v);
    if (u != v)
    {
        if (rank[u] < rank[v])
            swap(u, v);
        parent[v] = u;
        if (rank[u] == rank[v])
            rank[u]++;
    }
}

// ✅ check if in same set

if (find(u) == find(v))
    cout << "Same component";