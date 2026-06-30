// kruskal: builds Minimum Spanning Tree (MST) using Kruskal's algorithm
struct DSU
{
    vector<int> par;
    // initialize DSU with n nodes
    DSU(int n) : par(n) { iota(par.begin(), par.end(), 0); }

    // find with path compression
    int find(int x) { return x == par[x] ? x : par[x] = find(par[x]); }

    // unite two sets, return false if already connected
    bool unite(int x, int y)
    {
        x = find(x);
        y = find(y);
        if (x == y)
            return false;
        par[y] = x;
        return true;
    }
};

int kruskal(int n, vector<tuple<int, int, int>> &edges)
{
    sort(edges.begin(), edges.end()); // sort edges by weight
    DSU dsu(n);
    int total = 0;
    for (auto [w, u, v] : edges)
        if (dsu.unite(u, v)) // if u and v are in different sets
            total += w;      // include this edge in MST
    return total;            // return total weight of MST
}