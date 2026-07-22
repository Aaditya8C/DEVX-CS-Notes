#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
using namespace std;

void solve(int n, int x)
{
    vector<int> nums(n);
    for (int i = 0; i < n; i++)
        cin >> nums[i];
    int ans = 0;
    int prev = 0;

    for (int i = 0; i < nums.size(); i++)
    {
        ans = max(ans, nums[i] - prev);
        prev = nums[i];
    }
    ans = max(ans, 2 * (x - prev));
    cout << ans << endl;
}
int main()
{
    int T;
    cin >> T;
    int n, x;
    while (T--)
    {
        cin >> n >> x;
        solve(n, x);
    }
    return 0;
}