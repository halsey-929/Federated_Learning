#pragma once
#include<iostream>
#include<random>
#include<assert.h>
using namespace std;
class _Matrix
{
	double** M;
	int m;//row_num
	int n;//col_num
public:
	_Matrix(void);
	_Matrix(int m, int n);
	_Matrix(const _Matrix& other);
	double get_value(int row, int col)const;
	double sum(void)const;
	void set_value(int row, int col, double value);
	void elem_apply(double (*function)(double));
	int get_row_num(void)const;
	int get_col_num(void)const;
	_Matrix& operator=(const _Matrix& other);
	_Matrix& operator=(const double number);
	_Matrix T(void);
	_Matrix& operator^=(const _Matrix& other);
	_Matrix operator^(const _Matrix& other)const;
	_Matrix& operator*=(const _Matrix& other);
	_Matrix& operator*=(const double number);
	_Matrix& operator+=(const _Matrix& other);
	_Matrix& operator+=(const double number);
	_Matrix operator*(const _Matrix& other)const;
	_Matrix operator*(const double number)const;
	_Matrix operator+(const _Matrix& other)const;
	_Matrix operator+(const double number)const;

	~_Matrix(void);

	//
	void show(void)const;
};
