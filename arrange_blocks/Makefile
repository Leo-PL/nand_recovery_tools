CXXFLAGS += -O2 -Werror -Wall -Wextra -Wpedantic

all: arrange_blocks
arrange_blocks: arrangeblocks.o blockmarker.o
	g++ -o arrange_blocks blockmarker.o arrangeblocks.o

clean: 
	rm *.o arrange_blocks
