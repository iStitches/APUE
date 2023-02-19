#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

#define RADIX 10   //基数,默认为10

//获取最大位数
int GetMaxDigit(int *arr, size_t n) {
    int d = 1;
    size_t base = 10;
    for (int i = 0; i < n; i++) {
        while (arr[i] >= base) {
            base *= 10;
            ++d;
        }
    } 
    return d;
}

//获取数据指定位的数字
inline int GetDigitInPos(int num, int pos) \
{
    int temp = 1;
    for (int i = 0; i < pos - 1; i++)
    {
        temp *= 10;
    }
    return (num / temp) % 10;
}

//串行基数排序
inline void RadixSort(int *unorderArray, size_t n)
{   
    int *bucket[RADIX];
}

