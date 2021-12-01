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

vector<vector<uchar> > decode(char* path)   //path为图片路径
{
	Mat img = imread(path);                // 将图片传入Mat容器中
//       显示原图片
	  // namedWindow("old", WINDOW_NORMAL);
	  // imshow("old", img);
	  //waitKey(0);
	int w = img.cols * img.channels();     //可能为3通道，宽度要乘图片的通道数
	int h = img.rows;

	vector<vector<uchar> > array(h, vector<uchar>(w));      //初始化二维vector
	for (int i = 0; i < h; i++)
	{
		uchar* inData = img.ptr<uchar>(i);            //ptr为指向图片的行指针，参数i为行数
		for (int j = 0; j < w; j++)
		{
			array[i][j] = inData[j];
		}
	}
	return array;
}


//传入二维vecotr，显示输出的图片，并保存图片到指定地址
void code(vector<vector<uchar> > array, char* path)
{
	size_t h = array.size();
	size_t w = array[0].size();
	//初始化图片的像素长宽
	Mat img(h, (size_t)(w / 3), CV_8UC3);           //保存为RGB，图像列数像素要除以3；
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

// 将cl文件代码转为字符串
cl_int ConvertToString(const char *pFileName, std::string &Str)
{

	size_t		uiSize = 0;
	size_t		uiFileSize = 0;
	char		*pStr = NULL;
	std::fstream fFile(pFileName, (std::fstream::in | std::fstream::binary));


	if (fFile.is_open())
	{
		fFile.seekg(0, std::fstream::end);
		uiSize = uiFileSize = (size_t)fFile.tellg();  // 获得文件大小
		fFile.seekg(0, std::fstream::beg);
		pStr = new char[uiSize + 1];

		if (NULL == pStr)
		{
			fFile.close();
			return 0;
		}

		fFile.read(pStr, uiFileSize);				// 读取uiFileSize字节
		fFile.close();
		pStr[uiSize] = '\0';
		Str = pStr;

		delete[] pStr;

		return 0;
	}

	cout << "Error: Failed to open cl file\n:" << pFileName << endl;

	return -1;
}

