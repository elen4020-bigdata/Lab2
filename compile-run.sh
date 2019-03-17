#!/bin/bash

echo Compiling Excecutables.

g++ matrixTransposeNaive.cpp -O3 -o matrixTransposeNaive.exe

g++ -fopenmp -lgomp openMP.cpp -O2 -o openMP.exe
g++ -fopenmp -lgomp openMPBlock.cpp -O2 -o openMPBlock.exe

g++ -pthread PThreadsDiagonal.cpp -o PThreadsDiagonal.exe
g++ -pthread PThreadsBlock.cpp -o PThreadsBlock.exe
echo Done.
echo Running...
./matrixTransposeNaive.exe
./openMP.exe
./openMPBlock.exe
./PThreadsDiagonal.exe
./PThreadsBlock.exe

echo Done.
