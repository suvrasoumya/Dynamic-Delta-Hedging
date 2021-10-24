#include "Option.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <algorithm>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream> // std::stringstream
using namespace std;


Option::Option(double X, double P, double r, double T, double vol) {

    strike = X;
    price_s = P;
    rfr = r;
    t = T;
    sigma = vol;

    price = BlackScholes();

}

Option::Option() {
    init();

}
void Option::init() {
    strike = 100;
    price_s = 100;
    rfr = 0.05;
    t = 2;
    sigma = 0.3;

    price = 0;
    cdf_d1 = 0;
    cdf_d2 = 0;

}

// Define the constants for Normal Cumulative Distribution function
char flag = 'C';
const double a = 0.319381530;
const double b = -0.35653782;
const double c = 1.781477937;
const double d = -1.821255978;
const double e = 1.330274429;
const double f = 0.2316419;
const double g = 0.39894228;     // Value for 1/sqrt(2*pi);


double N(double x) {     // Standard Normal cdf referenced from Quantstart

    double k = 1.0 / (1.0 + f * x);
    double k_sum = k * (a + k * (b + k * (c + k * (d + e * k))));

    if (x >= 0.0) {
        return (1.0 - (1.0 / (pow(2 * M_PI, 0.5))) * exp(-0.5 * x * x) * k_sum);
    }
    else {
        return 1.0 - N(-x);
    }

}

double Option::BlackScholes() {

    double d1, d2;
    d1 = (log(price_s / strike) + (rfr + sigma * sigma * 0.5) * t) / (sigma * pow(t, 0.5));
    d2 = d1 - (sigma * pow(t, 0.5));
    //cout << d1 << "     " << d2 << endl;
    double c;
    c = price_s * N(d1) - strike * exp(-rfr * t) * N(d2);
    if (flag == 'C' || flag == 'c') {
        price = c;

    }
    else {
        price = strike * exp(-rfr * t) - price_s + c;    // By Put-Call parity, the price of Put option
    }
    cdf_d1 = N(d1);
    cdf_d2 = N(d2);

    return price;

}
