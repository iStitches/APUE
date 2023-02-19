void radixsort_serial_cache(int *input_array, int input_array_size,int key_size){
	int digit=1;
	int i,j,k;

	int *tmp = (int *)calloc(input_array_size, sizeof(int));//临时数组
	int buckets[DIGITS_AMOUNT];   //10个计数桶,每个桶开辟数据大小的空间
	
	int buf_length = 8096/DIGITS_AMOUNT;
	int buf_buckets[DIGITS_AMOUNT][buf_length];       //缓冲桶组 确保这个组能被全部一次性写入cache
	//初始化桶
       memset(buckets, 0, DIGITS_AMOUNT*sizeof(int));
	   memset(buf_buckets, 0, DIGITS_AMOUNT*buf_length*sizeof(int));

	for ( i = 0; i < key_size; i++) //数据的每一位进行一次循环 从低位开始
	{
		//i=0 digit=1;i=1 digit=10
		for( j = 0; j < input_array_size; j++){
			k=input_array[j]it%DIGITS_AMOUNT;  //k是一个数据在第i位的值
			//计数加
			buckets[k]++;
		}
		
		//桶计算前缀和
		for (j=1; j<DIGITS_AMOUNT; j++){
		 	buckets[j]=buckets[j-1]+buckets[j];
		}
		//收集
		for(j=input_array_size-1;j>=0;j--) {
			k = input_array[j]it%DIGITS_AMOUNT;
			//插入缓存
			buf_buckets[k][0]++;
			buf_buckets[k][buf_buckets[k][0]] = input_array[j];
			if (buf_buckets[k][0]==buf_length-1)
			{
				//缓冲区全部冲入
				for (int p = 1; p <= buf_length-1; p++)
				{
					int pos = --buckets[k];
					tmp[pos]=buf_buckets[k][p];
				}
				buf_buckets[k][0]=0;
			}
		}
		//剩余缓冲区冲入
		for(k = 0;k<DIGITS_AMOUNT;k++){
			if(buf_buckets[k][0]!=0){
				for (int j = 1; j <= buf_buckets[k][0]; j++)
				{
					int pos = --buckets[k];
					tmp[pos]=buf_buckets[k][j];
					
				}
				buf_buckets[k][0]=0;
			}
		}
		for(int f=0;f<input_array_size;f++){
			input_array[f]=tmp[f];
        }
		digit=digit*10;
		memset(buckets, 0, DIGITS_AMOUNT*sizeof(int));
	}

	free(tmp);
}