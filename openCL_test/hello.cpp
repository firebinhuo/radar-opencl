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
//	size_t gidx = get_global_id(0); //��һά�ȵ�ȫ��ID
//	size_t gidy = get_global_id(1);//�ڶ�ά�ȵ�ȫ��ID
//	size_t lidx = get_global_id(0);
//	buer[gidx + 4 * gidy] = (1 << gidx) | (0 x10 << gidy);
//
//});

int t_main() {
	cout << "hello openCL" << endl;

	/*************************
	*	  ����������         *
	**************************/

	cl_int			iStatus = 0; //��������״̬��Ϊ0������Ϊ1������  
	cl_uint			uiNumPlatforms = 0;//ƽ̨����  uint��ʾ�޷����������ݣ�32λ��
	cl_platform_id	Platform = NULL;  //ѡ��ĵ�ǰƽ̨
	size_t			uiSize = 0;		// ƽ̨�汾�����ֽ���	
	cl_int			iErr = 0;				// ���ز���
	char			*pName = NULL;				// ƽ̨�汾��

	cl_uint			uiNumDevices = 0;//�豸����
	cl_device_id	*pDevices = NULL;				// �豸

	cl_context		Context = NULL;				// �豸����
	cl_command_queue	CommandQueue = NULL;			// �������

	const char		*pFileName = "HelloWorld_Kernel.cl";	// cl�ļ���
	string			strSource = "";				// ���ڴ洢cl�ļ��еĴ���
	const char		*pSource;							// �����ַ���ָ��
	size_t			uiArrSourceSize[] = { 0 };			// �����ַ�������


	cl_program		Program = NULL;				// �������
	const char		*pInput = "gdkknvnqkc";		// �����ַ���
	size_t			uiStrlength = strlen(pInput);	// �����ַ�������
	char			*pOutput = NULL;				// ����ַ���
	cl_mem			memInutBuffer = NULL;				// �����ڴ����
	cl_mem			memOutputBuffer = NULL;				// ����ڴ����

	cl_kernel		Kernel = NULL;				// �ں˶���
	size_t			uiGlobal_Work_Size[1] = { 0 };		// �����趨�ں˷ֲ�	

	//-------------------1. ��ò�ѡ�����ƽ̨-----------------------------
		// ��ѯ���õ�ƽ̨������������״̬
	iStatus = clGetPlatformIDs(0, NULL, &uiNumPlatforms);
	if (CL_SUCCESS !=iStatus)
	{
		cout << "Error: Getting platforms error" << endl;
		return 0;
	}
	// ���ƽ̨��ַ
	if (uiNumPlatforms > 0)  // ����п���ƽ̨
	{
		// ����ƽ̨��Ϊƽ̨�����ڴ�ռ�
		cl_platform_id *pPlatforms = (cl_platform_id *)malloc(uiNumPlatforms * sizeof(cl_platform_id));

		// ��ÿ��õ�ƽ̨
		iStatus = clGetPlatformIDs(uiNumPlatforms, pPlatforms, NULL);
		Platform = pPlatforms[0];	// ��õ�һ��ƽ̨�ĵ�ַ
		free(pPlatforms);			// �ͷ�ƽ̨ռ�õ��ڴ�ռ�
	}
	// ���ƽ̨�汾��
	// ���ƽ̨�汾�����ֽ���
	iErr = clGetPlatformInfo(Platform, CL_PLATFORM_VERSION, 0, NULL, &uiSize);

	// �����ֽ���Ϊƽ̨�汾�������ڴ�ռ�
	pName = (char *)alloca(uiSize * sizeof(char));

	// ���ƽ̨�汾����
	iErr = clGetPlatformInfo(Platform, CL_PLATFORM_VERSION, uiSize, pName, NULL);
	cout << pName << endl;


	//--------------2. ��ѯGPU�豸����ѡ������豸------------------------
	// ���GPU�豸����
	iStatus = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_GPU, 0, NULL, &uiNumDevices);
	if (0 == uiNumDevices)	// ���û��GPU�豸
	{
		cout << "No GPU device available." << endl;
		cout << "Choose CPU as default device." << endl;

		// ѡ��CPU��Ϊ�豸������豸��
		iStatus = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_CPU, 0, NULL, &uiNumDevices);

		// Ϊ�豸����ռ�
		pDevices = (cl_device_id *)malloc(uiNumDevices * sizeof(cl_device_id));

		// ���ƽ̨
		iStatus = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_CPU, uiNumDevices, pDevices, NULL);
	}
	else
	{
		pDevices = (cl_device_id *)malloc(uiNumDevices * sizeof(cl_device_id));
		// ���ƽ̨ ΪGPU
		iStatus = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_GPU, uiNumDevices, pDevices, NULL);
	}
	return 0;
}