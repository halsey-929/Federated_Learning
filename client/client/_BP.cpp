#include "_BP.h"
_BP::_BP(int input_num,int output_num,double _alpha)
{
	alpha = _alpha;
	in_num = input_num;
	out_num = output_num;
	_Layer tmp(in_num, out_num, alpha, 0, sigmoid, sigmoid_);
	list.push_back(tmp);
}
void _BP::add(int number, double (*activate)(double),double (*activate_)(double))
{
	int n = list.size();
	int pre_num = list[n - 1].get_shape().first;
	list.pop_back();
	list.push_back(_Layer(pre_num, number, alpha, 0, activate, activate_));
	list.push_back(_Layer(number, out_num, alpha, 0, activate, activate_));
}
void _BP::fit(const vector<_Matrix>& train_data, const vector<_Matrix>& label,double rate)
{
	assert(train_data.size() == label.size()&&label.size());


	int num = label.size();
	for (int i = 0; i < num*rate; i++)
	{
		assert(out_num == label[i].get_col_num() && label[i].get_row_num() == 1);
		//cout << train_data[i].get_col_num() << endl;
		assert(in_num == train_data[i].get_col_num() && train_data[i].get_row_num() == 1);


		_Matrix output(train_data[i]);
		for (auto& node : list)
			output = node.farward(output);
		double s = output.sum();
		//cout << ":\n"; output.show();
		//cout << s;
		_Matrix error = label[i] + output * ( - 1 );
		//cout << "E:";
		//error.show();
		for (int j = list.size() - 1; j >= 0; j--)
			error = list[j].feedback(error);
		if (i % 100 == 0)
		{
			cout << double(i) / (num*rate) * 100 << "%\n";
		}
	}
}
void _BP::set_weights(const vector<_Matrix>& w)
{
	assert(w.size() > 0);
	bool match = w.size()==list.size();
	for (int i = 0; match &&i < w.size(); i++)
	{
		match &= w[i].get_row_num() == list[i].get_shape().first;
		match &= w[i].get_col_num() == list[i].get_shape().second;
	}
	if (!match)
	{
		list.clear();
		in_num = w[0].get_row_num();
		out_num = w[w.size() - 1].get_col_num();
		_Layer tmp(in_num, out_num, alpha, 0, sigmoid, sigmoid_);
		list.push_back(tmp);
		for (int i = 0; i < w.size()-1; i++)
		{
			add(w[i].get_col_num());
		}
	}
	for (int i = 0; i < list.size(); i++)
	{
		list[i].set_weight(w[i]);
	}
}
vector<_Matrix> _BP::get_weight(void)
{
	vector<_Matrix> w;
	for (int i = 0; i < list.size(); i++)
		w.push_back(list[i].get_weight());
	return w;
}
_Matrix _BP::predict(const _Matrix& testdata)const
{
	assert(testdata.get_col_num() == in_num);
	assert(testdata.get_row_num() == 1);
	_Matrix output=testdata;
	for (const auto& node : list)
	{
		output=node.predict(output);
	}
	//output *= 1 / output.sum();
	return output;
}
vector<_Matrix> _BP::predict(const vector<_Matrix>& testdata)const
{
	vector<_Matrix> ans;
	for (int i = 0; i < testdata.size(); i++)
		ans.push_back(predict(testdata[i]));
	return ans;
}