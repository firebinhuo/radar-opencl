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
//		�ֱ�	������ǰ�����ļƻ�
//		������	commQueueFFT �е������������0 ��һ����Чֵ������������¿ͻ��˲�ϣ������ʱ���и���ʵ���ֻԤ�ȼ���״̬��Ϣ
//		����FFT	�ͻ��˴����� cl_command_queue ���飻������б����Ǽƻ��������а������豸���ʵ��Ӽ�
//		pfn_notify	ָ��֪ͨ���̵ĺ���ָ�롣֪ͨ������һ���ص�������Ӧ�ó������ע�Ტ�ڳ����ִ���ļ��������ɹ��򲻳ɹ���ʱ���á�Ŀǰ���˲�������Ϊ NULL �� nullptr��
//		�û�����	���� pfn_notify ʱ��Ϊ�������ݡ�Ŀǰ���˲�������Ϊ NULL �� nullptr��
//	*/
#define pi 3.1416

//������

int readDataFromFile(const char *path,float *arr) {
	FILE *fp;
	int ch, i = 0, n = 0;
	char str[20];
	const char *filename = path;
	if ((fp = fopen(filename, "rt")) == NULL) {
		printf("���ܴ��ļ�:%s\n", filename);
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
	printf("��Ч���ݸ���:%d\n", n);
	fclose(fp);
}

int defArgs() {
	/*
		C=3.0e8;  %����(m/s)
		RF=3.140e9/2;  %�״���Ƶ
		Lambda=C/RF;%�״﹤������
		PulseNumber=16;   %�ز������� 
		BandWidth=2.0e6;  %�����źŴ���
		TimeWidth=42.0e-6; %�����ź�ʱ��(������)
		%PRT=4.096e-3;   % �״﷢�������ظ�����(s),240us��Ӧ1/2*240*300=36000��
		PRT=240e-6;   % �״﷢�������ظ�����(s),240us��Ӧ1/2*240*300=36000��
		PRF=1/PRT;
		Fs=2.0e6;  %����Ƶ��
		NoisePower=-12;%(dB);%�������ʣ�Ŀ��Ϊ0dB��
		% ---------------------------------------------------------------%
		SampleNumber=fix(Fs*PRT);%����һ���������ڵĲ�������480��
		TotalNumber=SampleNumber*PulseNumber;%�ܵĲ�������480*16=7680��
		BlindNumber=fix(Fs*TimeWidth);%����һ���������ڵ�ä��-�ڵ���������
	*/
	float C = 3.0e8f;
	float Fs = 2.0e6;  //����Ƶ��
	float BandWidth = 2.0e6; // %�����źŴ���
	float TimeWidth = 42.0e-6;// %�����ź�ʱ��(������)
	float number = floor(Fs*TimeWidth); //%�ز��Ĳ������� = ��ѹϵ������ = ��̬����Ŀ + 1
	if ((int)number % 2 ==1.0)
	{
		number++;
	}

	return number;
	/*
	for i=-fix(number/2):fix(number/2)-1
		 Chirp(i+fix(number/2)+1)=exp(j*(pi*(BandWidth/TimeWidth)*(i/Fs)^2));%exp(j*fi)* chirp�ź�
	end
	coeff=conj(fliplr(Chirp));%������ѹϵ�� fliplr˳��ת��conj ����Z��Ԫ�صĸ����
	*/
	_Dcomplex result1 = {0,-1};
	
	_Dcomplex z = cexp(result1); // Euler's formula

}

int main(void)
{
	const size_t N = 8192;
	float Fs = 2.0e6;  //����Ƶ��
	float BandWidth = 2.0e6; // %�����źŴ���
	float TimeWidth = 42.0e-6;// %�����ź�ʱ��(������)
	const int number = floor(Fs*TimeWidth); //%�ز��Ĳ������� = ��ѹϵ������ = ��̬����Ŀ + 1
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


	//����	
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

	//��ˮ��

	cl_mem bufSignal = clCreateBuffer(ctx, CL_MEM_READ_WRITE, N * 2 * sizeof(float), NULL, &err);
	err = clEnqueueWriteBuffer(queue, bufSignal, CL_TRUE, 0,
		N * 2 * sizeof(float), arr, 0, NULL, NULL);

	cl_mem bufChirp = clCreateBuffer(ctx, CL_MEM_READ_WRITE, N * 2 * sizeof(float), NULL, &err);
	err = clEnqueueWriteBuffer(queue, bufChirp, CL_TRUE, 0, N * 2 * sizeof(float), chirp, 0, NULL, NULL);
	/* Create a default plan for a complex FFT. */
	err = clfftCreateDefaultPlan(&planHandle, ctx, dim, clLengths);

	/* Set plan parameters. */
	err = clfftSetPlanPrecision(planHandle, CLFFT_SINGLE);//���� FFT ���ݵĸ��㾫�ȡ�
	err = clfftSetLayout(planHandle, CLFFT_COMPLEX_INTERLEAVED, CLFFT_COMPLEX_INTERLEAVED);//��������������������Ԥ�ڲ���
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
