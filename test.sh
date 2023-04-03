
rm -rf ./output
mkdir ./output
rm -rf ./bin
mkdir ./bin

g++ ./src/main.cpp -o ./bin/main

./bin/main
