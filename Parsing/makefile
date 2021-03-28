# $< - first dependency name   $@ - target name
GCC = g++ -g -Wall
OF = ../Obj
LF = ../InternalLib
#DEF = -D EVAL_DEBUG
OBJS = $(OF)/ProductionParser.o $(OF)/LSParseFuncs.o $(OF)/LSFileParser.o $(OF)/RPNTokenize.o $(OF)/ParsingFuncs.o

test: main.o $(LF)/LSParser.a
	g++ -g -Wall main.o $(LF)/LSParser.a $(LF)/LSContainers.a -o test

main.o: main.cpp
	g++ -g -Wall main.cpp -c

$(LF)/LSParser.a: $(OBJS)
	ar rvs $(LF)/LSParser.a $(OBJS)

$(LF)/LSContainers.a:
	make -C ../Containers -f makefile

$(OF)/ProductionParser.o: ProductionParser.cpp ProductionParser.hpp $(OF)/LSParseFuncs.o
	$(GCC) $< -c -o $@

$(OF)/LSParseFuncs.o: LSParseFuncs.cpp LSParseFuncs.hpp ../Containers/LSentence.hpp\
						$(OF)/LSFileParser.o
	$(GCC) $< -c -o $@

$(OF)/LSFileParser.o: LSFileParser.cpp LSFileParser.hpp ../Containers/LSFileData.hpp\
						 ../Containers/ProductionData.hpp
	$(GCC) $< -c -o $@

$(OF)/RPNTokenize.o: RPNTokenize.cpp RPNTokenize.hpp ../Containers/RPNToken.hpp ../Containers/VarIndice.hpp
	$(GCC) $< -c -o $@

$(OF)/ParsingFuncs.o: ParsingFuncs.cpp ParsingFuncs.hpp
	$(GCC) $< -c -o $@

clean:
	rm -f $(OBJS) main.o test $(LF)/LSParser.a