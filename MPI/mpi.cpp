#include <float.h>
#include <iostream>
#include <cmath>
#include <chrono>
#include <mpi.h>

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

void calculateSolution(int start, int end, Solution &solution)
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

                if (val < solution.val)
                {
                    solution.x1 = x1;
                    solution.x2 = x2;
                    solution.val = val;
                }
            }
        }
    }
}

// Custom MPI_MINLOC reduction operation
void minLocReduce(void *in, void *inout, int *len, MPI_Datatype *datatype)
{
    Solution *inSolution = reinterpret_cast<Solution *>(in);
    Solution *inoutSolution = reinterpret_cast<Solution *>(inout);

    if (inSolution->val < inoutSolution->val)
    {
        inoutSolution->val = inSolution->val;
        inoutSolution->x1 = inSolution->x1;
        inoutSolution->x2 = inSolution->x2;
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int numProcesses, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    Solution localSolution;
    localSolution.val = DBL_MAX;

    std::chrono::steady_clock::time_point begin;
    if (rank == 0)
    {
        begin = std::chrono::steady_clock::now();
    }

    int iterationsPerProcess = MaxIterX1 / numProcesses;
    int extraIterations = MaxIterX1 % numProcesses;
    int start = rank * iterationsPerProcess + iterationsPerProcess + (rank < extraIterations ? 1 : 0);
    int end = rank * iterationsPerProcess + std::min(rank, extraIterations);

    calculateSolution(start, end, localSolution);

    // Create custom MPI datatype for Solution struct
    MPI_Datatype solutionType;
    int blockLengths[3] = {1, 1, 1};
    MPI_Aint displacements[3];
    MPI_Datatype types[3] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    MPI_Aint startAddress, address;

    MPI_Get_address(&localSolution, &startAddress);
    MPI_Get_address(&localSolution.x1, &address);
    displacements[0] = address - startAddress;
    MPI_Get_address(&localSolution.x2, &address);
    displacements[1] = address - startAddress;
    MPI_Get_address(&localSolution.val, &address);
    displacements[2] = address - startAddress;

    MPI_Type_create_struct(3, blockLengths, displacements, types, &solutionType);
    MPI_Type_commit(&solutionType);

    // Define the custom MPI operation for MINLOC reduction
    MPI_Op minLocOp;
    MPI_Op_create(minLocReduce, 1, &minLocOp);

    Solution globalSolution;
    globalSolution.val = DBL_MAX;

    MPI_Reduce(&localSolution, &globalSolution, 1, solutionType, minLocOp, 0, MPI_COMM_WORLD);

    MPI_Type_free(&solutionType);
    MPI_Op_free(&minLocOp);

    if (rank == 0)
    {
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Elapsed = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " ms" << std::endl;

        // x1 = 1.4958, x2 = 1.9999, min = -1.52168
        std::cout << "x1 = " << globalSolution.x1
                  << ", x2 = " << globalSolution.x2
                  << ", min = " << globalSolution.val << "\n";
    }

    MPI_Finalize();
}
