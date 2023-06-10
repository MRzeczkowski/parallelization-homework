#include <float.h>
#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>

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

void calculateSolution(int start, int end, Solution &localSolution)
{
    for (int i = start; i > end; i--)
    {
        double x1 = 2.0 - i * E;

        for (int j = 0; j < MaxIterX2; j++)
        {
            double x2 = 1.0 + j * E;

            double c = pow(x1 - 2.0, 2.0) / 4.0 + pow(x2 - 1.0, 2.0) / 9.0;

            if (c <= 1.0)
            {
                double val = sin(2.0 * x2) + 0.1 * pow(x1, 2.0) + cos(x1 * x2);

                if (val < localSolution.val)
                {
                    localSolution.x1 = x1;
                    localSolution.x2 = x2;
                    localSolution.val = val;
                }
            }
        }
    }
}

int main()
{
    Solution globalSolution;
    globalSolution.val = DBL_MAX;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    int numThreads = std::thread::hardware_concurrency();
    int iterationsPerThread = MaxIterX1 / numThreads;
    int extraIterations = MaxIterX1 % numThreads;

    std::vector<std::thread> threads(numThreads);
    std::vector<Solution> localSolutions(numThreads);

    for (int i = 0; i < numThreads; i++)
    {
        int start = start + iterationsPerThread + (i < extraIterations ? 1 : 0);
        int end = i * iterationsPerThread + std::min(i, extraIterations);
        threads[i] = std::thread(calculateSolution, start, end, std::ref(localSolutions[i]));
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    for (const auto &localSolution : localSolutions)
    {
        if (localSolution.val < globalSolution.val)
        {
            globalSolution = localSolution;
        }
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Elapsed = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " ms" << std::endl;

    // x1 = 1.4958, x2 = 1.9999, min = -1.52168
    std::cout << "x1 = " << globalSolution.x1
              << ", x2 = " << globalSolution.x2
              << ", min = " << globalSolution.val << "\n";
}
