sources := $(wildcard src/*.cpp)
cuda_files := $(wildcard src/*.cu)
headers := $(wildcard include/*.h*)
includes := include/

all: serial.out OMP.out cuda.out

serial.out: $(sources) $(headers)
	g++ $(sources) -lX11 -I$(includes) -Wall -o serial.out

OMP.out: $(sources) $(headers)
	g++ $(sources) -lX11 -I$(includes) -Wall -o OMP.out -fopenmp

cuda.out: $(sources) $(headers) $(cuda_files)
	nvcc src/graphics.cpp src/gui.cpp src/input.cpp src/main.cpp src/timing.cpp $(cuda_files) -lX11 -I$(includes) -o cuda.out -std=c++11 $(OPT) --threads 0 -rdc=true -m64

.PHONY: bench
bench: $(sources) $(headers)
	g++ $(sources) -lX11 -I$(includes) -Wall -o serial.out -DBENCHMARK
	g++ $(sources) -lX11 -I$(includes) -Wall -o OMP.out -fopenmp -DBENCHMARK
	nvcc src/graphics.cpp src/gui.cpp src/input.cpp src/main.cpp src/timing.cpp $(cuda_files) -lX11 -I$(includes) -o cuda.out -std=c++11 $(OPT) --threads 0 -rdc=true -m64 -DBENCHMARK

.PHONY: clean
clean:
	rm -f serial.out OMP.out cuda.out

.PHONY: move
move: serial.out OMP.out cuda.out
	scp ./serial.out ./OMP.out ./cuda.out alexz9@ssh.et.byu.edu:~/