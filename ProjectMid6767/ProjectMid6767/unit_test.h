#ifndef TEST_H
#define TEST_H


using namespace std;

class unit_test
{

public:

    void SetPrice(double val) { price_trueval = val; }
    void SetIV(double val) { ivol_trueval = val; }
    void SetDelta(double val) { delta_trueval = val; }

    
    bool unit_test_Price(double);
    bool unit_test_IV(double);
    bool unit_test_Delta(double);

private:
    double price_trueval = 0.0;
    double ivol_trueval = 0.0;
    double delta_trueval = 0.0;

};

#endif // TEST_H
