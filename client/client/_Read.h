#pragma once
#pragma warning(disable : 4996)
#include<fstream>
#include<iostream>
#include <opencv2/opencv.hpp>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc.hpp>
#include"_Matrix.h"
#include<io.h>
#include<stdio.h>
#include<cstring>
#include<Windows.h>
using namespace std;
using namespace cv;


_Matrix imageread(string filename)
{
	Mat image = imread(filename);
	int iColumn = (image.cols) * (image.channels());
	int iRow = image.rows;
	_Matrix tmp(1, image.cols * iRow);
	for (size_t i = 0; i < iRow; i++)
	{
		uchar* data = image.ptr<uchar>(i);
		for (size_t j = 0; j < iColumn; j+=3)
		{
			double iPIX = data[j]/255.0;
			//printf("%.3f", iPIX);
			tmp.set_value(0, j/3 + i * image.cols, iPIX);
		}
		//cout << endl;
	}
	//system("pause");
	return tmp;
}
void find(string path,int l,vector<_Matrix>& data,vector<_Matrix>& label)
{
	if (path == "未输入路径") path = "*.*";
	int index = 0;
	_finddata_t fileInfo;
	auto handle = _findfirst((path+"\\*.png").c_str(), &fileInfo);//_findfirst（） 没找到文件时，返回-1，找到了就返回句柄
	if (handle == -1) return;
	else
	{
	}
	if (0 == _findnext(handle, &fileInfo)) //_findnext() 找到文件时返回0，没找到文件时返回-1
	{
	}
	else return;
	while (1)
	{
		//没找到下一个文件 ？跳出循环　：继续执行
		if (-1 == _findnext64i32(handle, &fileInfo)) break;
		if (fileInfo.attrib == _A_SUBDIR)
		{
			cout << (path + "/" + fileInfo.name) << endl;
		}
		else
		{
			//cout << (path + "/" + fileInfo.name) << endl;
			_Matrix td = imageread(path + "\\" + fileInfo.name);
			_Matrix la(1, 10);
			la.set_value(0, l, 1);
			data.push_back(td);
			label.push_back(la);
		}
	}
}
void test(vector<_Matrix>& data,vector<_Matrix>& label)
{
    //const char* filename = "C:\\Users\\12151\\Desktop\\夏季学期\\test\\mnist_train\\0\\mnist_train_1.png";
    string filename = "C:\\Users\\12151\\Desktop\\summer\\test\\mnist_train\\";
    //find(filename);
	for (int i = 0; i < 10; i++)
	{
		find(filename + char(i + '0'), i, data, label);
	}
}

