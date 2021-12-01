// TODO: Add OpenCL kernel code here.
__kernel void convolution(__global float* imageIn,__global float* imageOut,__constant float* filter,
int rows,int cols,int filterWidth, __local float* localImage ,int localHeight,int localWidth)
{ 
	int filterRadius = (filterWidth / 2);
    int padding = (filterRadius / 2) * 2;
    //决定workgroup何处开始读
  
	int groupIdCol = get_group_id(0);
	int groupIdRow = get_group_id(1);
	int groupStartCol = groupIdCol * get_local_size(0);
	//printf("groupId-->(%d,%d)  ",groupIdRow , groupIdCol);
    int groupStartRow = groupIdRow * get_local_size(1);
	//printf("groupStart-->(%d,%d) ",groupStartRow ,groupStartCol);
	
    //localID
    int localCol = get_local_id(0);
    int localRow = get_local_id(1);
	
    //唯一ID   
    int globalCol = groupStartCol + localCol;
    int globalRow = groupStartRow + localRow;

	
    //printf("%d\t",localCol);
    // printf("%d\t",localHeight);
	//int gr = get_global_id(1);
	//int gc = get_global_id(0);
	//printf("%d ",gr)
	//imageOut[gr * cols + gc] = imageIn[gr * cols + gc];
	
    //将数据移动到 local memory
    for(int i = localRow; i < localHeight; i += get_local_size(1))
    {
        int curRow = groupStartRow + i;//当前组的具体一行{ } + { }
        for(int j = localCol; j < localWidth; j += get_local_size(0))
        {
			//printf("(%d,%d)",groupStartCol , j);
            int curCol = groupStartCol + j;//当前组的具体一列
			printf("current-->(%d,%d)\n ",curRow,curCol);
			
            //如果有限制，则执行读取操作
            if(curRow < rows && curCol <cols)//小于图像原始宽高
            {
                localImage[i * localWidth + j] = imageIn[curRow * cols + curCol];
				printf("%0.2f  ",localImage[i * localWidth + j]);		
            }
        }
		
    }
	printf("%s\n"," ");
	barrier(CLK_LOCAL_MEM_FENCE);

	//执行卷积
	
	if(globalRow < rows - padding && globalCol < cols - padding)
	{
		float sum = 0.0f;
		int filterIdx = 0;
		/*
		for(int i = localRow;i<localRow + filterWidth;i++)
		{
			int offset = i * localWidth;
			for(int j = localCol; j < localCol + filterWidth;j++)
			{
				sum += 	localImage[offset+j] * filter[filterIdx++];
				printf("(%d,%d,%0.2f)  ",localRow,localCol,sum);
			}
			//printf("%s\n",  "  ");
		}*/
		
		for(int i = localRow; i<localRow + filterWidth;i++){ 
			
			int offset = i * localWidth + localCol;
			//printf("localRow-->(%d,%d)\t  ", localRow,offset);
			printf("%0.2f ",localImage[offset++]);
			//printf("%0.2f ",localImage[offset++]);
			//printf("%0.2f ",localImage[offset++]);
			//sum += localocalImage[offset++]lImage[offset++] * filter[filterIdx++];
			//sum += localocalImage[offset++]lImage[offset++] * filter[filterIdx++];
			//sum += localImage[offset++] * filter[filterIdx++];
			//sum += localImage[offset++] * filter[filterIdx++];
			//sum += localImage[offset++] * filter[filterIdx++];
			//sum += localImage[offset++] * filter[filterIdx++];
			//sum += localImage[offset++] * filter[filterIdx++];
			//printf("%0.2f ", offset);
			//printf("(%d,%d,%0.2f)  ",localRow,localCol,sum);
			
		}
		printf("%d  ", globalRow );
		imageOut[(globalRow + filterRadius) * cols + (globalCol + filterRadius)] = sum;
	
	}return;
}