sources := $(wildcard src/*.cpp)
headers := $(wildcard include/*.hpp)
includes := include/

all: serial.out

run: serial.out
	./serial.out

serial.out: $(sources) $(headers)
	g++ $(sources) -lX11 -I$(includes) -Wall -o serial.out


.PHONY: clean
clean:
	rm -f serial.out