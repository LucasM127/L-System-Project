GCC = g++ -g -Wall
OUTFOLDER = ../Lib
#DEF = -D EVAL_DEBUG
OBJS =

test: $(OUTFOLDER)/OstreamOperators.o test.o
	$(GCC) test.o $(OUTFOLDER)/EvalLib.a -o test -ldl

test.o: $(OUTFOLDER)/OstreamOperators.o test.cpp
	$(GCC) test.cpp -c

$(OUTFOLDER)/OstreamOperators.o: OstreamOperators.hpp OstreamOperators.cpp
	$(GCC) OstreamOperators.cpp -c -o $(OUTFOLDER)/OstreamOperators.o

clean:
	rm $(OUTFOLDER)/OstreamOperators.o test