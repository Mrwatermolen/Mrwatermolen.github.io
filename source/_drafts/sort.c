#include <stdio.h>

void InsertSort(int array[], int len)
{
    //得到递增序列
    for (int i = 2; i <= len; ++i)
    {
        array[0] = array[i];
        int j = i - 1;
        for (; array[0] < array[j]; --j)
        {
            array[j + 1] = array[j];
        }
        array[j + 1] = array[0];
    }
}

void InsertBinSort(int array[], int len)
{
    //得到递增序列
    for (int i = 2; i <= len; ++i)
    {
        array[0] = array[i];
        int high = i - 1, low = 1;
        while (low <= high)
        {
            int mid = low + ((high - low) >> 1);
            if (array[mid] > array[0])
            {
                //high最后永远指向小于等于array[0]的最后一个元素
                high = mid - 1; //保证递增序列的稳定性，相等的元素后判断的要插入后半区
            }
            else
            {
                low = mid + 1;
            }
        }
        for (int j = i - 1; j > high; --j)
        {
            array[j + 1] = array[j];
        }
        array[high + 1] = array[0];
    }
}

void ShellSort(int array[], int len, int dlta[], int dLen)
{
    void ShellInsert(int array[], int len, int d);
    //dlta 0～dLen-1  array 1~len
    for (int k = 0; k < dLen; ++k)
    {
        ShellInsert(array, len, dlta[k]);
    }
}

void ShellInsert(int array[], int len, int d)
{
    //不对子表单独处理，而是一起处理
    for (int i = d + 1; i <= len; ++i)
    {
        if (array[i] < array[i - d])
        {
            array[0] = array[i - d];
            int j = i - d;
            for (; j > 0 && array[j] < array[0]; j -= d) //这个大于0很重要，因为[0]已经不是哨兵了
            {
                array[j + d] = array[j];
            }
            array[j + d] = array[0];
        }
    }
}

void Swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}
void BubbleSort(int array[], int len)
{
    for (int i = 1; i < len; ++i)
    {
        for (int j = 1; j < len - i + 1; ++j)
        {
            if (array[j] > array[j + 1])
            {
                Swap(&array[j], &array[j + 1]);
            }
        }
    }
}

void QuickSort(int array[], int low, int high)
{
    int Partiton(int array[], int low, int high);
    if (low < high)
    {
        int mid = Partiton(array, low, high);
        QuickSort(array, low, mid - 1);
        QuickSort(array, mid + 1, high);
    }
}

int Partiton(int array[], int low, int high)
{
    int privot = array[low];
    while (low < high)
    {
        while (low < high && array[high] >= privot)
        {
            --high;
        }
        //Swap(&array[high],&array[low]);
        array[low] = array[high]; //不用交换是因为第一次交换时low就是privot的位置这个位子本来就能被代替，没代替一次就产生一个废位子
        while (low < high && array[low] <= privot)
        {
            ++low;
        }
        //Swap(&array[high],&array[low]);
        array[high] = array[low];
    }
    array[low] = privot; //没有使用交换记得最后把privot移回去
    return low;
}

void SelcetSort(int array[], int len)
{
    for (int i = 1; i < len; ++i)
    {
        int min = i;
        for (int j = i + 1; j <= len; ++j)
        {
            if (array[min] > array[j])
            {
                min = j;
            }
        }
        if (min != i)
        {
            Swap(array + i, array + min);
        }
    }
}

void HeapSort(int array[], int len)
{
    void HeadAdjust(int array[], int len, int k);
    for (int i = len / 2; i > 0; --i)
    {
        HeadAdjust(array, len, i);
    }
    for (int i = 1; i < len; ++i)
    {
        Swap(array + 1, array + len - i + 1);
        HeadAdjust(array, len - i, 1);
    }
}

void HeadAdjust(int array[], int len, int k)
{
    int son = 2 * k;
    while (son <= len)
    {
        if (son + 1 <= len && array[son] < array[son + 1])
            ++son;
        if (array[son] < array[k])
            return;
        Swap(array + son, array + k);
        k = son;
        son = 2 * k;
    }
}

int main()
{
    int arr[] = {9999, 3, 5, 3, 0, 8, 6, 1, 5, 8, 6, 2, 4, 9, 4, 7, 0, 1, 8, 9, 7, 3, 1, 2, 5, 9, 7, 4, 0, 2, 6};
    int len = sizeof(arr) / sizeof(int) - 1;
    BubbleSort(arr, len);
    for (int i = 1; i <= len; ++i)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
}
