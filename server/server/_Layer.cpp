#include "_Layer.h"
inline double rnd(double a)
{
	return double(rand() % 201-100) / 1000;
}
_Layer::_Layer(int input_num, int output_num,double _alpha,int _type,double(*_activate)(double), double(*_activate_)(double))
{
	assert(input_num > 0);
	assert(output_num > 0);
	assert(0 < _alpha && _alpha < 1);
	b = double((rand() % 101)) / 100;
	alpha = _alpha;
	in_num = input_num;
	out_num = output_num;
	weight = _Matrix(in_num, out_num);
	weight.elem_apply(rnd);
	//weight.show();
	net = _Matrix(1, out_num);
	output = _Matrix(1, out_num);
	input = _Matrix(1, in_num);
	error = _Matrix(1, out_num);
	activate = _activate;
	activate_ = _activate_;
	type = _type;
}
pair<int, int> _Layer::get_shape(void)
{
	return make_pair(in_num, out_num);
}
void _Layer::set_weight(const _Matrix& w)
{
	weight = w;
}
//void _Layer::data_input(const _Matrix& data)
//{
//	input = data;
//}
_Matrix _Layer::farward(const _Matrix& data)
{
	input = data;
	net = input * weight + b;
	//cout << "??\n";
	//net.show();
	//cout << "!!\n";
	output = net;
	//cout << "O:";
	//output.show();
	output.elem_apply(activate);
	//cout << "A:";
	//output.show();
	return output;
}
_Matrix _Layer::predict(const _Matrix& data)const
{
	_Matrix tmp=data * weight;
	tmp.elem_apply(activate);
	return tmp;
}
_Matrix _Layer::feedback(const _Matrix& error)//????
{
	//cout << error.get_row_num() << endl;
	//cout << output.get_col_num() << endl;
	assert(error.get_col_num() == output.get_col_num());

	_Matrix back;
	//if (type == 1)//Êä³ö²ã
	{
		_Matrix tmp(output);
		tmp.elem_apply(activate_);
		//cout << "O:\n"; output.show();
		//cout << "T:\n";tmp.show();
		//cout << "E:\n";error.show();
		//cout << "T^E:\n";(error ^ tmp).show();
		//cout << "I:\n";input.show();
		back = (error ^ tmp * (-1))*weight.T();//(1*in_num)=(1*out_num)*(in_num*out_num)
		_Matrix derta = input.T()* (error ^ tmp * (-1));//(in_num*out_num)=(in_num*1)*(1*out_num)
		//cout <<'(' << derta.get_row_num() << ',' << derta.get_col_num() << ")\n";
		//derta.show();
		//system("pause");
		weight = weight + derta * alpha * (-1);
	}
	return back;
}
_Matrix _Layer::get_weight(void)const
{
	return weight;
}
_Layer::~_Layer(void)
{
}