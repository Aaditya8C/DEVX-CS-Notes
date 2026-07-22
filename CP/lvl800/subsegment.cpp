#include <iostream>
#include <vector>
// #include <algorithm>
#include <set>
#include <map>
using namespace std;
int main()
{
    int T;
    int n, k;
    cin >> T;
    while (T--)
    {
        cin >> n >> k;
        vector<int> nums(n);
        map<int, int> mp;
        for (int i = 0; i < n; i++)
        {
            cin >> nums[i];
            mp[nums[i]]++;
        }
        set<pair<int, int>> st;
        for (auto elem : mp)
        {
            st.insert({elem.second, elem.first});
        }
        if (st.rbegin()->second == k || mp.count(k))
            cout << "Yes" << endl;
        else
            cout << "No" << endl;
    }
}