#include <float.h>
#include <iostream>
#include <cmath>
#include <chrono>

double P = pow(10, 4),
       E = 1 / P;

int MaxIterX1 = 2 * P,
    MaxIterX2 = P;

struct Solution
{
    double x1;
    double x2;
    double val;
};
#pragma omp declare reduction(minimum : struct Solution : omp_out = omp_in.val < omp_out.val ? omp_in : omp_out)

int main()
{
    Solution solution;
    solution.val = DBL_MAX;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

#pragma omp parallel for reduction(minimum : solution)
    for (int i = MaxIterX1; i > 0; i--)
    {
        double x1 = 2.0 - i * E;

        for (int j = 0; j < MaxIterX2; j++)
        {
            double x2 = 1.0 + j * E;

            double c = pow(x1 - 2.0, 2.0) / 4.0 + pow(x2 - 1.0, 2.0) / 9.0;

            if (c <= 1.0)
            {
                double val = sin(2.0 * x2) + 0.1 * pow(x1, 2.0) + cos(x1 * x2);

                if (val < solution.val)
                {
                    solution.x1 = x1;
                    solution.x2 = x2;
                    solution.val = val;
                }
            }
        }
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Elapsed = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " ms" << std::endl;

    // x1 = 1.4958, x2 = 1.9999, min = -1.52168
    std::cout << "x1 = " << solution.x1
              << ", x2 = " << solution.x2
              << ", min = " << solution.val << "\n";
}
