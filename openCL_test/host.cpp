// Add you host code
#include <CL/cl.h>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "utils.h"
using namespace std;
using namespace cv;

#define WGX 4
#define WGY 4
#define NON_OPTIMIZED
#define READ_ALIGNED
#define READ4
#pragma warning( disable : 4996 )
float arr[650 * 564];

//����mul�ľ���value�����������
unsigned int roundUp(unsigned int value, unsigned int mul) {
	unsigned int remainder = value % mul;
	if (remainder != 0) {
		value += (mul - remainder);
	}
	return    value;
};

/*
	���ܣ���ȡһ���Ҷȵ�ͼ��
*/
float* readGrayImage(const char *path, int& imageWidth, int& imageHeight) {
	Mat img = imread(path);
	cvtColor(img, img, COLOR_BGR2GRAY);
	imageWidth = img.cols;
	imageHeight = img.rows;

	int W = img.cols * img.channels();
	int h = img.rows;

	for (int row = 0; row < h; row++)
	{
		uchar* inData = img.ptr<uchar>(row);            //ptrΪָ��ͼƬ����ָ�룬����iΪ����
		for (int col = 0; col < W; col++)
		{
			//cout << setw(3) << (float)img.at<uchar>(row, col) << " ";
			arr[row * W + col] = inData[col];
			//cout << (int)arr[i] << " ";
		}
		//cout << endl;
	}
	return	arr;
}

float* readImage(char *path, int& imageWidth, int& imageHeight) {
	Mat img = imread(path);
	//cvtColor(img, img, COLOR_BGR2GRAY);
	/*namedWindow("hehe", WINDOW_NORMAL);
	imshow("hehe", img);
	waitKey(2000);*/
	imageWidth = img.cols;
	imageHeight = img.rows;

	int W = img.cols * img.channels();
	int h = img.rows;

	for (int row = 0; row < h; row++)
	{
		uchar* inData = img.ptr<uchar>(row);            //ptrΪָ��ͼƬ����ָ�룬����iΪ����
		for (int col = 0; col < W; col++)
		{
			//cout << setw(3) << (float)img.at<uchar>(row, col) << " ";
			arr[row * W + col] = inData[col];
			//cout << (int)arr[i] << " ";
		}
		//cout << endl;
	}
	/*

	Mat imgSrc(h,(size_t)(W/3),CV_8UC3);
	for (size_t x = 0; x < h; x++)
	{
		uchar* outData = imgSrc.ptr<uchar>(x);
		for (size_t y = 0; y < W; y++)
		{
			outData[y] = arr[x*W+y];
		}
	}*/
	/*namedWindow("laji", WINDOW_NORMAL);
	imshow("laji", imgSrc);
	waitKey(5000);
	imwrite("save.jpg", imgSrc);*/
	return	arr;

}
void writeImage(char *path, int imageWidth, int imageHeight, float *arr) {
	Mat imgSrc(imageHeight, (size_t)(imageWidth), CV_8UC1);
	//Mat imgSrc(imageHeight, (size_t)(imageWidth / 3), CV_8UC3);
	//�������
	cout << endl;
	for (size_t x = 0; x < imageHeight; x++)
	{
		uchar* outData = imgSrc.ptr<uchar>(x);
		for (size_t y = 0; y < imageWidth; y++)
		{
			outData[y] = arr[x * imageWidth + y];
			cout << arr[x * imageWidth + y] << "\t";
		}
		cout << endl;

	}
	imwrite(path, imgSrc);
}


int t_main(int argc, char **argv) {
	//const char* path = "23.jpg";
	int imageWidth = 8;
	int imageHeight = 8;
	//float* image = readGrayImage(path, imageWidth, imageHeight);

	//float* image = readImage(path,imageWidth,imageHeight);
	//���Դ��룬��֤����ͼ���Ƿ��������
	/*for (size_t i = 0; i < imageHeight; i++){
		for (size_t j = 0; j < imageWidth; j++) {
			cout << image[i* imageWidth + j] <<    "\t";
		}
		cout << endl;
	}*/
	//imageWidth = imageWidth * 3;
	//writeImage("save.jpg", imageWidth, imageHeight,image);
	//imageWidth = imageWidth * 3;//ͨ����
	float image[64] = {
	1,		2,		3,		4,		5,		6,		7,		1,
	2,		3,		4,		5,		6,		7,		1,		2,
	3,		4,		5,		6,		7,		1,		2,		3,
	4,		5,		6,		7,		1,		2,		3,		4,
	5,		6,		7,		1,		2,		3,		4,		5,
	6,		7,		1,		2,		3,		4,		5,		6,
	7,		1,		2,		3,		4,		5,		6,		7,
	1,		2,		3,		4,		5,		6,		7,		1
	};
	int dataSize = imageHeight * imageWidth * sizeof(float);
	//�Ƿ�Ϊ�����
#ifdef NON_OPTIMIZED
	int deviceWidth = imageWidth;
#else//READ_ALIGNED || READ4
	int deviceWidth = roundUp(imageWidth, WGX);
#endif // NOT_OPTIMIZED
	int deviceHeight = imageHeight;

	int deviceDataSize = imageHeight * deviceWidth * sizeof(float);

	//���ͼ��
	float* outputImage = NULL;
	outputImage = (float*)malloc(dataSize);
	//��ʼ�����ͼ������

	for (size_t i = 0; i < imageHeight; i++)
	{
		for (size_t j = 0; j < imageWidth; j++) {
			outputImage[i * imageWidth + j] = 0;
		}
	}
	/*
		�������
	*/
	//float filter[49] = {
	//	0,		0,		0,		0,		0,		0.0145,	0,
	//	0,		0,		0,		0,		0.0376,	0.1283,	0.0145,
	//	0,		0,		0,		0.0376,	0.1283,	0.0376,	0,
	//	0,		0,		0.0376,	0.1283,	0.0376,	0,		0,
	//	0,		0.0376,	0.1283,	0.0376,	0,		0,		0,
	//	0.0145,	0.1283,	0.0376,	0,		0,		0,		0,
	//	0,		0.0145,	0,		0,		0,		0,		0
	//};
	float filter[9] = {
		0,	0,	1,
		0,	1,	0,
		1,	0,	0
	};
	int filterWidth = 3;
	float filterRadius = filterWidth / 2;
	int paddingPixels = (int)(filterRadius / 2) * 2;

	/*
		��ʼ��OpenCLƽ̨
		1.����ƽ̨
		2.�ҵ��豸
		3.����������
		4.��������
	*/
	cl_uint uiNumPlatforms = 0;
	cl_platform_id platformId;
	clGetPlatformIDs(0, NULL, &uiNumPlatforms);
	// ����ƽ̨��Ϊƽ̨�����ڴ�ռ�
	cl_platform_id *pPlatforms = (cl_platform_id *)malloc(uiNumPlatforms * sizeof(cl_platform_id));

	// ��ÿ��õ�ƽ̨
	clGetPlatformIDs(uiNumPlatforms, pPlatforms, NULL);
	platformId = pPlatforms[0];	// ��õ�һ��ƽ̨�ĵ�ַ
	free(pPlatforms);
	cl_device_id deviceId;
	clGetDeviceIDs(platformId, CL_DEVICE_TYPE_GPU, 1, &deviceId, NULL);

	cl_context_properties props[3] = { CL_CONTEXT_PLATFORM,(cl_context_properties)(platformId),0 };
	cl_context context;
	context = clCreateContext(props, 1, &deviceId, NULL, NULL, NULL);

	cl_command_queue queue;
	queue = clCreateCommandQueue(context, deviceId, 0, NULL);


	cl_uint work_item_dim;
	size_t work_item_sizes[3];
	size_t work_group_size;
	cl_uint ucomput_uint = 0;
	cl_uint uconstant_args = 0;
	cl_ulong uconstant_buffer_size = 0;

	//(void*)&   ��������ָ�����
	//(void*)& a  ��������ָ�����  �ǽ�a�ĵ�ַǿ��ת��Ϊ�������͵�ָ�����
	clGetDeviceInfo(deviceId, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &work_item_dim, NULL);
	//clGetDeviceInfo(pDevices[0], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), (void *)&work_item_dim, NULL);
	clGetDeviceInfo(deviceId, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(work_item_sizes), (void *)work_item_sizes, NULL);
	clGetDeviceInfo(deviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), (void *)&work_group_size, NULL);
	clGetDeviceInfo(deviceId, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), (void *)&ucomput_uint, NULL);
	clGetDeviceInfo(deviceId, CL_DEVICE_MAX_CONSTANT_ARGS, sizeof(cl_uint), (void *)&uconstant_args, NULL);
	clGetDeviceInfo(deviceId, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_ulong), (void *)&uconstant_buffer_size, NULL);
	//clGetDeviceInfo(deviceId, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(size_t), (void *)&ucomput_uint, NULL);
	printf("Max work-item dimensions   : %d\n", work_item_dim);
	printf("Max work-item sizes        : %d %d %d\n", work_item_sizes[0], work_item_sizes[1], work_item_sizes[2]);
	printf("Max work-group sizes       : %d\n", work_group_size);
	printf("Max comput_uint            : %u\n", ucomput_uint);
	printf("Max constant_args          : %u\n", uconstant_args);
	printf("Max constant_buffer_size   : %u\n", uconstant_buffer_size);

	//��������
	cl_mem d_inputImage;
	cl_mem d_outputImage;
	cl_mem d_filter;
	d_inputImage = clCreateBuffer(context, CL_MEM_READ_ONLY, deviceDataSize, NULL, NULL);
	d_outputImage = clCreateBuffer(context, CL_MEM_WRITE_ONLY, deviceDataSize, NULL, NULL);
	d_filter = clCreateBuffer(context, CL_MEM_READ_ONLY, filterWidth*filterWidth * sizeof(float), NULL, NULL);
#ifdef NON_OPTIMIZED
	clEnqueueWriteBuffer(queue, d_inputImage, CL_TRUE, 0, deviceDataSize, image, 0, NULL, NULL);
#else// READ_ALIGNED ||READ4
	size_t buffer_origin[3] = { 0,0,0 };
	size_t host_origin[3] = { 0,0,0 };
	size_t region[3] = { deviceWidth * sizeof(float),imageHeight,1 };
	clEnqueueWriteBufferRect(queue, d_inputImage, CL_TRUE, 0, filterWidth * filterWidth * sizeof(float),
		buffer_origin, host_origin, region,
		deviceWidth * sizeof(float), 0, imageWidth * sizeof(float), 0,
		image, 0, NULL, NULL);

#endif // NON_OPTIMIZED
	//�������д���豸��
	clEnqueueWriteBuffer(queue, d_filter, CL_TRUE, 0, filterWidth* filterWidth * sizeof(float), filter, 0, NULL, NULL);

	//�����ں��ļ�
	// ��cl�ļ��еĴ���תΪ�ַ���
	string strSource;
	const char *pSource = NULL;
	//ConvertToString("conv.cl", strSource);
	ConvertToString("globalConvolution.cl", strSource);

	pSource = strSource.c_str();			// ���strSourceָ��
	size_t strLength[1] = { strlen(pSource) };	// �ַ�����С

											// �����������
	cl_program program;
	program = clCreateProgramWithSource(context, 1, &pSource, strLength, NULL);

	cl_int build_status;
	build_status = clBuildProgram(program, 1, &deviceId, NULL, NULL, NULL);

	//�����ں�
	cl_kernel kernel;
#if defined NON_OPTIMIZED || defined READ_ALIGNED
	//
	kernel = clCreateKernel(program, "convolution1", NULL);
#else //READ4
	kernel = clCreateKernel(program, "convolution_read4", NULL);
#endif
	//ѡ��workgroup��Сλ 16* 16
	int wgWidth = WGX;
	int wgHeight = WGY;
	//
	int totalWorkItemX = roundUp(imageWidth - paddingPixels, wgWidth);
	int totalWorkItemY = roundUp(imageHeight - paddingPixels, wgHeight);
	//workgroup ��С
	size_t localsize[2] = { wgWidth,wgHeight };
	// NDRange
	size_t globalSize[2] = { totalWorkItemX,totalWorkItemY };
	size_t offset[2] = { 0,0 };
# if defined NON_OPTIMIZED || defined READ_ALIGNED
	int localWidth = localsize[0] + paddingPixels;
#else //READ4
	int localWidth = roundUp(localsize[0] + paddingPixels, 4);
#endif
	int localHeight = localsize[1] + paddingPixels;
	size_t localMemsize = (localWidth * localHeight * sizeof(float));
	/*
		�����ں˲���
	*/
	clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_inputImage);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_outputImage);
	clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_filter);
	clSetKernelArg(kernel, 3, sizeof(int), &deviceWidth);
	clSetKernelArg(kernel, 4, sizeof(int), &deviceHeight);
	clSetKernelArg(kernel, 5, sizeof(int), &filterWidth);
	clSetKernelArg(kernel, 6, localMemsize, NULL);
	clSetKernelArg(kernel, 7, sizeof(int), &localHeight);
	clSetKernelArg(kernel, 8, sizeof(int), &localWidth);

	/*
		ִ���ں�
	*/
	clEnqueueNDRangeKernel(queue, kernel, 2, offset, globalSize, localsize, 0, NULL, NULL);
	//�ȴ��ں�ִ�н���
	clFinish(queue);
#ifdef NON_OPTIMIZED 
	clEnqueueReadBuffer(queue, d_outputImage, CL_TRUE, 0, deviceDataSize, outputImage, 0, NULL, NULL);
#else  //READ_ALIGNED || READ4
	buffer_origin[0] = 3 * sizeof(float);
	buffer_origin[1] = 3;
	buffer_origin[2] = 0;

	host_origin[0] = 3 * sizeof(float);
	host_origin[1] = 3;
	host_origin[2] = 0;

	region[0] = (imageWidth - paddingPixels) * sizeof(float);
	region[1] = (imageHeight - paddingPixels);
	region[2] = 1;

	//ִ�ж�����
	clEnqueueReadBufferRect(queue, d_outputImage, CL_TRUE, buffer_origin��host_origin, region,
		deviceWidth * sizeof(float), 0, imageWidth * sizeof(float), 0,
		outputImage, 0, NULL, NULL);

#endif // NON_OPTIMIZED || READ4 
	char* outImage1 = (char*)"save1.jpg";
	writeImage(outImage1, imageWidth, imageHeight, outputImage);

	clReleaseMemObject(d_inputImage);
	clReleaseMemObject(d_outputImage);
	clReleaseMemObject(d_filter);
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);
	system("pause");
	return 0;
}
