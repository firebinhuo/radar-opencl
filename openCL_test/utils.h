
#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <CL/cl.h>
using namespace std;
//传入图片地址，输出二维vecto
vector<vector<uchar> > decode(char* path);
//传入二维vecotr，显示输出的图片，并保存图片到指定地址
void code(vector<vector<uchar> > array, char* path);

cl_int ConvertToString(const char *pFileName, std::string &Str);
