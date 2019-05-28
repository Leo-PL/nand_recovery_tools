CXXFLAGS += -O2 -Werror -Wall -Wextra -Wpedantic

all: filter_blocks
filter_blocks: filterblocks.o condition.o
	g++ -o filter_blocks filterblocks.o condition.o

clean: 
	rm *.o filter_blocks
