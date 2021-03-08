GCC = g++ -g -Wall
OUTFOLDER = ../Lib
#DEF = -D EVAL_DEBUG
OBJS = LSentence.o Production.o ParametricProduction.o LSystem.o LSInterpreter.o
#may be better as a 'container' ???
#something to do with these 'libraries' they won't work the same as a whole bunch of object files ???
#getting a collision

test: $(OUTFOLDER)/LSystem.a $(OUTFOLDER)/EvalLib.a $(OUTFOLDER)/LSParser.a main.o
	$(GCC) main.o $(OUTFOLDER)/LSystem.a $(OUTFOLDER)/EvalLib.a $(OUTFOLDER)/LSParser.a -o test -ldl

main.o: main.cpp
	$(GCC) -Wall main.cpp -c

$(OUTFOLDER)/LSystem.a: $(OBJS)
	ar rvs $(OUTFOLDER)/LSystem.a $(OBJS)

LSInterpreter.o: LSInterpreter.cpp LSInterpreter.hpp LSystem.o
	$(GCC) LSInterpreter.cpp -c -I ../Logger/

LSystem.o: LSystem.cpp LSystem.hpp ParametricProduction.o
	$(GCC) -DLOG_DEBUG_STDOUT LSystem.cpp -c -I ../Logger/

ParametricProduction.o: ParametricProduction.hpp ParametricProduction.cpp Production.o $(OUTFOLDER)/EvalLib.a
	$(GCC) ParametricProduction.cpp -c -I ../Logger/

Production.o: Production.cpp Production.hpp LSentence.o
	$(GCC) Production.cpp -c

LSentence.o: LSentence.hpp LSentence.cpp
	$(GCC) -D DEBUG LSentence.cpp -c

clean:
	rm *.o test $(OUTFOLDER)/LSystem.a