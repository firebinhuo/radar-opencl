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
//	/*
//		手柄	处理先前创建的计划
//		队列数	commQueueFFT 中的命令队列数；0 是一个有效值，在这种情况下客户端不希望运行时运行负载实验而只预先计算状态信息
//		队列FFT	客户端创建的 cl_command_queue 数组；命令队列必须是计划上下文中包含的设备的适当子集
//		pfn_notify	指向通知例程的函数指针。通知例程是一个回调函数，应用程序可以注册并在程序可执行文件构建（成功或不成功）时调用。目前，此参数必须为 NULL 或 nullptr。
//		用户数据	调用 pfn_notify 时作为参数传递。目前，此参数必须为 NULL 或 nullptr。
//	*/
#define pi 3.1416

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
	/*
		C=3.0e8;  %光速(m/s)
		RF=3.140e9/2;  %雷达射频
		Lambda=C/RF;%雷达工作波长
		PulseNumber=16;   %回波脉冲数 
		BandWidth=2.0e6;  %发射信号带宽
		TimeWidth=42.0e-6; %发射信号时宽(脉冲宽度)
		%PRT=4.096e-3;   % 雷达发射脉冲重复周期(s),240us对应1/2*240*300=36000米
		PRT=240e-6;   % 雷达发射脉冲重复周期(s),240us对应1/2*240*300=36000米
		PRF=1/PRT;
		Fs=2.0e6;  %采样频率
		NoisePower=-12;%(dB);%噪声功率（目标为0dB）
		% ---------------------------------------------------------------%
		SampleNumber=fix(Fs*PRT);%计算一个脉冲周期的采样点数480；
		TotalNumber=SampleNumber*PulseNumber;%总的采样点数480*16=7680；
		BlindNumber=fix(Fs*TimeWidth);%计算一个脉冲周期的盲区-遮挡样点数；
	*/
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

int main(void)
{
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

	/* Allocate host & initialize data. */
	/* Only allocation shown for simplicity. */
	//X = (float *)malloc(N * 2 * sizeof(*X));

	/* print input array */
	/*printf("\nPerforming fft on an one dimensional array of size N = %lu\n", (unsigned long)N);
	int print_iter = 0;
	while (print_iter < N) {
		float x = (float)print_iter;
		float y = (float)print_iter * 3;
		X[2 * print_iter] = x;
		X[2 * print_iter + 1] = y;
		printf("(%0.2f, %0.2f) ", x, y);
		print_iter++;
	}
	printf("\n\nfft result: \n");*/

	//流水线

	cl_mem bufSignal = clCreateBuffer(ctx, CL_MEM_READ_WRITE, N * 2 * sizeof(float), NULL, &err);
	err = clEnqueueWriteBuffer(queue, bufSignal, CL_TRUE, 0,
		N * 2 * sizeof(float), arr, 0, NULL, NULL);

	cl_mem bufChirp = clCreateBuffer(ctx, CL_MEM_READ_WRITE, N * 2 * sizeof(float), NULL, &err);
	err = clEnqueueWriteBuffer(queue, bufChirp, CL_TRUE, 0, N * 2 * sizeof(float), chirp, 0, NULL, NULL);
	/* Create a default plan for a complex FFT. */
	err = clfftCreateDefaultPlan(&planHandle, ctx, dim, clLengths);

	/* Set plan parameters. */
	err = clfftSetPlanPrecision(planHandle, CLFFT_SINGLE);//设置 FFT 数据的浮点精度。
	err = clfftSetLayout(planHandle, CLFFT_COMPLEX_INTERLEAVED, CLFFT_COMPLEX_INTERLEAVED);//设置输入和输出缓冲区的预期布局
	err = clfftSetResultLocation(planHandle, CLFFT_INPLACE);
	const char* postcallbackstr = "void post(__global void *output, uint outoffset, __global void *userdata, float2 fftoutput, __local void *localmem )\
	{\
		*((__global float2*)output + outoffset) = fftoutput;\
	}";
	/*int h_postuserdata[N * 2] = {  };
	cl_mem postuserdata = clCreateBuffer(ctx, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N * 2, (void*)h_postuserdata, NULL);*/
	//clfftSetPlanCallback(planHandle,"post", postcallbackstr, 0, POSTCALLBACK, NULL, 0);
	/* Bake the plan. */
	err = clfftBakePlan(planHandle, 1, &queue, NULL, NULL);

	/* Execute the plan. */
	err = clfftEnqueueTransform(planHandle, CLFFT_FORWARD, 1, &queue, 0, NULL, NULL, &bufSignal, NULL, NULL);
	//err = clfftEnqueueTransform(planHandle, CLFFT_FORWARD, 1, &queue, 0, NULL, NULL, &bufChirp, NULL, NULL);
	//err = clfftEnqueueTransform(planHandle, CLFFT_BACKWARD, 1, &queue, 0, NULL, NULL, &bufX, NULL, NULL);
	
	/* Wait for calculations to be finished. */
	err = clFinish(queue);

	/* Fetch results of calculations. */
	err = clEnqueueReadBuffer(queue, bufSignal, CL_TRUE, 0, N * 2 * sizeof(float), arr, 0, NULL, NULL);
	err = clEnqueueReadBuffer(queue, bufChirp, CL_TRUE, 0, N * 2 * sizeof(float), chirp, 0, NULL, NULL);

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
