#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>
#include <iostream>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <malloc.h>
#include <omp.h>

using namespace std;

#define MATRIX_N 1024

#define TIME_LINUX_CAL(CODE_TO_CAL) \
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

float rand_float(float s){
	return 4*s*(1-s);
}

void matrix_gen(float *a,float *b,int N,float seed){
	float s=seed;
	for(int i=0;i<N*N;i++){
		s=rand_float(s);
		a[i]=s;
		s=rand_float(s);
		b[i]=s;
	}
}

void matrix_print(float *a, int n) {
    // for (int i = 0; i < n; i++) {
    //     for (int j = 0; j < n; j++) {
    //         cout<<a[i * n + j]<<" ";
    //     }
    //     cout<<endl;
    // }
    for (int j = 0; j < n; j++) {
        cout<<a[(n-1)*n+j]<<" ";
    }
}

void matrix_multiply(float *a, float *b, float *c, int N) {
    int i, j, k;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            float sum = 0.0;
            for (k = 0; k < N; k++) {
                sum += a[i*N+k] * b[k*N+j];
            }
            c[i * N + j] = sum;
        }
    }
}

void matrix_multiply_blocks(float *a, float *b, float *c, int N, int blockSize) {
    auto calBlock = [blockSize, N](float *a, float *b, float *c) {
        for (int i = 0; i < blockSize; i++) {
            for (int j = 0; j < blockSize; j++) {
                for (int k = 0; k < blockSize; k++) {
                    c[i * N + j] += a[i * N + k] * b[k * N + j];
                }
            }
        }
    };

    for (int i = 0; i < N; i += blockSize) {
        for (int j = 0; j < N; j += blockSize) {
            for (int k = 0; k < N; k += blockSize) {
                calBlock(a+i*N+k, b+k*N+j, c+i*N+j);
            }
        }
    }
}


//avx计算 8×8 矩阵
void MATRIX_AVX8(float *a, float *b, float *c, int N) {
    __m256 vc0, vc1, vc2, vc3, vc4, vc5, vc6, vc7, vb;
    vc0 = _mm256_loadu_ps(&(c[0]));
    vc1 = _mm256_loadu_ps(&(c[N]));
    vc2 = _mm256_loadu_ps(&(c[2*N]));
    vc3 = _mm256_loadu_ps(&(c[3*N]));
    vc4 = _mm256_loadu_ps(&(c[4*N]));
    vc5 = _mm256_loadu_ps(&(c[5*N]));
    vc6 = _mm256_loadu_ps(&(c[6*N]));
    vc7 = _mm256_loadu_ps(&(c[7*N]));
    for (int j = 0; j < 8; j++) {
        vb = _mm256_loadu_ps(&(b[j * N]));
        vc0 = _mm256_fmadd_ps(_mm256_broadcast_ss(&(a[j])), vb, vc0);
        vc1 = _mm256_fmadd_ps(_mm256_broadcast_ss(&(a[j + N])), vb, vc1);
        vc2 = _mm256_fmadd_ps(_mm256_broadcast_ss(&(a[j + 2*N])), vb, vc2);
        vc3 = _mm256_fmadd_ps(_mm256_broadcast_ss(&(a[j + 3*N])), vb, vc3);
        vc4 = _mm256_fmadd_ps(_mm256_broadcast_ss(&(a[j + 4*N])), vb, vc4);
        vc5 = _mm256_fmadd_ps(_mm256_broadcast_ss(&(a[j + 5*N])), vb, vc5);
        vc6 = _mm256_fmadd_ps(_mm256_broadcast_ss(&(a[j + 6*N])), vb, vc6);
        vc7 = _mm256_fmadd_ps(_mm256_broadcast_ss(&(a[j + 7*N])), vb, vc7);
    }
    _mm256_storeu_ps(&(c[0]), vc0);
    _mm256_storeu_ps(&(c[N]), vc1);
    _mm256_storeu_ps(&(c[2*N]), vc2);
    _mm256_storeu_ps(&(c[3*N]), vc3);
    _mm256_storeu_ps(&(c[4*N]), vc4);
    _mm256_storeu_ps(&(c[5*N]), vc5);
    _mm256_storeu_ps(&(c[6*N]), vc6);
    _mm256_storeu_ps(&(c[7*N]), vc7);
}

void matrix_multiply_avx8(float *a, float *b, float *c, int N) {
    for (int i = 0; i < N; i += 8) {
        for (int j = 0; j < N; j += 8) {
            for (int k = 0; k < N; k += 8) {
                MATRIX_AVX8((a+i*N+k), (b+k*N+j), (c+i*N+j), (N));
            }
        }
    }
}

//openmp
void MATRIX_AVX8_OMP(float *a, float *b, float *c, int N) {
    __m256 vc0, vc1, vc2, vc3, vc4, vc5, vc6, vc7, vb;
    vc0 = _mm256_set1_ps(0.0);
    vc1 = _mm256_set1_ps(0.0);
    vc2 = _mm256_set1_ps(0.0);
    vc3 = _mm256_set1_ps(0.0);
    vc4 = _mm256_set1_ps(0.0);
    vc5 = _mm256_set1_ps(0.0);
    vc6 = _mm256_set1_ps(0.0);
    vc7 = _mm256_set1_ps(0.0);

    #pragma omp parallel for reduction(+:vc0,vc1,vc2,vc3,vc4,vc5,vc6,vc7)
    for (int j = 0; j < 8; j++) {
        vb = _mm256_loadu_ps(&(b[j * N]));
        vc0 = _mm256_fmadd_ps(_mm256_broadcast_ss(&(a[j])), vb, vc0);
        vc1 = _mm256_fmadd_ps(_mm256_broadcast_ss(&(a[j + N])), vb, vc1);
        vc2 = _mm256_fmadd_ps(_mm256_broadcast_ss(&(a[j + 2*N])), vb, vc2);
        vc3 = _mm256_fmadd_ps(_mm256_broadcast_ss(&(a[j + 3*N])), vb, vc3);
        vc4 = _mm256_fmadd_ps(_mm256_broadcast_ss(&(a[j + 4*N])), vb, vc4);
        vc5 = _mm256_fmadd_ps(_mm256_broadcast_ss(&(a[j + 5*N])), vb, vc5);
        vc6 = _mm256_fmadd_ps(_mm256_broadcast_ss(&(a[j + 6*N])), vb, vc6);
        vc7 = _mm256_fmadd_ps(_mm256_broadcast_ss(&(a[j + 7*N])), vb, vc7);
    }

    vc0 = _mm256_add_ps(vc0, _mm256_loadu_ps(&(c[0])));
    vc1 = _mm256_add_ps(vc1, _mm256_loadu_ps(&(c[N])));
    vc2 = _mm256_add_ps(vc2, _mm256_loadu_ps(&(c[2*N])));
    vc3 = _mm256_add_ps(vc3, _mm256_loadu_ps(&(c[3*N])));
    vc4 = _mm256_add_ps(vc4, _mm256_loadu_ps(&(c[4*N])));
    vc5 = _mm256_add_ps(vc5, _mm256_loadu_ps(&(c[5*N])));
    vc6 = _mm256_add_ps(vc6, _mm256_loadu_ps(&(c[6*N])));
    vc7 = _mm256_add_ps(vc7, _mm256_loadu_ps(&(c[7*N])));

    _mm256_storeu_ps(&(c[0]), vc0);
    _mm256_storeu_ps(&(c[N]), vc1);
    _mm256_storeu_ps(&(c[2*N]), vc2);
    _mm256_storeu_ps(&(c[3*N]), vc3);
    _mm256_storeu_ps(&(c[4*N]), vc4);
    _mm256_storeu_ps(&(c[5*N]), vc5);
    _mm256_storeu_ps(&(c[6*N]), vc6);
    _mm256_storeu_ps(&(c[7*N]), vc7);
}

void matrix_multiply_avx8_omp(float *a, float *b, float *c, int N) {
    omp_set_num_threads(4);
    #pragma omp parallel for
    for (int i = 0; i < N; i += 8) {
        for (int j = 0; j < N; j += 8) {
            for (int k = 0; k < N; k += 8) {
                MATRIX_AVX8((a+i*N+k), (b+k*N+j), (c+i*N+j), (N));
            }
        }
    }
}

void matrix_multiply_avx8_omp2(float *a, float *b, float *c, int N) {
    omp_set_num_threads(16);
    #pragma omp parallel for
    for (int i = 0; i < N; i += 8) {
        for (int j = 0; j < N; j += 8) {
            for (int k = 0; k < N; k += 8) {
                MATRIX_AVX8_OMP((a+i*N+k), (b+k*N+j), (c+i*N+j), (N));
            }
        }
    }
}

int main() {
    int all = MATRIX_N * MATRIX_N;
    float *a, *b, *c;
    struct timeval start;
    struct timeval end;
    unsigned long diff;
    a = new float[MATRIX_N * MATRIX_N];
    b = new float[MATRIX_N * MATRIX_N];
    c = new float[MATRIX_N * MATRIX_N];
    matrix_gen(a, b, MATRIX_N, 0.3);
    
    TIME_LINUX_CAL(matrix_multiply(a, b, c, MATRIX_N));

    float *res1 = new float[all];
    TIME_LINUX_CAL(matrix_multiply_blocks(a, b, res1, MATRIX_N, 8));
    // matrix_print(res1, MATRIX_N);
    
    float *res2 = new float[all];
    TIME_LINUX_CAL(matrix_multiply_avx8(a, b, res2, MATRIX_N));

    float *res3 = new float[all];
    TIME_LINUX_CAL(matrix_multiply_avx8_omp(a, b, res3, MATRIX_N));
    // matrix_print(res3, MATRIX_N);

    float *res4 = new float[all];
    TIME_LINUX_CAL(matrix_multiply_avx8_omp2(a, b, res4, MATRIX_N)); 
    // matrix_print(res4, MATRIX_N);
}