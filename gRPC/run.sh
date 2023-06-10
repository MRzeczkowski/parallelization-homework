#! /bin/sh

ports=(5550 5551 5552 5553 5554 5555 5556 5557)

cd ./Service

for port in "${ports[@]}"
do
    dotnet run port > /dev/null 2>&1 &
done

cd ../Client
dotnet run "${ports[@]}"