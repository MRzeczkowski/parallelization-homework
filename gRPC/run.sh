#! /bin/sh

ports=(5550 5551 5552 5553 5554 5555 5556 5557)

cd ./Service

dotnet build

for port in "${ports[@]}"
do
    dotnet run --no-build port
done

cd ../Client

dotnet build
dotnet run --no-build "${ports[@]}"