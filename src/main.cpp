#include <iostream>

#include "verificationTest.h"

double nthRoot(double num, double frac);
double power(double base, double exp);

int main(int , char**)
{
	using namespace nsNumber;
	nsTest::Tester tester;
	//tester.runAll();
	BigNumber base("2.5");
	BigNumber exp("2.5");

	std::cout << pow(base, exp) << '\n';

    std::cout << power(2.5, 2.5) << '\n';

	return 0;
}

double power(double base, double exp)
{
    int wholePartOfExp = static_cast<int>(exp);
    double fractionPartOfExp = (exp - wholePartOfExp);

    double ans = base;
    while (--wholePartOfExp)
    {
        ans *= base;
    }
    double multiPlier = 1;
    if (fractionPartOfExp)
    {
        multiPlier = nthRoot(base, fractionPartOfExp);
    }
    printf("Ans = %f, Multiplier = %f\n", ans, multiPlier);
    ans = ans * multiPlier;
    return ans;
}

double nthRoot(double num, double frac)
{
    // initially guessing a random number between
    // 0 and 9
    //double xPre = rand() % 10;
    double xPre = 1;

    //  smaller eps, denotes more accuracy
    double eps = 1e-3;

    // initializing difference between two
    // roots by INT_MAX
    double delX = INT_MAX;

    //  xK denotes current value of x
    double xK{};

    double n_minus_1 = (1 / frac) - 1;
    //  loop until we reach desired accuracy
    while (delX > eps)
    {
        //  calculating current value from previous
        // value by newton's method
        double p = pow(xPre, n_minus_1);
        double b1 = n_minus_1 * xPre;
        double b2 = num / p;
        double c = b1 + b2;
        xK = c * frac;
        delX = abs(xK - xPre);
        printf("p = %f, b1 = %f, b2 = %f, c = %f, xk = %f, xPre = %f, delX = %f\n", p, b1, b2, c, xK, xPre, delX);
        xPre = xK;
    }

    return xK;
}

/*
p = 1.000000, b1 = 1.000000, b2 = 2.500000, c = 3.500000, xk = 1.750000, xPre = 1.750000, delX = 0.750000
p = 1.750000, b1 = 1.750000, b2 = 1.428571, c = 3.178571, xk = 1.589286, xPre = 1.589286, delX = 0.160714
p = 1.589286, b1 = 1.589286, b2 = 1.573034, c = 3.162319, xk = 1.581160, xPre = 1.581160, delX = 0.008126
p = 1.581160, b1 = 1.581160, b2 = 1.581118, c = 3.162278, xk = 1.581139, xPre = 1.581139, delX = 0.000021
Ans = 6.250000, Multiplier = 1.581139
9.88212
*/