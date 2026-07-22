#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

void solve(int n, int k)
{
    vector<int> nums(n);
    for (int i = 0; i < n; i++)
        cin >> nums[i];

    vector<int> temp = nums;
    sort(temp.begin(), temp.end());
    if (temp == nums && k > 1)
        cout << "YES" << endl;
    else
        cout << "NO" << endl;
}
int main()
{
    int T;
    cin >> T;
    int n, k;
    while (T--)
    {
        cin >> n >> k;
        solve(n, k);
    }
    return 0;
}