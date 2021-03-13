GCC = g++ -g -Wall
OUTFOLDER = ../Lib
#DEF = -D EVAL_DEBUG
OBJS = Alphabet.o LSentence.o OstreamOperators.o

test: test.o $(OUTFOLDER)/LSContainers.a
	$(GCC) test.o $(OUTFOLDER)/LSContainers.a -o test

test.o: test.cpp
	$(GCC) test.cpp -c


$(OUTFOLDER)/LSContainers.a: $(OBJS)
	ar rvs $(OUTFOLDER)/LSContainers.a $(OBJS)

OstreamOperators.o: OstreamOperators.hpp OstreamOperators.cpp ProductionData.hpp Alphabet.hpp LSentence.hpp
	$(GCC) OstreamOperators.cpp -c

LSentence.o: LSentence.cpp LSentence.hpp Alphabet.o
	$(GCC) LSentence.cpp -c

Alphabet.o: Alphabet.cpp Alphabet.hpp
	$(GCC) Alphabet.cpp -c

clean:
	rm *.o test $(OUTFOLDER)/LSContainers.a