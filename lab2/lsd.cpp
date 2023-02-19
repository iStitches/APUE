#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <omp.h>
#include <malloc.h>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <sys/time.h>
#include <vector>

#define T 16
#define NUM_MAX 100000000    //排序数据个数
#define RADIX 10             //排序基数
#define LEN_MAX 10           //数据最大位数

using namespace std;

void sort_gen(int *d,int N,int seed){
	srand(seed);
	for(int i=0;i<N;i++){
		d[i]=rand();
	}
}

const int digit[10] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

//串行lsd排序 (先计算所有前缀和版本)
void LSDSerailSort1(int *arr, int n) {
    //临时数组存储
    int *tmp = (int*) calloc(n, sizeof(int));
    int i, j, m, num, index;

    //数据位存储
    int count[LEN_MAX][RADIX];
    for (i = 0; i < LEN_MAX; i++) {
        for (j = 0; j < RADIX; j++) {
            count[i][j] = 0;
        }
    }
    //整体统计各数每一位数字个数，count[i][j] ：某数字第i位上值为j的个数
    for (i = 0; i < n; i++) {
        for (j = 0; j < LEN_MAX; j++) {
            num = digit[j];
            m = (arr[i] / num) % RADIX;
            count[j][m]++;
        }
    }
    //计算前缀和
    for (i = 0; i < LEN_MAX; i++) {
        for (j = 1; j < RADIX; j++) {
            count[i][j] += count[i][j-1];
        }
    }
    //重新收集
    for (i = 0; i < LEN_MAX; i++) {
        num = digit[i];
        for (j = n - 1; j >= 0; j--) {
            m = (arr[j] / num) % RADIX;  //每个数第i位上的值
            index = --count[i][m];
            tmp[index] = arr[j];
        }
        for (j = 0; j < n; j++) {  //更新后的arr用于下一次的数值统计排序
            arr[j] = tmp[j];
        }
    }
    free(tmp);
}

//串行lsd排序+cache优化  (先计算所有前缀和版本)
void LSDSerialCacheSort1(int *arr, int n) {
    //临时数组存储
    int *tmp = (int*) calloc(n, sizeof(int));
    int i, j, m, num, index, k;

    //数据位存储
    int count[LEN_MAX][RADIX];
    for (i = 0; i < LEN_MAX; i++) {
        for (j = 0; j < RADIX; j++) {
            count[i][j] = 0;
        }
    }

    //整体统计各数每一位数字个数，count[i][j] ：某数字第i位上值为j的个数
    for (i = 0; i < n; i++) {
        for (j = 0; j < LEN_MAX; j++) {
            num = digit[j];
            m = (arr[i] / num) % RADIX;
            count[j][m]++;
        }
    }
    //计算前缀和
    for (i = 0; i < LEN_MAX; i++) {
        for (j = 1; j < RADIX; j++) {
            count[i][j] += count[i][j-1];
        }
    }

    //重新收集,加入cache优化
    int buf_length = 8096 / RADIX;   //每个桶的缓冲长度
    int buf_buckets[RADIX][buf_length]; //缓冲桶,第0位记录元素个数
    memset(buf_buckets, 0, RADIX * buf_length * sizeof(int));
    for (i = 0; i < LEN_MAX; i++) {
        num = digit[i];
        for (j = n - 1; j >= 0; j--) {
            m = (arr[j] / num) % RADIX;  //每个数第i位上的值
            buf_buckets[m][0]++;
            buf_buckets[m][buf_buckets[m][0]] = arr[j];
            if (buf_buckets[m][0] == buf_length - 1) {
                // cout<<"size:"<<buf_buckets[m][0]<<endl;
                for (k = 1; k <= buf_buckets[m][0]; k++) {
                    index = --count[i][m];
                    // if (index < 0)
                        // cout<<"index:"<<index<<endl;
                    tmp[index] = buf_buckets[m][k];
                }
                buf_buckets[m][0] = 0;
            }
        }

        for (k = 0; k < RADIX; k++) {
            if (buf_buckets[k][0] > 0) {
                for (j = 1; j <= buf_buckets[k][0]; j++) {
                    index = --count[i][k];
                    tmp[index] = buf_buckets[k][j];
                }
                buf_buckets[k][0] = 0;
            }
        }
        for (j = 0; j < n; j++) {  //更新后的arr用于下一次的数值统计排序
            arr[j] = tmp[j];
        }
    }
    free(tmp);
}

//串行lsd排序 (每一轮计算前缀和)
void LSDSerialSort2(int *arr, int n) {
    int *tmp = (int*) calloc(NUM_MAX, sizeof(int));
    int base = 1;
    int i, j, k, index;
    int buckets[RADIX];
    memset(buckets, 0, RADIX * sizeof(int));
    for (i = 0; i < RADIX; i++) {
        //扫描数据每一位
        for (j = 0; j < n; j++) {
            k = arr[j] / base % RADIX;
            buckets[k]++;
        }

        //计算前缀和
        for (j = 1; j < RADIX; j++) {
            buckets[j] += buckets[j - 1];
        }

        //重新收集
        for (j = n - 1; j >= 0; j--) {
            k = arr[j] / base % RADIX;
            index = --buckets[k];
            tmp[index] = arr[j];
        }

        //写回覆盖
        for (j = 0; j < n; j++) {
            arr[j] = tmp[j];
        }
        base *= 10;
        memset(buckets, 0, RADIX * sizeof(int));
    }
    free(tmp);
}

//串行lsd排序+cache优化 (每一轮计算前缀和)
void LSDSerialCacheSort2(int *arr, int n) {
    int *tmp = (int*) calloc(NUM_MAX, sizeof(int));
    int base = 1;
    int i, j, k, m, index;
    int buckets[RADIX];
    int buf_length = 8096 / RADIX;
    int buf_buckets[RADIX][buf_length];
    memset(buckets, 0, RADIX * sizeof(int));
    memset(buf_buckets, 0, RADIX * buf_length * sizeof(int));
    for (i = 0; i < RADIX; i++) {
        //扫描数据每一位
        for (j = 0; j < n; j++) {
            k = arr[j] / base % RADIX;
            buckets[k]++;
        }

        //计算前缀和
        for (j = 1; j < RADIX; j++) {
            buckets[j] += buckets[j - 1];
        }

        //重新收集
        for (j = n - 1; j >= 0; j--) {
            k = arr[j] / base % RADIX;
            
            //首先添加进缓存
            buf_buckets[k][0]++;
            buf_buckets[k][buf_buckets[k][0]] = arr[j];
            if (buf_buckets[k][0] == buf_length - 1) {
                for (m = 1; m <= buf_length - 1; m++) {
                    index = --buckets[k];
                    tmp[index] = buf_buckets[k][m];
                }
                buf_buckets[k][0] = 0;
            }
        }

        //缓存剩余部分再保存
        for (j = 0; j < RADIX; j++) {
            if (buf_buckets[j][0] > 0) {
                for (m = 1; m <= buf_buckets[j][0]; m++) {
                    index = --buckets[j];
                    tmp[index] = buf_buckets[j][m];
                }
                buf_buckets[j][0] = 0;
            }
        }

        //写回覆盖
        for (j = 0; j < n; j++) {
            arr[j] = tmp[j];
        }
        base *= 10;
        memset(buckets, 0, RADIX * sizeof(int));
    }
    free(tmp);
}

//lsd-openmp
void LSDSortOMP(int *arr, size_t n) {
    int *tmp = (int*) calloc(NUM_MAX, sizeof(int));
    int i, j, index, thread_id, k;
    int base = 1;
    size_t per_count = n/T;   //每个线程处理数据个数
    int count[T][10];
    int start[T][10];
    int all_count[10];
    memset(count, 0, 10*T*sizeof(int));
    memset(start, 0, 10*T*sizeof(int));
    memset(all_count, 0, 10 * sizeof(int));

    // step1. 扫描原始数据,计算每个桶的元素数
    for (i = 0; i < LEN_MAX; i++) {

        //并发扫描原始数据
        omp_set_num_threads(T);
        #pragma omp parallel for private(index, thread_id) shared(count) schedule(static, per_count)
        for (j = 0; j < n; j++) {
            index = arr[j] / base % 10;
            thread_id = omp_get_thread_num();
            count[thread_id][index]++;
        }

        //总数字统计
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < T; k++) {
                all_count[j] += count[k][j];
            }
        }

        //统计第0个线程起始地址(前缀和)
        for (j = 1; j < 10; j++) {
            start[0][j] = all_count[j-1] + start[0][j-1];
        }
        //计算每个线程中桶的起始地址
        for (j = 1; j < T; j++) {
            for (k = 0; k < 10; k++) {
                start[j][k] = start[j-1][k] + count[j-1][k];
            }
        }

        //从桶中重新排序
        // omp_set_num_threads(T);
        #pragma omp parallel for shared(tmp) private(index, thread_id) schedule(static ,per_count)
        for (j = 0; j < n; j++) {
            index = arr[j] / base % 10;
            thread_id = omp_get_thread_num();
            tmp[start[thread_id][index]++] = arr[j];
        }
        base *= 10;
        memcpy(arr, tmp, n * sizeof(int));
        memset(count, 0, 10*T*sizeof(int));
        memset(start, 0, 10*T*sizeof(int));
        memset(all_count, 0, 10 * sizeof(int));
    }
    free(tmp);
}

//lsd-cache-openmp
void LSDSortOMPCache(int *arr, size_t n) {
    int *tmp = (int*) calloc(NUM_MAX, sizeof(int));
    int i, j, index, thread_id, k;
    int base = 1;
    int per_count = n/T;   //每个线程处理数据个数
    int count[T][10];      //每个线程的计数
    int start[T][10];      //每个线程的起始插入地址
    int all_count[10];     //总计数

    memset(count, 0, 10*T*sizeof(int));
    memset(start, 0, 10*T*sizeof(int));
    memset(all_count, 0, 10 * sizeof(int));

    // step1. 扫描原始数据,计算每个桶的元素数
    for (i = 0; i < LEN_MAX; i++) {

        //并发扫描原始数据
        omp_set_num_threads(T);
        #pragma omp parallel for private(index, thread_id) shared(count) schedule(static, per_count)
        for (j = 0; j < n; j++) {
            index = arr[j] / base % 10;
            thread_id = omp_get_thread_num();
            count[thread_id][index]++;
        }

        //总数字统计
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < T; k++) {
                all_count[j] += count[k][j];
            }
        }

        //统计第0个线程起始地址(前缀和)
        for (j = 1; j < 10; j++) {
            start[0][j] = all_count[j-1] + start[0][j-1];
        }
        //计算每个线程中桶的起始地址
        for (j = 1; j < T; j++) {
            for (k = 0; k < 10; k++) {
                start[j][k] = start[j-1][k] + count[j-1][k];
            }
        }

        //从桶中重新排序
        // omp_set_num_threads(T);
        #pragma omp parallel for shared(tmp) private(index, thread_id) schedule(static ,per_count)
        for (j = 0; j < n; j++) {
            index = arr[j] / base % 10;
            thread_id = omp_get_thread_num();
            tmp[start[thread_id][index]++] = arr[j];
        }
        base *= 10;
        memcpy(arr, tmp, n * sizeof(int));
        memset(count, 0, 10*T*sizeof(int));
        memset(start, 0, 10*T*sizeof(int));
        memset(all_count, 0, 10 * sizeof(int));
    }
    free(tmp);
}

// void radix_sor_gpt(int *arr, int n, int max_digits) {
//     int *temp = new int[n];
//     int *count = new int[256];
//     int shift = 0;
//     int mask = 0xFF;
//     for (int d = 0; d < max_digits; ++d) {
//         shift = d << 3;
//         #pragma omp parallel for
//         for (int i = 0; i < 256; ++i) {
//             count[i] = 0;
//         }
//         #pragma omp parallel for
//         for (int i = 0; i < n; ++i) {
//             count[(arr[i] >> shift) & mask]++;
//         }
//         #pragma omp parallel for
//         for (int i = 1; i < 256; ++i) {
//             count[i] += count[i-1];
//         }
//         for (int i = n-1; i >= 0; --i) {
//             temp[--count[(arr[i] >> shift) & mask]] = arr[i];
//         }
//         for (int i = 0; i < n; ++i) {
//             arr[i] = temp[i];
//         }
//     }
//     delete[] temp;
//     delete[] count;
// }

void Print(int* arr, int n)
{
    int incr = n / 10;
	for (int i = 0; i < n; i+=incr)
	{
		cout << arr[i] << " ";
	}
	cout << endl;
}

#define TIME_CAL(CODE_TO_CAL) \
{\
    struct timeval start;\
    struct timeval end;\
    double diff;\
    gettimeofday(&start, NULL);\
    CODE_TO_CAL;\
    gettimeofday(&end, NULL);\
    diff = double(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec)/1000000.0;\
    cout<<diff<<endl;\
}

#define OMP_TIME_CAL(CODE_TO_CAL)\
{\
    double start = omp_get_wtime();\
    CODE_TO_CAL;\
    double end = omp_get_wtime();\
    cout<<end-start<<endl;\
}

int main() {
    // int arr[5] = { 1205554746, 483147985, 844158168, 953350440, 612121425 };
    // int arr1[10] = { 123, 234, 543, 324, 568, 975, 547, 672, 783, 239 };
    int *arr1 = (int*)memalign(32, sizeof(int)*NUM_MAX);
    int *arr2 = (int*)memalign(32, sizeof(int)*NUM_MAX);
    int *arr3 = (int*)memalign(32, sizeof(int)*NUM_MAX);
    int *arr4 = (int*)memalign(32, sizeof(int)*NUM_MAX);
    int *arr5 = (int*)memalign(32, sizeof(int)*NUM_MAX);
    int *arr6 = (int*)memalign(32, sizeof(int)*NUM_MAX); 
    sort_gen(arr1, NUM_MAX, 0.3);
    sort_gen(arr2, NUM_MAX, 0.3);
    sort_gen(arr3, NUM_MAX, 0.3);
    sort_gen(arr4, NUM_MAX, 0.3);
    sort_gen(arr5, NUM_MAX, 0.3);
    sort_gen(arr6, NUM_MAX, 0.3);

    OMP_TIME_CAL(sort(arr1, arr1 + NUM_MAX));
    Print(arr1, NUM_MAX);
	// OMP_TIME_CAL(LSDSerailSort1(arr2, NUM_MAX));
    // Print(arr2, NUM_MAX);
    // OMP_TIME_CAL(LSDSerialCacheSort1(arr3, NUM_MAX));
    // Print(arr3, NUM_MAX);
    OMP_TIME_CAL(LSDSerialSort2(arr3, NUM_MAX));
    Print(arr3, NUM_MAX);
    OMP_TIME_CAL(LSDSerialCacheSort2(arr4, NUM_MAX));
    Print(arr4, NUM_MAX);
    OMP_TIME_CAL(LSDSortOMP(arr6, NUM_MAX));
    Print(arr6, NUM_MAX);
}