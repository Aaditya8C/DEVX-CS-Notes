void mergeSort(int *arr, int l, int r)
{
    if (l >= r)
        return;
    int mid = (l + r) / 2;
    mergeSort(arr, l, mid);     // recursively sort left half
    mergeSort(arr, mid + 1, r); // recursively sort right half
    merge(arr, l, mid, r);      // merge the two sorted halves
}

void merge(int *arr, int l, int mid, int r)
{
    int subArrOne = mid - l + 1;
    int subArrTwo = r - mid;

    // create temp arrays to hold copies of the split halves
    int temp1[subArrOne];
    int temp2[subArrTwo];

    for (int i = 0; i < subArrOne; i++)
    {
        temp1[i] = arr[l + i];
    }

    for (int i = 0; i < subArrTwo; i++)
    {
        temp2[i] = arr[mid + 1 + i];
    }

    int i = 0;
    int j = 0;
    int k = l; // starting index for merging back into original array

    // merge temp arrays into original array in sorted order
    while (i < subArrOne && j < subArrTwo)
    {
        if (temp1[i] < temp2[j])
        {
            arr[k] = temp1[i];
            k++;
            i++;
        }
        else
        {
            arr[k] = temp2[j];
            k++;
            j++;
        }
    }

    // copy any remaining elements
    while (i < subArrOne)
    {
        arr[k] = temp1[i];
        k++;
        i++;
    }
    while (j < subArrTwo)
    {
        arr[k] = temp2[j];
        k++;
        j++;
    }
}

// void display()
// {
//     mergeSort(arr, 0, size - 1);
//     for (int i = 0; i < size; i++)
//         cout << arr[i] << endl;
// }
