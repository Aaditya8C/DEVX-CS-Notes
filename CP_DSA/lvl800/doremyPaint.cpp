#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
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
        map<int, int> mp;

        for (int i = 0; i < n; i++)
        {
            cin >> nums[i];
            mp[nums[i]]++;
        }

        if (mp.size() >= 3)
            cout << "No" << endl;
        else
        {
            if ((abs(mp.begin()->second - mp.rbegin()->second)) <= 1) // any number should appear only n/2 times in array
                cout << "YES" << endl;
            else
                cout << "NO" << endl;
        }
    }
}