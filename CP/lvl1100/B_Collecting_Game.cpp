#include <bits/stdc++.h>
#include <iostream>
#include <vector>
#define for0(i, n) for (int i = 0; i < (int)(n); ++i)            // 0 based indexing
#define for1(i, n) for (int i = 1; i < (int)(n); ++i)            // 0 based indexing starts with 1
#define for11(i, n) for (int i = 1; i <= (int)(n); ++i)          // 1 based indexing
#define forc(i, l, r) for (int i = (int)(l); i <= (int)(r); ++i) // closed interver from l to r r inclusive
#define forr0(i, n) for (int i = (int)(n) - 1; i >= 0; --i)      // reverse 0 based.
#define forr1(i, n) for (int i = (int)(n); i >= 1; --i)          // reverse 1 based

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

void solve()
{
    int n;
    cin >> n;
    ii arr[n + 1];
    for (int i = 1; i <= n; i++)
        cin >> arr[i].fi, arr[i].se = i;
    sort(arr + 1, arr + n + 1);
    int nxt[n + 1];
    ll sum[n + 1];
    int ans[n + 1];
    nxt[0] = sum[0] = 0;
    for (int i = 1; i <= n; i++)
    {
        if (nxt[i - 1] >= i)
        {
            nxt[i] = nxt[i - 1];
            sum[i] = sum[i - 1];
        }
        else
        {
            sum[i] = sum[i - 1] + arr[i].fi;
            nxt[i] = i;
            while (nxt[i] + 1 <= n && sum[i] >= arr[nxt[i] + 1].fi)
            {
                nxt[i]++;
                sum[i] += arr[nxt[i]].fi;
            }
        }
        ans[arr[i].se] = nxt[i];
    }
    for (int i = 1; i <= n; i++)
        cout << ans[i] - 1 << " ";
    cout << endl;
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