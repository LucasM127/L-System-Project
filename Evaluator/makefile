GCC = g++ -g -Wall
OUTFOLDER = ../Lib
#DEF = -D EVAL_DEBUG
OBJS =  LibEvalLoader.o ShuntYardAlgorithm.o EvalOpMap.o Tokenize.o EvalLoader.o RPNToken.o

test: $(OUTFOLDER)/EvalLib.a test.o
	$(GCC) test.o $(OBJS) $(OUTFOLDER)/LSParser.a -o test -ldl

$(OUTFOLDER)/EvalLib.a: $(OBJS)
	ar rvs $(OUTFOLDER)/EvalLib.a $(OBJS)

test.o: test.cpp LibEvalLoader.o
	$(GCC) test.cpp -c

LibEvalLoader.o: LibEvalLoader.hpp LibEvalLoader.cpp EvalLoader.o
	$(GCC) LibEvalLoader.cpp -c

ShuntYardAlgorithm.o: ShuntYardAlgorithm.cpp EvalLoader.o
	$(GCC) ShuntYardAlgorithm.cpp -c

EvalOpMap.o: EvalOpMap.cpp EvalLoader.o
	$(GCC) $(DEF) EvalOpMap.cpp -c

Tokenize.o: Tokenize.cpp EvalLoader.o
	$(GCC) Tokenize.cpp -c

EvalLoader.o: EvalLoader.cpp EvalLoader.hpp Evaluator.hpp ../Containers/VarIndice.hpp RPNToken.o
	$(GCC) $(DEF) EvalLoader.cpp -c

RPNToken.o: RPNToken.hpp RPNToken.cpp Numbers.hpp
	$(GCC) RPNToken.cpp -c

clean:
	rm *.o test $(OUTFOLDER)/EvalLib.a