#pragma warning( disable : 4996 )


#include <CL/cl.h>
#include <tchar.h>
#include <memory.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <complex.h>
#include <stdlib.h>

/* No need to explicitely include the OpenCL headers */
#include <clFFT.h>
#include <Windows.h>
//#define pi 3.1416

//读数据
int readDataFromFile(const char *path,float *arr) {
	FILE *fp;
	int ch, i = 0, n = 0;
	char str[20];
	const char *filename = path;
	if ((fp = fopen(filename, "rt")) == NULL) {
		printf("不能打开文件:%s\n", filename);
		return 1;
	}
	while ((ch = fgetc(fp)) != EOF) {
		if (ch == '\t' || ch == '\n') {
			if (i > 0) {
				str[i++] = '\0';
				//printf("%g\n", atof(str));
				arr[n] = atof(str);
				i = 0;
				n++;
			}
		}
		else str[i++] = ch;
	}
	printf("有效数据个数:%d\n", n);
	fclose(fp);
}

int defArgs() {
	
	float C = 3.0e8f;
	float Fs = 2.0e6;  //采样频率
	float BandWidth = 2.0e6; // %发射信号带宽
	float TimeWidth = 42.0e-6;// %发射信号时宽(脉冲宽度)
	float number = floor(Fs*TimeWidth); //%回波的采样点数 = 脉压系数长度 = 暂态点数目 + 1
	if ((int)number % 2 ==1.0)
	{
		number++;
	}

	return number;
	/*
	for i=-fix(number/2):fix(number/2)-1
		 Chirp(i+fix(number/2)+1)=exp(j*(pi*(BandWidth/TimeWidth)*(i/Fs)^2));%exp(j*fi)* chirp信号
	end
	coeff=conj(fliplr(Chirp));%产生脉压系数 fliplr顺序反转，conj 返回Z的元素的复共轭。
	*/
	_Dcomplex result1 = {0,-1};
	
	_Dcomplex z = cexp(result1); // Euler's formula

}
void LogInfo(const char* str, ...)
{
	if (str)
	{
		va_list args;
		va_start(args, str);

		vfprintf(stdout, str, args);

		va_end(args);
	}
}
int main(void)
{
	
	LARGE_INTEGER perfFrequency;
	LARGE_INTEGER performanceCountNDRangeStart;
	LARGE_INTEGER performanceCountNDRangeStop;

	const size_t N = 8192;
	float Fs = 2.0e6;  //采样频率
	float BandWidth = 2.0e6; // %发射信号带宽
	float TimeWidth = 42.0e-6;// %发射信号时宽(脉冲宽度)
	const int number = floor(Fs*TimeWidth); //%回波的采样点数 = 脉压系数长度 = 暂态点数目 + 1
	int num = number;
	if ((int)number % 2 == 1.0)
	{
		num++;
	}
	
	
	float arr[N * 2] = {0};
	const char* path = "echo.dat";
	readDataFromFile(path, arr);
	
	float chirp[N * 2] = {0};
	path = "coeff.dat";
	readDataFromFile(path, chirp);


	//测试	
	cl_int err;
	cl_platform_id platform = 0;
	cl_device_id device = 0;
	cl_context_properties props[3] = { CL_CONTEXT_PLATFORM, 0, 0 };
	cl_context ctx = 0;
	cl_command_queue queue = 0;

	cl_event event = NULL;
	int ret = 0;
	char platform_name[128];
	char device_name[128];

	/* FFT library realted declarations */
	clfftPlanHandle planHandle;
	clfftDim dim = CLFFT_1D;
	size_t clLengths[1] = { N };

	/* Setup OpenCL environment. */
	err = clGetPlatformIDs(1, &platform, NULL);

	size_t ret_param_size = 0;
	err = clGetPlatformInfo(platform, CL_PLATFORM_NAME,
		sizeof(platform_name), platform_name,
		&ret_param_size);
	printf("Platform found: %s\n", platform_name);

	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &device, NULL);

	err = clGetDeviceInfo(device, CL_DEVICE_NAME,
		sizeof(device_name), device_name,
		&ret_param_size);
	printf("Device found on the above platform: %s\n", device_name);

	props[1] = (cl_context_properties)platform;
	ctx = clCreateContext(props, 1, &device, NULL, NULL, &err);
	queue = clCreateCommandQueue(ctx, device, 0, &err);

	/* Setup clFFT. */
	clfftSetupData fftSetup;
	err = clfftInitSetupData(&fftSetup);
	err = clfftSetup(&fftSetup);

	bool queueProfilingEnable = true;
	if (queueProfilingEnable)
		QueryPerformanceCounter(&performanceCountNDRangeStart);
	


	//流水线
	cl_mem bufSignal = clCreateBuffer(ctx, CL_MEM_READ_WRITE, N * 2 * sizeof(float), NULL, &err);
	err = clEnqueueWriteBuffer(queue, bufSignal, CL_TRUE, 0,
		N * 2 * sizeof(float), arr, 0, NULL, NULL);

	/* Create a default plan for a complex FFT. */
	err = clfftCreateDefaultPlan(&planHandle, ctx, dim, clLengths);

	/* Set plan parameters. */
	err = clfftSetPlanPrecision(planHandle, CLFFT_SINGLE);//设置 FFT 数据的浮点精度。
	err = clfftSetLayout(planHandle, CLFFT_COMPLEX_INTERLEAVED, CLFFT_COMPLEX_INTERLEAVED);//设置输入和输出缓冲区的预期布局
	err = clfftSetResultLocation(planHandle, CLFFT_INPLACE);
	/* Execute the plan. */
	err = clfftEnqueueTransform(planHandle, CLFFT_FORWARD, 1, &queue, 0, NULL, NULL, &bufSignal, NULL, NULL);

	err = clEnqueueReadBuffer(queue, bufSignal, CL_TRUE, 0, N * 2 * sizeof(float), arr, 0, NULL, NULL);

	cl_mem bufChirp = clCreateBuffer(ctx, CL_MEM_READ_WRITE, N * 2 * sizeof(float), NULL, &err);
	err = clEnqueueWriteBuffer(queue, bufChirp, CL_TRUE, 0, N * 2 * sizeof(float), chirp, 0, NULL, NULL);
	

	/* Bake the plan. */
	err = clfftBakePlan(planHandle, 1, &queue, NULL, NULL);

	/* Execute the plan. */
	//err = clfftEnqueueTransform(planHandle, CLFFT_FORWARD, 1, &queue, 0, NULL, NULL, &bufSignal, NULL, NULL);
	err = clfftEnqueueTransform(planHandle, CLFFT_FORWARD, 1, &queue, 0, NULL, NULL, &bufChirp, NULL, NULL);
	//err = clfftEnqueueTransform(planHandle, CLFFT_BACKWARD, 1, &queue, 0, NULL, NULL, &bufX, NULL, NULL);
		/* Fetch results of calculations. */
	
	/* Wait for calculations to be finished. */
	err = clFinish(queue);

	/* Fetch results of calculations. */
	//err = clEnqueueReadBuffer(queue, bufSignal, CL_TRUE, 0, N * 2 * sizeof(float), arr, 0, NULL, NULL);
	err = clEnqueueReadBuffer(queue, bufChirp, CL_TRUE, 0, N * 2 * sizeof(float), chirp, 0, NULL, NULL);
	// Execute (enqueue) the kernel
	if (CL_SUCCESS != err)
	{
		return -1;
	}
	if (queueProfilingEnable)
		QueryPerformanceCounter(&performanceCountNDRangeStop);
	// retrieve performance counter frequency
	if (queueProfilingEnable)
	{
		QueryPerformanceFrequency(&perfFrequency);
		LogInfo("三级流水线执行时间 %f ms.\n",
			1000.0f*(float)(performanceCountNDRangeStop.QuadPart - performanceCountNDRangeStart.QuadPart) / (float)perfFrequency.QuadPart);
	}
	/* print output array */
	//print_iter = 0;
	//while (print_iter < N) {
	//	printf("(%f, %f) ", X[2 * print_iter], X[2 * print_iter + 1]);
	//	print_iter++;
	//}
	//printf("\n");

	/* Release OpenCL memory objects. */
	//clReleaseMemObject(bufX);

	//free(X);

	/* Release the plan. */
	err = clfftDestroyPlan(&planHandle);

	/* Release clFFT library. */
	clfftTeardown();

	/* Release OpenCL working objects. */
	clReleaseCommandQueue(queue);
	clReleaseContext(ctx);

	return ret;
}

// Upload the OpenCL C source code to output argument source
// The memory resource is implicitly allocated in the function
// and should be deallocated by the caller

#pragma warning( pop )
