
#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <CL/cl.h>
using namespace std;
//����ͼƬ��ַ�������άvecto
vector<vector<uchar> > decode(char* path);
//�����άvecotr����ʾ�����ͼƬ��������ͼƬ��ָ����ַ
void code(vector<vector<uchar> > array, char* path);

cl_int ConvertToString(const char *pFileName, std::string &Str);
