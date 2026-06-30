#include <iostream>
using namespace std;
int main()
{
    int T;
    cin >> T;
    int n;

    while (T--)
    {
        cin >> n;
        int rem = n % 3;

        if (rem == 1 || rem == 2)
            cout << "First" << endl;
        else
            cout << "Second" << endl;
    }
}