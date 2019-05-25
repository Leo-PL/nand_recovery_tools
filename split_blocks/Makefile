CXXFLAGS += -O2 -Werror -Wall -Wextra -Wpedantic

all: split_blocks
split_blocks: splitblocks.o
	g++ -o split_blocks splitblocks.o

clean: 
	rm *.o split_blocks
