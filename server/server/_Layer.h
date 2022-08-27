#pragma once
#include"_Matrix.h"
#include<random>
using namespace std;
class _Layer
{
	_Matrix weight;
	_Matrix net;
	_Matrix output;
	_Matrix input;
	_Matrix error;
	double b;
	int in_num;
	int out_num;
	double (* activate) (double);
	double (*activate_) (double);
	double alpha;
	int type;//0Ϊ�����㣬1Ϊ����㣬2Ϊ�����
public:
	_Layer(int input_num,int output_num,double _alpha,int _type,double (*activate) (double),double (*activate_) (double));
	pair<int, int> get_shape(void);
	void set_weight(const _Matrix& w);
	//void data_input(const _Matrix& data);
	_Matrix farward(const _Matrix& data);
	_Matrix feedback(const _Matrix& error);
	_Matrix predict(const _Matrix& data)const;
	_Matrix get_weight(void)const;
	~_Layer(void);
};


