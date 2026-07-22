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

        for (int i = 0; i < n; i++)
            cin >> nums[i];

        if (nums[0] == 1)
            cout << "YES" << endl;
        else
            cout << "NO" << endl;
    }
}