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
    int n, m;
    cin >> n >> m;
    vll a(n);
    vll b(m);
    for0(i, n) cin >> a[i];
    for0(i, m) cin >> b[i];
    ll dp0, dp1;
    ll bMin = *min_element(all(b));
    ll bMax = *max_element(all(b));

    dp0 = a[0];
    dp1 = bMin - a[0];

    for1(i, n)
    {
        ll newDp0 = 1e9, newDp1 = 1e9;

        ll prev = min(dp0, dp1);
        if (prev <= a[i])
            newDp0 = a[i];

        ll l = bMin - a[i];
        ll r = bMax - a[i];

        if (dp0 != 1e9)
        {
            ll ans = max(dp0, l);
            if (ans <= r)
                newDp1 = min(newDp1, ans);
        }

        if (dp1 != 1e9)
        {
            ll ans = max(dp1, l);
            if (ans <= r)
                newDp1 = min(newDp1, ans);
        }

        dp0 = newDp0;
        dp1 = newDp1;
    }
    if (min(dp0, dp1) == 1e9)
        cout << "NO" << "\n";
    else
        cout << "YES" << "\n";
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