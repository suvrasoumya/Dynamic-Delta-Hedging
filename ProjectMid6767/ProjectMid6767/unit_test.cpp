#include "unit_test.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

using namespace std;


bool unit_test::unit_test_Price(double check) {
    return abs(price_trueval - check) / check < 0.001;
}
bool unit_test::unit_test_IV(double check) {
    return abs(ivol_trueval - check) / check < 0.01;
}
bool unit_test::unit_test_Delta(double check) {
    return abs(delta_trueval - check) / check < 0.001;
}

