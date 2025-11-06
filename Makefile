sources := $(wildcard src/*.cpp)
headers := $(wildcard include/*.hpp)
includes := include/

all: serial.out OMP.out

.PHONY: run
run: serial.out
	./serial.out

serial.out: $(sources) $(headers)
	g++ $(sources) -lX11 -I$(includes) -Wall -o serial.out

OMP.out: $(sources) $(headers)
	g++ $(sources) -lX11 -I$(includes) -Wall -o OMP.out -fopenmp


.PHONY: clean
clean:
	rm -f serial.out OMP.out

.PHONY: move
move:
	scp ./serial.out ./OMP.out alexz9@ssh.et.byu.edu:~/