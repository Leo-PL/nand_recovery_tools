CXXFLAGS += -O2 -Werror -Wall -Wextra -Wpedantic

all: majority_edac
majority_edac: majorityedac.o bitindex.o bitflip.o
	g++ -o majority_edac majorityedac.o bitindex.o bitflip.o

clean: 
	rm *.o majority_edac
