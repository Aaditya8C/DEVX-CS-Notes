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
    ll n;
    cin >> n;
    vll s(n), p(n);
    for0(i, n) cin >> s[i];
    for0(i, n) p[i] = i + 1;

    int i = 0;
    while (i < n)
    {
        ll curSize = s[i];

        // finding range for curr shoe size
        ll l = i;
        ll r = i;
        while (r < n && s[r] == curSize)
            r++;

        // unique shoe size
        if (l == r - 1)
        {
            cout << -1 << "\n";
            return;
        }
        // rotating this grp cyclically left by 1
        // f m     e     - first, middle, one past last elements
        // 6 6 6 6 7 7
        rotate(p.begin() + l, p.begin() + l + 1, p.begin() + r);

        i = r;
    }

    for (auto elem : p)
        cout << elem << sp;
    cout << "\n";
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