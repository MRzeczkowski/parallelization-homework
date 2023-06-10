using Grpc.Net.Client;
using Calculate;
using System.Diagnostics;
using Grpc.Net.Client.Balancer;
using Grpc.Core;
using Grpc.Net.Client.Configuration;

GrpcChannel GetGrpCChannel(int[] ports)
{
    if (ports.Length != 0)
    {
        int numServices = args.Length;

        List<BalancerAddress> addresses = new List<BalancerAddress>(numServices);

        foreach (int port in args.Select(int.Parse))
        {
            addresses.Add(new BalancerAddress("localhost", port));
        }

        var factory = new StaticResolverFactory(addr => addresses);
        var services = new ServiceCollection();
        services.AddSingleton<ResolverFactory>(factory);

        Console.WriteLine($"Will send requests to services on ports: {string.Join(", ", ports)}");

        return GrpcChannel.ForAddress(
                "static://localhost",
                new GrpcChannelOptions
                {
                    Credentials = ChannelCredentials.Insecure,
                    ServiceConfig = new ServiceConfig { LoadBalancingConfigs = { new RoundRobinConfig() } },
                    ServiceProvider = services.BuildServiceProvider()
                });
    }

    return GrpcChannel.ForAddress("http://localhost:5550");
}

int[] ports = args.Select(int.Parse).ToArray();
int portNum = ports.Length;

// By default split the task to 8 gRPC calls, 
// otherwise we'll split the work to separate services.
int numServices = portNum == 0 ? 8 : portNum;

using var channel = GetGrpCChannel(ports);

double p = Math.Pow(10, 4);
double e = 1 / p;
int maxIterX1 = (int)(2 * p);
int maxIterX2 = (int)p;

double minX1 = 0;
double minX2 = 0;
double minVal = double.MaxValue;

int iterationsPerService = maxIterX1 / numServices;
int extraIterations = maxIterX1 % numServices;

var replies = new List<Task<CalculationReply>>(numServices);

var sw = Stopwatch.StartNew();

for (int i = 0; i < numServices; i++)
{
    var client = new Calculator.CalculatorClient(channel);
    int start = i * iterationsPerService + iterationsPerService + (i < extraIterations ? 1 : 0);
    int end = i * iterationsPerService + int.Min(i, extraIterations);

    var reply = client.PerformCalculationAsync(
                  new CalculationRequest
                  {
                      P = p,
                      E = e,
                      MaxIterX2 = maxIterX2,
                      Start = start,
                      End = end
                  }).ResponseAsync;

    replies.Add(reply);
}

await Task.WhenAll(replies);

foreach (var r in replies.Select(r => r.Result))
{
    if (r.Val < minVal)
    {
        minX1 = r.X1;
        minX2 = r.X2;
        minVal = r.Val;
    }
}

sw.Stop();

Console.WriteLine($"Elapsed = {sw.ElapsedMilliseconds} ms");

// x1 = 1.4958, x2 = 1.9999, min = -1.52168
Console.WriteLine($"x1 = {minX1}, x2 = {minX2}, min = {minVal} ");