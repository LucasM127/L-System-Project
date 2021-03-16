# $< - first dependency name   $@ - target name
GCC = g++ -g -Wall
OF = ../Obj
LF = ../InternalLib
#DEF = -D EVAL_DEBUG
DEF=
OBJS =  $(OF)/LibEvalLoader.o $(OF)/ShuntYardAlgorithm.o $(OF)/EvalOpMap.o\
		 $(OF)/Tokenize.o $(OF)/EvalLoader.o $(OF)/RPNToken.o

test: $(LF)/EvalLib.a $(LF)/LSParser.a test.o
	$(GCC) test.o $(LF)/EvalLib.a $(LF)/LSParser.a -o test -ldl

test.o: test.cpp
	$(GCC) $< -c -o $@

$(LF)/EvalLib.a: $(OBJS)
	ar rvs $@ $(OBJS)

$(LF)/LSParser.a:
	make -C ../Parsing -f makefile

$(OF)/LibEvalLoader.o: LibEvalLoader.cpp LibEvalLoader.hpp EvalLoader.o
	$(GCC) $< -c -o $@

$(OF)/ShuntYardAlgorithm.o: ShuntYardAlgorithm.cpp EvalLoader.o
	$(GCC) $(DEF) $< -c -o $@

$(OF)/EvalOpMap.o: EvalOpMap.cpp EvalLoader.o
	$(GCC) $(DEF) $< -c -o $@

$(OF)/Tokenize.o: Tokenize.cpp EvalLoader.o $(LF)/LSParser.a
	$(GCC) $< -c -o $@

$(OF)/EvalLoader.o: EvalLoader.cpp EvalLoader.hpp Evaluator.hpp\
					 ../Containers/VarIndice.hpp RPNToken.o
	$(GCC) $(DEF) $< -c -o $@

$(OF)/RPNToken.o: RPNToken.cpp RPNToken.hpp Numbers.hpp
	$(GCC) $< -c -o $@

clean:
	rm -f *.o $(OBJS) test $(LF)/EvalLib.a