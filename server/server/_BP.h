#pragma once
#include<vector>
#include<math.h>
#include<random>
#include"_Matrix.h"
#include"_Layer.h"
using namespace std;

inline double sigmoid(double x)
{
	return 1 / (1 + exp(-x));
}
inline double sigmoid_(double x)
{
	return x*(1-x);
}
class _BP
{
	vector<_Layer> list;
	double alpha;
	int in_num;
	int out_num;
public:
	_BP(int input_num,int output_num,double _alpha=0.5);
	void add(int number, double (*activate)(double) = sigmoid, double (*activate_)(double) = sigmoid_);
	void fit(const vector<_Matrix>& train_data, const vector<_Matrix>& label,double rate=1);
	void set_weights(const vector<_Matrix>& w);
	vector<_Matrix> get_weight(void);
	_Matrix predict(const _Matrix& testdata)const;
	vector<_Matrix> predict(const vector<_Matrix>& testdata)const;
};

