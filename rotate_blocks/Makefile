CXXFLAGS += -O2 -Werror -Wall -Wextra -Wpedantic

all: rotate_blocks
rotate_blocks: rotateblocks.o blockmarker.o
	g++ -o rotate_blocks blockmarker.o rotateblocks.o

clean: 
	rm *.o rotate_blocks
