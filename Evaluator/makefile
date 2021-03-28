# $< - first dependency name   $@ - target name
GCC = g++ -g -Wall
OF = ../Obj
LF = ../InternalLib
#DEF = -D EVAL_DEBUG
DEF=
OBJS =  $(OF)/LibEvalLoader.o $(OF)/EvalLoader.o $(OF)/ShuntYardAlgorithm.o $(OF)/EvalOpMap.o\
		$(OF)/Evaluator.o $(OF)/Simplify.o $(OF)/RPNListFuncs.o

test: $(LF)/EvalLib.a $(LF)/LSParser.a test.o
	$(GCC) test.o $(LF)/EvalLib.a $(LF)/LSParser.a $(LF)/LSContainers.a -o test -ldl

test.o: test.cpp
	$(GCC) $< -c -o $@

$(LF)/EvalLib.a: $(OBJS)
	ar rvs $@ $(OBJS)

$(LF)/LSParser.a:
	make -C ../Parsing -f makefile

$(LF)/LSContainers.a:
	make -C ../Containers -f makefile

$(OF)/LibEvalLoader.o: LibEvalLoader.cpp LibEvalLoader.hpp $(OF)/EvalLoader.o
	$(GCC) $< -c -o $@

$(OF)/ShuntYardAlgorithm.o: ShuntYardAlgorithm.cpp $(OF)/EvalLoader.o
	$(GCC) $(DEF) $< -c -o $@

$(OF)/Simplify.o: Simplify.cpp Simplify.hpp
	$(GCC) $(DEF) $< -c -o $@

$(OF)/Evaluator.o: Evaluator.cpp Evaluator.hpp
	$(GCC) $(DEF) $< -c -o $@

$(OF)/RPNListFuncs.o: RPNListFuncs.cpp RPNListFuncs.hpp
	$(GCC) $(DEF) $< -c -o $@

$(OF)/EvalOpMap.o: EvalOpMap.cpp $(OF)/EvalLoader.o
	$(GCC) $(DEF) $< -c -o $@

$(OF)/EvalLoader.o: EvalLoader.cpp EvalLoader.hpp Evaluator.hpp\
					 ../Containers/VarIndice.hpp $(OF)/RPNToken.o
	$(GCC) $(DEF) $< -c -o $@

clean:
	rm -f *.o $(OBJS) test $(LF)/EvalLib.a