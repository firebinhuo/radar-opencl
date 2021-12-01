#include <iostream>
//
//#include "stdafx.h"
#include <tchar.h>
#include <CL/cl.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;


//#define KERNEL(...) # __VA_ARGS__
//
//const char * kernelSourceCode = KERNEL(
//	__kernel void hellocl(__global unit *buffer)
//{
//	size_t gidx = get_global_id(0); //第一维度的全局ID
//	size_t gidy = get_global_id(1);//第二维度的全局ID
//	size_t lidx = get_global_id(0);
//	buer[gidx + 4 * gidy] = (1 << gidx) | (0 x10 << gidy);
//
//});

int t_main() {
	cout << "hello openCL" << endl;

	/*************************
	*	  变量定义区         *
	**************************/

	cl_int			iStatus = 0; //函数返回状态，为0正常；为1，出错  
	cl_uint			uiNumPlatforms = 0;//平台个数  uint表示无符号整形数据（32位）
	cl_platform_id	Platform = NULL;  //选择的当前平台
	size_t			uiSize = 0;		// 平台版本名字字节数	
	cl_int			iErr = 0;				// 返回参数
	char			*pName = NULL;				// 平台版本名

	cl_uint			uiNumDevices = 0;//设备数量
	cl_device_id	*pDevices = NULL;				// 设备

	cl_context		Context = NULL;				// 设备环境
	cl_command_queue	CommandQueue = NULL;			// 命令队列

	const char		*pFileName = "HelloWorld_Kernel.cl";	// cl文件名
	string			strSource = "";				// 用于存储cl文件中的代码
	const char		*pSource;							// 代码字符串指针
	size_t			uiArrSourceSize[] = { 0 };			// 代码字符串长度


	cl_program		Program = NULL;				// 程序对象
	const char		*pInput = "gdkknvnqkc";		// 输入字符串
	size_t			uiStrlength = strlen(pInput);	// 输入字符串长度
	char			*pOutput = NULL;				// 输出字符串
	cl_mem			memInutBuffer = NULL;				// 输入内存对象
	cl_mem			memOutputBuffer = NULL;				// 输出内存对象

	cl_kernel		Kernel = NULL;				// 内核对象
	size_t			uiGlobal_Work_Size[1] = { 0 };		// 用于设定内核分布	

	//-------------------1. 获得并选择可用平台-----------------------------
		// 查询可用的平台个数，并返回状态
	iStatus = clGetPlatformIDs(0, NULL, &uiNumPlatforms);
	if (CL_SUCCESS !=iStatus)
	{
		cout << "Error: Getting platforms error" << endl;
		return 0;
	}
	// 获得平台地址
	if (uiNumPlatforms > 0)  // 如果有可用平台
	{
		// 根据平台数为平台分配内存空间
		cl_platform_id *pPlatforms = (cl_platform_id *)malloc(uiNumPlatforms * sizeof(cl_platform_id));

		// 获得可用的平台
		iStatus = clGetPlatformIDs(uiNumPlatforms, pPlatforms, NULL);
		Platform = pPlatforms[0];	// 获得第一个平台的地址
		free(pPlatforms);			// 释放平台占用的内存空间
	}
	// 获得平台版本名
	// 获得平台版本名的字节数
	iErr = clGetPlatformInfo(Platform, CL_PLATFORM_VERSION, 0, NULL, &uiSize);

	// 根据字节数为平台版本名分配内存空间
	pName = (char *)alloca(uiSize * sizeof(char));

	// 获得平台版本名字
	iErr = clGetPlatformInfo(Platform, CL_PLATFORM_VERSION, uiSize, pName, NULL);
	cout << pName << endl;


	//--------------2. 查询GPU设备，并选择可用设备------------------------
	// 获得GPU设备数量
	iStatus = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_GPU, 0, NULL, &uiNumDevices);
	if (0 == uiNumDevices)	// 如果没有GPU设备
	{
		cout << "No GPU device available." << endl;
		cout << "Choose CPU as default device." << endl;

		// 选择CPU作为设备，获得设备数
		iStatus = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_CPU, 0, NULL, &uiNumDevices);

		// 为设备分配空间
		pDevices = (cl_device_id *)malloc(uiNumDevices * sizeof(cl_device_id));

		// 获得平台
		iStatus = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_CPU, uiNumDevices, pDevices, NULL);
	}
	else
	{
		pDevices = (cl_device_id *)malloc(uiNumDevices * sizeof(cl_device_id));
		// 获得平台 为GPU
		iStatus = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_GPU, uiNumDevices, pDevices, NULL);
	}
	return 0;
}