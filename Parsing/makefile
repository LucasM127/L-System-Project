GCC = g++ -g -Wall
OUTFOLDER = ../Lib
#DEF = -D EVAL_DEBUG
OBJS = LSFileParser.o ParsingFuncs.o


test: main.o $(OUTFOLDER)/LSParser.a
	g++ -g -Wall main.o $(OUTFOLDER)/LSParser.a $(OUTFOLDER)/OstreamOperators.o -o test

main.o: main.cpp
	g++ -g -Wall main.cpp -c

$(OUTFOLDER)/LSParser.a: $(OBJS)
	ar rvs $(OUTFOLDER)/LSParser.a $(OBJS)

LSFileParser.o: LSFileParser.cpp LSFileParser.hpp
	g++ -g -Wall LSFileParser.cpp -c

ParsingFuncs.o: ParsingFuncs.cpp ParsingFuncs.hpp
	g++ -g -Wall ParsingFuncs.cpp -c

clean:
	rm *.o test $(OUTFOLDER)/LSParser.a