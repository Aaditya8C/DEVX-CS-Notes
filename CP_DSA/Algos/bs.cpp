int binarySearch(vector<int> &arr, int target)
{
    int l = 0, r = arr.size() - 1;
    while (l <= r)
    {
        int m = (l + r) / 2;
        if (arr[m] == target)
            return m;
        else if (arr[m] < target)
            l = m + 1;
        else
            r = m - 1;
    }
    return -1;
}
