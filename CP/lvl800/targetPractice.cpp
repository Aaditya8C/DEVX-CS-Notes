#include <iostream>
#include <vector>
using namespace std;
int main()
{
    int T;
    cin >> T;
    vector<vector<int>> mat(10, vector<int>(10, 0));
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (i == 0 || i == 9 || j == 0 || j == 9)
                mat[i][j] = 1;
        }
    }
    for (int i = 1; i < 9; i++)
    {
        for (int j = 1; j < 9; j++)
        {
            int minVal = min(i, j);
            minVal = min(minVal, 10 - i - 1);
            minVal = min(minVal, 10 - j - 1);
            mat[i][j] = minVal + 1;
        }
    }
    while (T--)
    {
        vector<vector<char>> grid(10, vector<char>(10, '.'));
        int res = 0;

        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                cin >> grid[i][j];
                if (grid[i][j] == 'X')
                    res += mat[i][j];
            }
        }
        cout << res << endl;
    }
}