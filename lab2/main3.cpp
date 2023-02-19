#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <omp.h>
#include <malloc.h>
#include <iostream>
#include<algorithm>

#define DIGITS_AMOUNT 	10    /* 每一位上有十个数 排序基数*/
#define KEY_AMOUNT 10   //关键字位数，这里为整形位数  最大数为2147483647(十进制)
#define DATA_LENGTH 1000000000   //数据长度
#define THREAD_NUM 2          //线程个数



#define TIME_CALCULATION(CODE_TO_CAL) \
{    \
    clock_t start = omp_get_wtime();\
    CODE_TO_CAL; \
    clock_t end = omp_get_wtime();\
    std::cout << (double)(end - start) <<"秒"<< std::endl;\
}

int *array1 = (int *)memalign(32,sizeof(float)*DATA_LENGTH);
int *array2 = (int *)memalign(32,sizeof(float)*DATA_LENGTH);
int *array3 = (int *)memalign(32,sizeof(float)*DATA_LENGTH);
void sort_gen(int *d,int N,int seed);

//数组初始化
void sort_gen(int *d,int N,int seed){
	srand(seed);
	for(int i=0;i<N;i++){
		d[i]=rand();
	}
}


int tenPowers[10] = {
	1,		10,		100,
	1000,		10000,		100000,
	1000000,	10000000,	100000000,
	1000000000,
};

/*
 * Gives power of 10 at certain exp
 */
int p(int exp)
{
	return tenPowers[exp];
}

//串行基数排序
void radixsort_serial(int *input_array, int input_array_size,int key_size){
    int digit;
	int i,j,k;
    
    int *tmp = (int *)calloc(input_array_size, sizeof(int));//临时数组
    
    int shared_buckets[key_size][DIGITS_AMOUNT];     //关键字最多10位 10进制下每一位有10个不同的数(桶)
    
    for (i=0; i<key_size; i++){                      //所有桶初始化
		for(j=0; j<DIGITS_AMOUNT; j++){
			shared_buckets[i][j]=0;    
        }
    }        

    for (i=0; i<input_array_size; i++) {

		//对于一个数据 该数据的每一位进桶分配
		for (j=0; j<key_size; j++) {
			digit = p(j);  //j=0 p(j)=1; j=2 p(j)=100
			k=input_array[i]/digit%DIGITS_AMOUNT;  //k是一个数据在第j位的值
			shared_buckets[j][k]++;
		}
	}

    //每一位上的桶计算前缀和
	for (j=0; j<key_size; j++) {
		for (i=1; i<DIGITS_AMOUNT; i++)
			shared_buckets[j][i]+=shared_buckets[j][i-1];
	}

    //从低位到高位收集
    for (j=0; j<key_size; j++) {
		digit = p(j);

		for(i=input_array_size-1;i>=0;i--) {
			int unit = input_array[i]/digit%DIGITS_AMOUNT;
			int pos = --shared_buckets[j][unit];
			tmp[pos]=input_array[i];
		}

		
		for(i=0;i<input_array_size;i++){
			input_array[i]=tmp[i];
        }
		
	}   
    free(tmp);   
}
//并行基数排序
void radixsort_parallel(int *input_array, int input_array_size,int key_size,int chunksize){
    int digit;
	int i,j,k;
    int *tmp = (int *)calloc(input_array_size, sizeof(int));
    int shared_buckets[key_size][DIGITS_AMOUNT];     //关键字最多10位 10进制下每一位有10个不同的数(桶)

    for (i=0; i<key_size; i++){                      //所有桶初始化
		for(j=0; j<DIGITS_AMOUNT; j++){
			shared_buckets[i][j]=0;    
        }
    }

    //用多个线程在不同的位数分配
    #pragma omp parallel for private(digit, i, j, k) num_threads(THREAD_NUM) schedule(dynamic, chunksize)
    for (i=0; i<input_array_size; i++) {

		//对于一个数据 该数据的每一位进桶分配
		for (j=0; j<key_size; j++) {
			digit = p(j);
			k=input_array[i]/digit%DIGITS_AMOUNT;
			#pragma omp atomic
			shared_buckets[j][k]++;
		}
	}

    //所有桶计算前缀和
    for (j=0; j<key_size; j++) {
		for (i=1; i<DIGITS_AMOUNT; i++)
			shared_buckets[j][i]+=shared_buckets[j][i-1];
	}

    //从低位到高位收集
    for (j=0; j<key_size; j++) {
		digit = p(j);

		for(i=input_array_size-1;i>=0;i--) {
			int unit = input_array[i]/digit%DIGITS_AMOUNT;
			int pos = --shared_buckets[j][unit];
			tmp[pos]=input_array[i];
		}
		
		for(i=0;i<input_array_size;i++){
			input_array[i]=tmp[i];
        }
		
	}

    free(tmp);
}



int main(){
    
    sort_gen(array1,DATA_LENGTH,0.3);
    sort_gen(array2,DATA_LENGTH,0.3);
    sort_gen(array3,DATA_LENGTH,0.3);
	
    TIME_CALCULATION(std::sort(array1,array1+DATA_LENGTH))
    TIME_CALCULATION(radixsort_serial(array2,DATA_LENGTH,KEY_AMOUNT))
    // TIME_CALCULATION(radixsort_parallel(array3,DATA_LENGTH,KEY_AMOUNT,1))
	
    
    // for (int i = 0; i < DATA_LENGTH; i+=DATA_LENGTH/10)
    // {
    //     std::cout<<array1[i]<<std::endl;
    //     std::cout<<array2[i]<<std::endl;
    //     std::cout<<array3[i]<<std::endl;
		
    // }
    

}
