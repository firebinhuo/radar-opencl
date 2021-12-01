#include "utils.h"
#include <CL/cl.h>
#include <string>
#include <stdio.h>
#include <fstream>
#include <vector>

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

vector<vector<uchar> > decode(char* path)   //pathΪͼƬ·��
{
	Mat img = imread(path);                // ��ͼƬ����Mat������
//       ��ʾԭͼƬ
	  // namedWindow("old", WINDOW_NORMAL);
	  // imshow("old", img);
	  //waitKey(0);
	int w = img.cols * img.channels();     //����Ϊ3ͨ�������Ҫ��ͼƬ��ͨ����
	int h = img.rows;

	vector<vector<uchar> > array(h, vector<uchar>(w));      //��ʼ����άvector
	for (int i = 0; i < h; i++)
	{
		uchar* inData = img.ptr<uchar>(i);            //ptrΪָ��ͼƬ����ָ�룬����iΪ����
		for (int j = 0; j < w; j++)
		{
			array[i][j] = inData[j];
		}
	}
	return array;
}


//�����άvecotr����ʾ�����ͼƬ��������ͼƬ��ָ����ַ
void code(vector<vector<uchar> > array, char* path)
{
	size_t h = array.size();
	size_t w = array[0].size();
	//��ʼ��ͼƬ�����س���
	Mat img(h, (size_t)(w / 3), CV_8UC3);           //����ΪRGB��ͼ����������Ҫ����3��
	for (size_t i = 0; i < h; i++)
	{
		uchar* outData = img.ptr<uchar>(i);
		for (size_t j = 0; j < w; j++)
		{
			outData[j] = array[i][j];
		}
	}
	namedWindow("new", WINDOW_NORMAL);
	imshow("new", img);
	waitKey(2000);
	imwrite("save.jpg", img);
}

// ��cl�ļ�����תΪ�ַ���
cl_int ConvertToString(const char *pFileName, std::string &Str)
{

	size_t		uiSize = 0;
	size_t		uiFileSize = 0;
	char		*pStr = NULL;
	std::fstream fFile(pFileName, (std::fstream::in | std::fstream::binary));


	if (fFile.is_open())
	{
		fFile.seekg(0, std::fstream::end);
		uiSize = uiFileSize = (size_t)fFile.tellg();  // ����ļ���С
		fFile.seekg(0, std::fstream::beg);
		pStr = new char[uiSize + 1];

		if (NULL == pStr)
		{
			fFile.close();
			return 0;
		}

		fFile.read(pStr, uiFileSize);				// ��ȡuiFileSize�ֽ�
		fFile.close();
		pStr[uiSize] = '\0';
		Str = pStr;

		delete[] pStr;

		return 0;
	}

	cout << "Error: Failed to open cl file\n:" << pFileName << endl;

	return -1;
}

