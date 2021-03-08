GCC = g++ -g -Wall
OUTFOLDER = ../Lib
#DEF = -D EVAL_DEBUG
OBJS = EvalOpMap.o EvalLoader.o LibEvalLoader.o

test: $(OUTFOLDER)/EvalLib.a test.o
	$(GCC) test.o $(OUTFOLDER)/EvalLib.a $(OUTFOLDER)/LSParser.a -o test -ldl

$(OUTFOLDER)/EvalLib.a: $(OBJS)
	ar rvs $(OUTFOLDER)/EvalLib.a $(OBJS)

test.o:
	$(GCC) test.cpp -c -I ../Logger/

EvalLoader.o: EvalLoader.cpp EvalLoader.hpp Evaluator.hpp RPNToken.hpp Numbers.hpp ../Containers/VarIndice.hpp
	$(GCC) $(DEF) EvalLoader.cpp -c -I ../Logger/

EvalOpMap.o: EvalOpMap.cpp EvalLoader.hpp Evaluator.hpp RPNToken.hpp Numbers.hpp ../Containers/VarIndice.hpp
	$(GCC) $(DEF) EvalOpMap.cpp -c -I ../Logger/

LibEvalLoader.o: LibEvalLoader.cpp EvalLoader.hpp Evaluator.hpp RPNToken.hpp Numbers.hpp ../Containers/VarIndice.hpp
	$(GCC) LibEvalLoader.cpp -c -I ../Logger/

clean:
	rm *.o test $(OUTFOLDER)/EvalLib.a