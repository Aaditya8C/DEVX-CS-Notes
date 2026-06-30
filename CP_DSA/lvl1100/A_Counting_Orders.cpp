

#include <bits/stdc++.h>
#include <iostream>
#include <vector>
#define rep(i, n) for (int i = 0; i < (int)(n); ++i)             // 0 based indexing
#define rep1(i, n) for (int i = 1; i < (int)(n); ++i)            // 0 based indexing starts with 1
#define rep1n(i, n) for (int i = 1; i <= (int)(n); ++i)          // 1 based indexing
#define forc(i, l, r) for (int i = (int)(l); i <= (int)(r); ++i) // closed interver from l to r r inclusive
#define rrep(i, n) for (int i = (int)(n) - 1; i >= 0; --i)       // reverse 0 based.
#define rrep1(i, n) for (int i = (int)(n); i >= 1; --i)          // reverse 1 based

#define pb push_back
#define fi first
#define se second
#define sp " "
#define nl "\n"

#define all(x) (x).begin(), (x).end()  // Forward traversal
#define rall(x) (x).rbegin, (x).rend() // reverse traversal

#define tr(c, i) for (__typeof__((c)).begin() i = (c).begin(); i != (c).end(); i++)

#define present(c, x) ((c).find(x) != (c).end())

#define cpresent(c, x) (find(all(c), x) != (c).end())

#define sz(a) int((a).size())

#define ceil_div(a, b) (((a) + ((b) - 1)) / (b))

using namespace std;

typedef vector<int> vi;
typedef vector<vi> vvi;
typedef pair<int, int> ii;
typedef vector<ii> vii;
typedef long long ll;
typedef vector<ll> vll;
typedef vector<vll> vvll;
typedef double ld;
const int MOD = 1e9 + 7;
void solve()
{

    int n;
    cin >> n;
    vi a(n), b(n);
    rep(i, n) cin >> a[i];
    rep(i, n) cin >> b[i];
    sort(all(a));
    sort(all(b), greater<>());
    ll res = 1;
    rep(i, n)
    {
        int count = sz(a) - (upper_bound(all(a), b[i]) - a.begin());
        res = res * max(count - i, 0) % MOD;
    }
    cout << res << nl;
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
