GCC = g++ -g -Wall
OUTFOLDER = ../Lib
#DEF = -D EVAL_DEBUG
OBJS = VLSentence.o Production.o ParametricProduction.o LSystem.o LSInterpreter.o
STATIC_LIBS = $(OUTFOLDER)/LSystem.a $(OUTFOLDER)/EvalLib.a $(OUTFOLDER)/LSParser.a $(OUTFOLDER)/LSContainers.a

test:  main.o $(STATIC_LIBS)
	$(GCC) main.o $(STATIC_LIBS) -o test -ldl

main.o: main.cpp
	$(GCC) -Wall main.cpp -c

$(OUTFOLDER)/LSystem.a: $(OBJS)
	ar rvs $(OUTFOLDER)/LSystem.a $(OBJS)

LSInterpreter.o: LSInterpreter.cpp LSInterpreter.hpp LSystem.o
	$(GCC) LSInterpreter.cpp -c

LSystem.o: LSystem.cpp LSystem.hpp ParametricProduction.o
	$(GCC) -DLOG_DEBUG_STDOUT LSystem.cpp -c

ParametricProduction.o: ParametricProduction.hpp ParametricProduction.cpp Production.o $(OUTFOLDER)/EvalLib.a
	$(GCC) ParametricProduction.cpp -c

Production.o: Production.cpp Production.hpp VLSentence.o
	$(GCC) Production.cpp -c

VLSentence.o: VLSentence.hpp VLSentence.cpp ../Lib/LSContainers.a
	$(GCC) -D DEBUG VLSentence.cpp -c

clean:
	rm *.o test $(OUTFOLDER)/LSystem.a
