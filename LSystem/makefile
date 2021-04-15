# $< - first dependency name   $@ - target name
GCC = g++ -g -Wall
OF = ../Obj
LF = ../InternalLib
OBJS = $(OF)/VLSentence.o $(OF)/Production.o $(OF)/ParametricProduction.o\
		 $(OF)/ProductChooser.o $(OF)/Product.o $(OF)/LSystem.o $(OF)/LSInterpreter.o
STATIC_LIBS = $(LF)/LSystem.a $(LF)/EvalLib.a $(LF)/LSParser.a $(LF)/LSContainers.a

test:  main.o $(STATIC_LIBS)
	$(GCC) main.o $(STATIC_LIBS) -o test -lpthread

main.o: main.cpp
	$(GCC) -Wall main.cpp -c

$(LF)/EvalLib.a:
	make -C ../Evaluator -f makefile

$(LF)/LSParser.a:
	make -C ../Parsing -f makefile

$(LF)/LSContainers.a:
	make -C ../Containers -f makefile

$(LF)/LSystem.a: $(OBJS)
	ar rvs $@ $(OBJS)

$(OF)/LSInterpreter.o: LSInterpreter.cpp LSInterpreter.hpp $(OF)/LSystem.o
	$(GCC) $< -c -o $@

$(OF)/LSystem.o: LSystem.cpp LSystem.hpp $(OF)/ParametricProduction.o $(OF)/ProductionParser.o
	$(GCC) $< -c -o $@

$(OF)/ProductChooser.o: ProductChooser.cpp ProductChooser.hpp $(OF)/Production.o
	$(GCC) $< -c -o $@

$(OF)/Product.o: Product.cpp Product.hpp
	$(GCC) $< -c -o $@

$(OF)/ParametricProduction.o: ParametricProduction.cpp ParametricProduction.hpp\
								 $(OF)/Production.o $(LF)/EvalLib.a
	$(GCC) $< -c -o $@

$(OF)/Production.o: Production.cpp Production.hpp $(OF)/VLSentence.o
	$(GCC) $< -c -o $@

$(OF)/VLSentence.o: VLSentence.cpp VLSentence.hpp $(LF)/LSContainers.a
	$(GCC) $< -c -o $@

clean:
	rm -f *.o $(OBJS) test $(LF)/LSystem.a
