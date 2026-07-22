#include <bits/stdc++.h> // This will work only for g++ compiler.
#include <iostream>
#include <vector>
#define for0(i, n) for (int i = 0; i < (int)(n); ++i)            // 0 based indexing
#define for1(i, n) for (int i = 1; i < (int)(n); ++i)            // 0 based indexing starts with 1
#define for11(i, n) for (int i = 1; i <= (int)(n); ++i)          // 1 based indexing
#define forc(i, l, r) for (int i = (int)(l); i <= (int)(r); ++i) // closed interver from l to r r inclusive
#define forr0(i, n) for (int i = (int)(n) - 1; i >= 0; --i)      // reverse 0 based.
#define forr1(i, n) for (int i = (int)(n); i >= 1; --i)          // reverse 1 based

// short hand for usual tokens
#define pb push_back
#define fi first
#define se second

// to be used with algorithms that processes a container Eg: find(all(c),42)
#define all(x) (x).begin(), (x).end()  // Forward traversal
#define rall(x) (x).rbegin, (x).rend() // reverse traversal

// traversal function to avoid long template definition. Now with C++11 auto alleviates the pain.
#define tr(c, i) for (__typeof__((c)).begin() i = (c).begin(); i != (c).end(); i++)

// find if a given value is present in a container. Container version. Runs in log(n) for set and map
#define present(c, x) ((c).find(x) != (c).end())

// find version works for all containers. This is present in std namespace.
#define cpresent(c, x) (find(all(c), x) != (c).end())

// Avoiding wrap around of size()-1 where size is a unsigned int.
#define sz(a) int((a).size())

// Macro for ceiling division
#define ceil_div(a, b) (((a) + ((b) - 1)) / (b))

using namespace std;

// Shorthand for commonly used types
typedef vector<int> vi;
typedef vector<vi> vvi;
typedef pair<int, int> ii;
typedef vector<ii> vii;
typedef long long ll;
typedef vector<ll> vll;
typedef vector<vll> vvll;
typedef double ld;

void solve()
{
    int n;
    cin >> n;
    vi a(n + 1), b(n + 1);
    for11(i, n)
    {
        cin >> a[i];
    }
    for11(i, n)
    {
        cin >> b[i];
    }

    vi fa(2 * n + 1);
    vi fb(2 * n + 1);
    int prev = 1;
    int res = 0;

    for (int i = 2; i <= n; i++)
    {
        if (a[i] != a[i - 1])
        {
            fa[a[i - 1]] = max(fa[a[i - 1]], i - prev);
            prev = i;
        }
    }
    fa[a[n]] = max(fa[a[n]], n - prev + 1);

    prev = 1;

    for (int i = 2; i <= n; i++)
    {
        if (b[i] != b[i - 1])
        {
            fb[b[i - 1]] = max(fb[b[i - 1]], i - prev);
            prev = i;
        }
    }
    fb[b[n]] = max(fb[b[n]], n - prev + 1);

    for11(i, n + n)
    {
        res = max(res, fa[i] + fb[i]);
    }
    cout << res << "\n";
}
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);

    cout.precision(10);

    int T;
    cin >> T;
    while (T--)
    {
        solve();
    }
    return 0;
}