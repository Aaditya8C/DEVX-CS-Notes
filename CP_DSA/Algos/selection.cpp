void selectionSort(int arr[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        int minIdx = i;

        // find the index of the minimum element in the unsorted part
        for (int j = i + 1; j < n; j++)
        {
            if (arr[j] < arr[minIdx])
                minIdx = j;
        }

        if (minIdx != i)
            swap(arr[i], arr[minIdx]); // place the min element at the current position
    }
}

best case:
O(n²)

average case:
O(n²)

worst case:
O(n²)
