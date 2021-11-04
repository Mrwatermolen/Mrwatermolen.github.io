#include <stdio.h>

int SearchSeq(int array[], int key, int length)
{
    array[0] = key;
    for (int i = length; array[i] != key; --i)
        ;
    return 0;
}

int SearchBin(int array[], int key, int length)
{
    int low = 1, height = length;
    while (low < height)
    {
        int mid = ((height - low) >> 1) + low;
        if (array[mid] == key)
        {
            return mid;
        }
        else if (array[mid] < key)
        {
            low = mid + 1;
        }
        else
        {
            height = mid + 1;
        }
    }
    return 0;
}