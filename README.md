# HPC_Fluid_Sim
Simple Fluid Simulation designed for my High Performance Computing Class. to experiment with how parallelization can help improve performance of programs.

Just uses X11 library on linux (No windows compatibility, sorry)

### Build
Build with Makefile 
```shell
make
./OMP.out
```

or build with g++
```shell
g++ src/graphics.cpp src/input.cpp src/main.cpp src/particle.cpp src/timing.cpp -lX11 -Iinclude/ -Wall -o OMP.out -fopenmp
./OMP.out
```