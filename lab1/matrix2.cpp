#include <iostream>
#include <time.h>

using namespace std;

void matricesMulNormal(int N, float* lhs, float* rhs, float* ret)
{
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
           for(int k = 0; k < N; ++k)
                ret[j + i * N] += lhs[k + i * N] * rhs[j + k * N]; 
}

void matricesMulBlock(int N, int BLOCKSIZE, float* lhs, float* rhs, float* ret)
{
    auto doBlock = [BLOCKSIZE, N](float* lhs, float* rhs, float* ret){
        for(int i = 0; i < BLOCKSIZE; i++)
            for(int j = 0; j < BLOCKSIZE; j++)
                for(int k = 0; k < BLOCKSIZE; k++ )
                    ret[i * N + j] += lhs[i * N + k] * rhs[k * N + j];
    };

    for (int j = 0; j < N; j += BLOCKSIZE)
        for (int i = 0; i < N; i += BLOCKSIZE)
            for (int k = 0; k < N; k += BLOCKSIZE)
                doBlock(lhs + i * N + k, rhs + k * N + j, ret + i * N + j);
}

float rand_float(float s)
{
    return 4 * s * (1-s);
}
void matrix_gen(float* a, float* b, int N, float seed)
{
    float s = seed;
    for (int i = 0; i < N * N; i++)
    {
        s = rand_float(s);
        a[i] = s;
        s = rand_float(s);
        b[i] = s;
    }
}

#define TIME_CALCULATION(CODE_TO_CAL) \
{    \
    clock_t start = clock();\
    CODE_TO_CAL; \
    clock_t end = clock();\
    cout << (double)(end - start)/CLOCKS_PER_SEC << endl;\
}

int main()
{
{    
    float* lhs = new float[512*512];
    float* rhs = new float[512*512]; 
    float* res = new float[512*512]; 
    matrix_gen(lhs, rhs, 512, 0.1);

    TIME_CALCULATION(matricesMulNormal(512, lhs, rhs, res))
    TIME_CALCULATION(matricesMulBlock(512, 16, lhs, rhs, res))
    TIME_CALCULATION(matricesMulBlock(512, 32, lhs, rhs, res))
    TIME_CALCULATION(matricesMulBlock(512, 64, lhs, rhs, res))   
    TIME_CALCULATION(matricesMulBlock(512, 128, lhs, rhs, res))   
}

{    
    float* lhs = new float[1024*1024];
    float* rhs = new float[1024*1024]; 
    float* res = new float[1024*1024]; 
    matrix_gen(lhs, rhs, 1024, 0.1);

    TIME_CALCULATION(matricesMulNormal(1024, lhs, rhs, res))
    TIME_CALCULATION(matricesMulBlock(1024, 16, lhs, rhs, res))
    TIME_CALCULATION(matricesMulBlock(1024, 32, lhs, rhs, res))   
    TIME_CALCULATION(matricesMulBlock(1024, 64, lhs, rhs, res))
    TIME_CALCULATION(matricesMulBlock(1024, 128, lhs, rhs, res))
}
{    
    float* lhs = new float[2048*2048];
    float* rhs = new float[2048*2048]; 
    float* res = new float[2048*2048]; 
    matrix_gen(lhs, rhs, 2048, 0.1);

    TIME_CALCULATION(matricesMulNormal(2048, lhs, rhs, res))
    TIME_CALCULATION(matricesMulBlock(2048, 16, lhs, rhs, res))
    TIME_CALCULATION(matricesMulBlock(2048, 32, lhs, rhs, res))
    TIME_CALCULATION(matricesMulBlock(2048, 64, lhs, rhs, res))   
    TIME_CALCULATION(matricesMulBlock(2048, 128, lhs, rhs, res))   
}
}
