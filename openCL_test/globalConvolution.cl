// TODO: Add OpenCL kernel code here.
__kernel void convolution1(__global float* imageIn,__global float* imageOut,__constant float* filter,
int rows,int cols,int filterWidth, __local float* localImage ,int localHeight,int localWidth)
{
	int globalRow = get_global_id(1);
	int globalCol = get_global_id(0);
	if(globalRow < rows-2){ 
		if(globalCol < cols-2){ 
			float sum = 0;
			for(int fy = 0; fy < filterWidth;fy++){ 
				for(int fx = 0; fx < filterWidth; fx++){ 
					float filterItem = filter[fx + fy * filterWidth];
					float imageItem = imageIn[fx + globalCol + (globalRow + fy) * cols];
					/*float filterItem = filter[fx + fy * filterWidth];
					float imageItem = imageIn[x + fx + (fy + y) * cols)];
				*/
					sum += filterItem * imageItem;
					printf("%0.2f  ",sum);
				}
			}
			imageOut[globalCol+1 + ((globalRow+1) * cols)] = sum;
		}
	}

}