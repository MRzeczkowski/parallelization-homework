#include <limits>
#include <iostream>
#include <cmath>

bool isInBound(long double x1, long double x2)
{
    long double diffX1 = x1 - 2.0;
    long double diffX2 = x2 - 1.0;

    return (pow(diffX1, 2) / 4 + pow(diffX2, 2) / 9) <= 1.0;
}

int main()
{
    long double min = INT_MAX;
    long double epsilon = 10.0e-9;

    long double x1 = 2.0, x2 = 1.0;
    long double minX1 = 2.0, minX2 = 1.0;

    for (x1 = 2.0; isInBound(x1, x2); x1 -= epsilon)
    {
        for (x2 = 1.0; isInBound(x1, x2); x2 += epsilon)
        {
            long double val = sin(2 * x2) + 0.1 * pow(x1, 2) + cos(x1 * x2);

            if (val < min)
            {
                minX1 = x1;
                minX2 = x2;
                min = val;
            }
        }
    }
}
