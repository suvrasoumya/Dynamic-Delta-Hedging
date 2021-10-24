#pragma once
#ifndef OPTION_H
#define OPTION_H


class Option
{
public:
	Option(double, double, double, double, double);
	Option();
	void init();
	char flag = 'c';
	void Set_flag(char val) { flag = val; }
	double BlackScholes();

	double price;
	double cdf_d1;
	double cdf_d2;

	double strike;
	double price_s;
	double rfr;
	double t;
	double sigma;

};

#endif // OPTION_H