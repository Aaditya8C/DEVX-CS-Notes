#include <iostream>
using namespace std;
int main()
{
    int T;
    cin >> T;
    int n;
    string s;

    while (T--)
    {
        int maxDotsSize = 0;
        int totalDots = 0;
        int currentDot = 0;
        cin >> n;
        cin >> s;

        for (int i = 0; i < n; i++)
        {
            if (s[i] == '.')
            {
                totalDots++;
                currentDot++;
                maxDotsSize = max(maxDotsSize, currentDot);
            }
            else
                currentDot = 0;
        }

        if (maxDotsSize > 2)
            cout << 2 << endl;
        else
            cout << totalDots << endl;
    }
}