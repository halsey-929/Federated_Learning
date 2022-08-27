#include "_Matrix.h"
void _Matrix::show(void)const
{
	assert(M);
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cout << M[i][j] << ' ';
		}
		cout << '\n';
	}
}
double _Matrix::sum(void)const
{
	double ans = 0;
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			ans += M[i][j];
	return ans;
}
double _Matrix::get_value(int row, int col)const
{	
	assert(M);
	assert(0 <= row && row <= m);
	assert(0 <= col && col <= n);
	return M[row][col];
}
void _Matrix::set_value(int row, int col, double value)
{
	assert(M);
	assert(0 <= row && row <= m);
	assert(0 <= col && col <= n);
	M[row][col] = value;
}
int _Matrix::get_col_num(void)const
{
	return n;
}
int _Matrix::get_row_num(void)const
{
	return m;
}
void _Matrix::elem_apply(double (*function)(double))
{
	assert(M);

	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			M[i][j] = function(M[i][j]);
}
_Matrix _Matrix::T(void)
{
	assert(M);
	_Matrix tmp(n, m);
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			tmp.M[j][i] = M[i][j];
	return tmp;
}
_Matrix::_Matrix(void)
{
	m = n = 0;
	M = NULL;
}
_Matrix::_Matrix(int _m, int _n)
{
	M = NULL;
	m = 0;
	n = 0;
	if (_m <= 0 || _n <= 0)return;
	m = _m;
	n = _n;
	M = new double* [m];
	for (int i = 0; i < m; i++)
	{
		M[i] = new double[n];
		for (int j = 0; j < n; j++)
			M[i][j] = 0;
	}
}
_Matrix::~_Matrix(void)
{
	if (M == NULL)
	{
		n = m = 0;
		return;
	}

	for (int i = 0; i < m; i++)
	{
		delete[]M[i];
	}
	delete[]M;
	M = NULL;
	m = n = 0;
}
_Matrix::_Matrix(const _Matrix& other)
{
	n = m = 0;
	M = NULL;
	*this = other;
}
_Matrix& _Matrix::operator=(const _Matrix& other)
{
	assert(other.M);
	assert((other.n != 0 && other.m != 0) || other.n == other.m);
	if (M != NULL) 
	{
		for (int i = 0; i < m; i++)
		{
			delete[]M[i];
		}
		delete[]M;
	}

	if (other.n == 0 && other.m == 0)
	{
		n = m = 0;
		M = NULL;
		return *this;
	}

	n = other.n;
	m = other.m;
	M = new double* [m];
	for (int i = 0; i < m; i++)
	{
		M[i] = new double[n];
		for (int j = 0; j < n; j++)
			M[i][j] = other.M[i][j];
	}
	return *this;
}
_Matrix& _Matrix::operator=(const double number)
{
	assert(M);
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			M[i][j] = number;
	return *this;
}
_Matrix _Matrix::operator*(const _Matrix& other)const
{
	assert(other.M);
	assert(n == other.m);

	_Matrix ans(m, other.n);
	for (int i = 0; i < m; i++)
		for (int k = 0; k < n; k++)
			for (int j = 0; j < other.n; j++)
				ans.M[i][j] += M[i][k] * other.M[k][j];
	return ans;
}
_Matrix& _Matrix::operator*=(const _Matrix& other)
{
	assert(M);
	assert(other.M);
	assert(n == other.m);

	*this = *this * other;
	return *this;
}
_Matrix& _Matrix::operator*=(const double number)
{
	assert(M);
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			M[i][j] *= number;
	return *this;
}
_Matrix _Matrix::operator*(const double number)const
{
	assert(M);
	_Matrix tmp(*this);
	return tmp *= number;
}
_Matrix& _Matrix::operator+=(const _Matrix& other)
{
	assert(M);
	assert(other.M);
	assert(n == other.n);
	assert(m == other.m);

	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			M[i][j] += other.M[i][j];
	return *this;
}
_Matrix _Matrix::operator+(const _Matrix& other)const
{
	assert(M);
	assert(other.M);
	assert(n == other.n);
	assert(m == other.m);

	_Matrix tmp(*this);
	return tmp += other;
}
_Matrix& _Matrix::operator+=(const double number)
{
	assert(M);

	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			M[i][j] += number;
	return *this;
}
_Matrix _Matrix::operator+(const double number)const
{
	assert(M);
	_Matrix tmp(*this);
	return tmp += number;
}

_Matrix& _Matrix::operator^=(const _Matrix& other)
{
	assert(M);
	assert(other.M);
	assert(n == other.n && m == other.m);
	
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			M[i][j] *= other.M[i][j];
	return *this;
}
_Matrix _Matrix::operator^(const _Matrix& other)const
{
	assert(M);
	assert(other.M);
	assert(n == other.n && m == other.m);

	_Matrix ans(*this);
	return ans ^= other;
}