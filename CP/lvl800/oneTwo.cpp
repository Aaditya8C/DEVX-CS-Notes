#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
int main()
{
    int T;
    cin >> T;
    int n;

    while (T--)
    {
        cin >> n;
        vector<int> nums(n);
        vector<long long> forward(n + 1, 1);
        vector<long long> backward(n + 1, 1);
        int res = -1;

        for (int i = 0; i < n; i++)
        {
            cin >> nums[i];
        }

        for (int i = 1; i <= n; i++)
        {
            forward[i] = forward[i - 1] * static_cast<long long>(nums[i - 1]);
        }

        for (int i = n - 1; i >= 0; i--)
        {
            backward[i] = static_cast<long long>(nums[i]) * backward[i + 1];
        }

        for (int i = 1; i < n; i++)
        {
            if (forward[i] == backward[i])
            {
                res = i;
                break;
            }
        }
        cout << res << endl;
    }
}