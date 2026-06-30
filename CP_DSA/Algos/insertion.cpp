void sort()
{
    for (int i = 0; i < size - 1; i++) // critical: should run till size - 1 to avoid arr[i + 1] overflow
    {
        int key = arr[i + 1];
        for (int j = i; j >= 0; j--)
        {
            if (key < arr[j])
            {
                // shift elements to the right to insert key
                arr[j + 1] = arr[j];
                arr[j] = key;
            }
        }
    }
}
