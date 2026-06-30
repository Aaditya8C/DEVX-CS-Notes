void bubbleSort(int arr[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        bool swapped = false; // optimization: check if array is already sorted

        for (int j = 0; j < n - i - 1; j++) // last i elements are already in place
        {
            if (arr[j] > arr[j + 1])
            {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }

        if (!swapped)
            break; // no swaps → array is sorted
    }
}
