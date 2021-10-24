// ProjectMid6767.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>
#include <random>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/format.hpp>
#include "unit_test.h"
#include "Option.h"
#include <exception>
using namespace std;

//class myexception : public exception
//{
//    virtual const char* what() const throw()
//    {
//        return "My exception happened";
//    }
//} myex;
void csv_out(string name, vector <vector <double>> data) {

    ofstream myfile(name);
    int vsize = data.size(), vsize1 = data[0].size();

    for (int i = 0; i < vsize; i++)
    {
        for (int j = 0; j < vsize1; j++)
            myfile << data[i][j] << ",";
        myfile << endl;
    }

}   // csv output reference from stackoverflow

void csv_out1(string name, vector <vector <string>> data) {

    ofstream myfile(name);
    int vsize = data.size(), vsize1 = data[0].size();

    for (int i = 0; i < vsize; i++)
    {
        for (int j = 0; j < vsize1; j++)
            myfile << data[i][j] << ",";
        myfile << endl;
    }

}
vector <vector<string>> read_csv(string path, string mask) {
    bool to_consider = false;
    bool for_col = false;

    ifstream infile(path);
    if (!infile.is_open()) throw std::runtime_error("Could not open file");

    string line, colname;
    string delimiter = ",", token;
    size_t pos = 0;
    vector <vector<string>> final_file;

    while (getline(infile, line))
    {
        if (line.find(mask) == 0) { // mask is the string in the first column name
            to_consider = true;
            for_col = true; // mask that is activated in column row to store column names for later

        }
        vector <string> data_tmp;
        if (to_consider) {

            if (!for_col) {
                while ((pos = line.find(delimiter)) != std::string::npos) {
                    token = line.substr(0, pos);
                    data_tmp.push_back(token);
                    line.erase(0, pos + delimiter.length());
                }
                data_tmp.push_back(line);
                final_file.push_back(data_tmp);
            }
            else {
                while ((pos = line.find(delimiter)) != std::string::npos) {
                    token = line.substr(0, pos);
                    data_tmp.push_back(token);
                    line.erase(0, pos + delimiter.length());
                }
                data_tmp.push_back(line);
                final_file.push_back(data_tmp);
            }

        }
        for_col = false;


    }
    // Close file
    infile.close();
    return final_file;

}

void print(vector <vector <string>>file) {
    int row = file.size(), col = file[0].size();
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < col; j++) {
            cout << file[i][j] << ", ";
        }
        cout << endl;
    }

    cout << row << "   cols  " << col;
    //cout << "Test stuff" << endl;
}

vector<double>  normal_dist(int n, int p)
{
    const int nrolls = 10000;  // number of experiments
    vector <double> vec;
    std::default_random_engine generator;
    generator.seed(p);
    std::normal_distribution<double> distribution(0.0, 1.0);
    int div = 10000 / n;

    for (int i = 0; i < nrolls; ++i) {
        double number = distribution(generator);
        if (i % div == 0) {
            vec.push_back(number);
        }
    }


    return vec;
}

vector <vector<double>> HedgingError(vector <vector<double>> S_t, vector <vector<double>> C_t, vector <vector<double>> delta, double delt, vector<double> r, double T) {
    vector <vector<double>> HE_t;
    int size2 = C_t[0].size(), SP = C_t.size();
    for (int i = 0; i < SP; i++) {
        vector<double> HE, B, del_t;
        double B0 = C_t[i][0] - delta[i][0] * S_t[i][0], tmpHE;
        B.push_back(B0);
        for (int j = 1; j < size2; j++) {

            B0 = delta[i][j - 1] * S_t[i][j] + B[j - 1] * exp(r[j-1] * delt) - delta[i][j] * S_t[i][j];
            B.push_back(B0);
            tmpHE = delta[i][j - 1] * S_t[i][j] + B[j - 1] * exp(r[j-1] * delt) - C_t[i][j];
            HE.push_back(tmpHE);
        }


        HE_t.push_back(HE);
    }
    csv_out("delta.csv", delta);
    return HE_t;
}

vector<double> getIV(double S, double V, double riskfree, double strike_GOOG, double ttm) {

    double iv = 0.0, lb = 0.0, ub = 1.0, threshold = 0.000001, err = 10, price = 0.0;
    
    int count=0;
    Option op = Option(strike_GOOG, S, riskfree, ttm, iv);
    //cout << S << "  |   " << riskfree << "  |   " << strike_GOOG << "  |   " << ttm << endl;
    while (err > threshold && count < 100) {
        iv = (ub + lb) / 2;
        op.sigma = iv;
        price = op.BlackScholes();
        err = abs(price - V);
        if (op.BlackScholes() > V) {
            ub = iv;
            op.sigma = iv;
            //cout << "ub =  " << ub <<" | " << price << endl;
        }
        else {
            lb = iv;
            op.sigma = iv;
            //cout << "lb =  " << lb << " | " << price << endl;
        }
        count++;
    }
    op.BlackScholes();
    vector <double> val{ iv, op.cdf_d1 };
    return val;
}

vector<vector<string>> filter_data(vector<vector<string>> op, string t0, string tN, string T, string convention, int col2, bool option =false, int col1 = 0) {

    boost::gregorian::date T0 = boost::gregorian::from_string(t0); // start
    boost::gregorian::date TN = boost::gregorian::from_string(tN); //end
    boost::gregorian::date T1 = boost::gregorian::from_string(T); //expiry
    cout << "Filtering out required data as per date range" << endl;

    boost::gregorian::date temp, temp1;
    vector<vector<string>> out;

    int row = op.size(), col = op[0].size();
    if (convention == "US") {
        for (int i = 1; i < row; i++) {
            temp = boost::gregorian::from_string(op[i][col2]);
            temp1 = boost::gregorian::from_string(op[i][col1]);
            if (((temp - TN).days() <= 0  && temp >= T0)) {
                //cout << (temp - TN).days() << endl;
                if (option) {
                    if (temp1 == T1 && op[i][3] == "500" && op[i][2] == "C")
                        out.push_back(op[i]);
                }
                else
                    out.push_back(op[i]);
            }

        }
    }
    
 


    return out;
}


vector<vector<string>> create_result_file(vector<string> result_column, vector<vector<string>> option_filtered, vector<double>S, vector<double>V, vector<double>IV, 
    vector<double>del_Goog, vector<double>hedge_err, vector<double>PNL, vector<double>PNL_hedge) {
    vector<vector<string>> final_result;

    final_result.push_back(result_column);
    for (int i = 0; i < hedge_err.size(); i++) {
        vector<string> row_temp;
        row_temp.push_back(option_filtered[i][0]);
        row_temp.push_back(to_string(S[i]));
        row_temp.push_back(to_string(V[i]));
        row_temp.push_back(to_string(IV[i]));
        row_temp.push_back(to_string(del_Goog[i]));
        row_temp.push_back(to_string(hedge_err[i]));
        row_temp.push_back(to_string(PNL[i]));
        row_temp.push_back(to_string(PNL[i]));
        row_temp.push_back(to_string(PNL_hedge[i]));
        final_result.push_back(row_temp);
    }
    return final_result;

}

void Task2(double strike_GOOG, string t0,  string tN, string T) {

    cout << "The Input parameters are" << endl;
    cout << strike_GOOG << endl;
    cout << t0 << endl;
    cout << tN << endl;
    cout << T << endl;
    
    
    cout << "Reading the Interest File" << endl;
    string path = "./data/interest.csv";
    string mask = "date";
    vector <vector<string>> interest_file = read_csv(path, mask);

    cout << "Reading the Options File" << endl;
    path = "./data/op_GOOG.csv";
    mask = "date";
    vector <vector<string>> option_file = read_csv(path, mask);
    vector<string> exdate, strike;


    cout << "Reading the Stock File" << endl;
    path = "./data/sec_GOOG.csv";
    mask = "date";
    vector <vector<string>> stock_file = read_csv(path, mask);

    /*cout << "Reading the Test File" << endl;
    path = "./test.csv";
    mask = "2011-07-05";
    vector <vector<string>> option_filtered = read_csv(path, mask);*/


    // t0 = 2011/07/05 tN = 2011/07/29 T = 2011/09/17 K = 500
    boost::gregorian::date T1 = boost::gregorian::from_string(T); //expiry

    cout << "Filter Options Data" << endl;
    vector<vector<string>> option_filtered = filter_data(option_file, t0, tN, T, "US" , 0, true, 1 );
    //csv_out1("test.csv", option_filtered);

    vector<string> result_column{ "date", "Stock Price", "V", "IV", "Delta", "Hedging Error", "Total Wealth (without Hedge)", "PNL (without hedge)", "PNL (with hedge)" };

    unordered_map <string, double> tw_wo_hedge;
    double position;
    for (int i = 0; i < option_filtered.size(); i++) {
        tw_wo_hedge[option_filtered[i][0]] = (stod(option_filtered[0][4]) + stod(option_filtered[0][5])) * 0.5 - (stod(option_filtered[i][4]) + stod(option_filtered[i][5])) * 0.5;
    }

    vector<double> S, V, riskfree, IV, del_Goog, hedge_err, PNL, PNL_hedge;
    cout << "Filter Stock Data" << endl;
    vector<vector<string>> stock_filter = filter_data(stock_file, t0, tN, T, "US", 0);
    cout << "Filter Risk Free Rate Data" << endl;
    vector<vector<string>> rfr_filter = filter_data(interest_file, t0, tN, T, "US", 0);
    cout << endl;
    for (int i = 0; i < stock_filter.size(); i++) {
        S.push_back(stod(stock_filter[i][1]));
        riskfree.push_back(stod(rfr_filter[i][1]) / 100.0);
    }

    cout << endl;
    double ttm;
    cout << "Calculating Option's Value from Bid and Ask" << endl;
    cout << "Calculating Option's Implied Volatility from Market and Using it to compute Black Scholes Delta" << endl;
    cout << "Calculating Total Wealth without hedge" << endl;
    for (int i = 0; i < option_filtered.size(); i++) {
        ttm = (T1 - boost::gregorian::from_string(option_filtered[i][0])).days();  //time to expiry
        V.push_back((stod(option_filtered[i][4]) + stod(option_filtered[i][5])) * 0.5);
        vector <double> ret = getIV(S[i], V[i], riskfree[i], strike_GOOG, ttm / 252);
        IV.push_back(ret[0]);  // 252 trading days in a year
        del_Goog.push_back(ret[1]);
        PNL.push_back(tw_wo_hedge[option_filtered[i][0]]);
    }
    ttm = (T1 - boost::gregorian::from_string(option_filtered[0][0])).days();
    vector<vector<double>> GoogS_t, GoogC_t, Googdelta;
    GoogS_t.push_back(S);
    GoogC_t.push_back(V);
    Googdelta.push_back(del_Goog);
    cout << "Calculating Hedging Error and PNL with Hedge" << endl;
    vector<vector<double>> he_Goog = HedgingError(GoogS_t, GoogC_t, Googdelta, 1.0 / 252.0, riskfree, ttm / 252);
    for (int i = 0; i < he_Goog[0].size()+1; i++) {
        if (i == 0) {
            hedge_err.push_back(0);
            PNL_hedge.push_back(0);
        }
        else {
            hedge_err.push_back(he_Goog[0][i-1] - hedge_err[i - 1]);
            PNL_hedge.push_back(he_Goog[0][i-1]);
        }
    }

    vector<vector<string>> final_result = create_result_file(result_column, option_filtered, S, V, IV, del_Goog, hedge_err, PNL, PNL_hedge);
    csv_out1("result.csv", final_result);



}

//vector <string> get_input() {
//    vector <string> inp{ "false" };
//
//    cout << "\n";
//    double K;
//    string t0, tN, T;
//    cout << "User defined input parameter step below " << endl;
//
//    cout << "Please enter the Strike Price" << endl;
//    while (true) {
//        if (cin >> K)
//            break;
//        else {
//            cout << "Please enter a valid input for Strike Price" << endl;
//            cin.clear();
//            while (cin.get() != '\n');
//        }
//    }
//
//    cout << "Please enter the Start Date" << endl;
//    while (true) {
//        bool c = true;
//        if (cin >> t0) {
//            try {
//                boost::gregorian::date t1 = boost::gregorian::from_string(t0);
//                //throw myex;
//            }
//            catch (exception& e) {
//                c = false;
//                cout << e.what() << '\n';
//                cout << "Please enter a valid input for Start Date" << endl;
//                cin.clear();
//                while (cin.get() != '\n');
//
//            }
//            if (c)
//                break;
//        }
//           
//    }
//
//
//    inp.push_back(to_string(K));
//    inp.push_back(t0);
//    return inp;
//}

void run_test_cases() {
    unit_test tst = unit_test();
    cout << "\n";
    cout << "Running the Test Cases - " << endl;
    double p1 = 33.406;
    double p2 = 0.548;
    double p3 = 0.853;
    tst.SetPrice(p1);
    tst.SetIV(p2);
    tst.SetDelta(p3);


    Option op1 = Option(200, 200, 0.02, 1, 0.4);
    //cout << op1.price << endl;
    vector <double> p = getIV(250, 56, 0.02, 200, 0.2);
    /*cout << p[0]<< endl;
    cout << p[1] << endl;*/


    cout << "The result for Option Price test case is " << endl;
    if (tst.unit_test_Price(op1.price))
        cout << "Test Case passed." << endl;
    else
        cout << "Test Case failed." << endl;
    cout << "\n";
    cout << "The result for Implied Volatity test case is " << endl;
    if (tst.unit_test_IV(p[0]))
        cout << "Test Case passed." << endl;
    else
        cout << "Test Case failed." << endl;
    cout << "\n";
    cout << "The result for Delta test case is " << endl;
    if (tst.unit_test_Delta(p[1]))
        cout << "Test Case passed." << endl;
    else
        cout << "Test Case failed." << endl;
    cout << "\n";
}

int main()
{

    double S0 = 100, delt = 0.4 / 100.0, mu = 0.05, sig = 0.24;
    double r = 0.025, T = 0.40, temp, Z = 1.0;
    int SP = 1000, N = 100;
    vector <vector <double>> S_t, C_t, delta, HE_t;
    vector<double> Zt;
    Option opt = Option(105, 100, r, 0.4, sig);
    cout << "Reading initial parameters" << endl;
    run_test_cases();

    if (true) {
        cout << "Executing Task 1" << endl;
        for (int j = 0; j < SP; j++) {

            vector<double> S;
            S.push_back(S0);
            Zt = normal_dist(N, j);
            for (int i = 1; i < N; i++) {
                temp = S[i - 1] + mu * S[i - 1] * delt + sig * S[i - 1] * sqrt(delt) * Zt[i - 1];
                S.push_back(temp);
            }
            S_t.push_back(S);
        }
        cout << "Writing Simulated Stocks Path file" << endl;

        csv_out("stocks.csv", S_t);


        int size1 = S_t.size(), size2 = S_t[1].size();

        for (int i = 0; i < size1; i++)
        {
            vector<double> C, del;
            for (int j = 0; j < size2; j++) {
                double t = T - (double)j * delt;

                Option bs1 = Option(105, S_t[i][j], r, t, sig);
                C.push_back(bs1.price);
                del.push_back(bs1.cdf_d1);
            }
            C_t.push_back(C);
            delta.push_back(del);
        }
        cout << "Writing Black Scholes Option prices file" << endl;

        csv_out("options.csv", C_t);

        vector<double> r1;
        for (int i = 0; i < N; i++) {
            r1.push_back(r);
        }
        HE_t = HedgingError(S_t, C_t, delta, delt, r1, T);

        cout << "Writing Hedging error file" << endl;

        csv_out("hedge_error.csv", HE_t);

    }
    // Need to write the input functions
    if (true) {
        cout << "\n";

        cout << "Executing Task 2" << endl;
        string t0= "2011-07-05", tN = "2011-07-29", T = "2011-09-17", strike_GOOG = "500";
        /*vector <string> user_input = get_input();
        if (user_input[0] == "true") {
            t0 = user_input[1];
            tN = user_input[2];
            T = user_input[3];
            strike_GOOG = user_input[4];

        }*/
        double s_g = stod(strike_GOOG);
        
        Task2(s_g, t0,  tN, T);
    }



    return 1;

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
