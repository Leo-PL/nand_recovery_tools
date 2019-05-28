CXXFLAGS += -O2 -Werror -Wall -Wextra -Wpedantic

all: join_blocks
join_blocks: joinblocks.o
	g++ -o join_blocks joinblocks.o

clean: 
	rm *.o join_blocks
