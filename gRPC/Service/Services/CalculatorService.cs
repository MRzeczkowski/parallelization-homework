using Grpc.Core;
using Calculate;

namespace Service.Services;

public class CalculatorService : Calculator.CalculatorBase
{
    public override Task<CalculationReply> PerformCalculation(CalculationRequest request, ServerCallContext context)
    {
        double minX1 = 0;
        double minX2 = 0;
        double minVal = double.MaxValue;

        for (int i = request.Start; i > request.End; i--)
        {
            double x1 = 2.0 - i * request.E;

            for (int j = 0; j < request.MaxIterX2; j++)
            {
                double x2 = 1.0 + j * request.E;

                double c = Math.Pow(x1 - 2.0, 2.0) / 4.0 + Math.Pow(x2 - 1.0, 2.0) / 9.0;

                if (c <= 1.0)
                {
                    double val = Math.Sin(2.0 * x2) + 0.1 * Math.Pow(x1, 2.0) + Math.Cos(x1 * x2);

                    if (val < minVal)
                    {
                        minX1 = x1;
                        minX2 = x2;
                        minVal = val;
                    }
                }
            }
        }

        return Task.FromResult(
            new CalculationReply
            {
                X1 = minX1,
                X2 = minX2,
                Val = minVal
            });
    }
}
