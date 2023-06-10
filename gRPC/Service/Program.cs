using Microsoft.AspNetCore.Server.Kestrel.Core;
using Service.Services;

var builder = WebApplication.CreateBuilder();

builder.WebHost.ConfigureKestrel(options =>
{
    // Setup a HTTP/2 endpoint without TLS. Required since running on MacOS.
    options.ListenLocalhost(
        args.Length != 1 ? 5550 : int.Parse(args[0]),
        o => o.Protocols = HttpProtocols.Http2);
});

builder.Services.AddGrpc();

var app = builder.Build();

app.MapGrpcService<CalculatorService>();
app.Run();
