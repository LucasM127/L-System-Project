GCC = g++ -g -Wall
OUTFOLDER = ../Lib
DEF = -D LOG_DEBUG_STDOUT

test: test.o
	$(GCC) test.o -o test

test.o: test.cpp Logger.hpp
	$(GCC) $(DEF) test.cpp -c

clean:
	rm *.o test