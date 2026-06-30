#include <iostream>
#include <vector>
using namespace std;
int main()
{
    int T;
    cin >> T;
    while (T--)
    {
        int n;
        cin >> n;
        vector<int> nums;
        for (int i = 0; i < n; i++)
        {
            int x;
            cin >> x;
            if (i && nums.back() > x)
                nums.push_back(1);
            nums.push_back(x);
        }
        cout << nums.size() << endl;
        for (auto elem : nums)
            cout << elem << " ";
        cout << endl;
    }
    return 0;
}