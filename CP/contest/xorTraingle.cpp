#include <iostream>
using namespace std;

void solve()
{
    int x;
    cin >> x;

    int y = -1;
    for (int i = 1; i < x; i++)
    { // iterate over possible values of y
        int xy = x ^ i;
        if (xy > 0 && (x + i > xy) && (x + xy > i) && (i + xy > x))
        {
            y = i;
            break;
        }
    }

    cout << y << "\n";
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--)
    {
        solve();
    }
    return 0;
}
