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