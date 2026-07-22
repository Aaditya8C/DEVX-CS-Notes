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

const string seq[] = {"00", "25", "50", "75"};
int solve(string &s, string &elem)
{

    int sptr = s.size() - 1;
    int res = 0;
    while (sptr >= 0 && s[sptr] != elem[1])
    {
        sptr--;
        res++;
    }

    if (sptr < 0)
    {
        return 1e9;
    }
    sptr--;

    while (sptr >= 0 && s[sptr] != elem[0])
    {
        sptr--;
        res++;
    }

    if (sptr < 0)
        return 1e9;
    else
        return res;
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
        string s;
        cin >> s;
        int minAns = 1e9;
        for (auto elem : seq)
        {
            minAns = min(minAns, solve(s, elem));
        }
        cout << minAns << "\n";
    }
    return 0;
}