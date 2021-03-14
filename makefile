STATIC_LIBS=Lib/LSystem.a Lib/EvalLib.a Lib/LSParser.a Lib/LSContainers.a

all: $(STATIC_LIBS)
	ar rvs lsystem.a $(STATIC_LIBS)

Lib/LSystem.a: Lib/EvalLib.a
	make -C LSystem -f makefile

Lib/EvalLib.a: Lib/LSParser.a
	make -C Evaluator -f makefile

Lib/LSParser.a: Lib/LSContainers.a
	make -C Parsing -f makefile

Lib/LSContainers.a:
	make -C Containers -f makefile

clean:
	make clean -C LSystem -f makefile
	make clean -C Evaluator -f makefile
	make clean -C Parsing -f makefile
	rm *.o test

.PHONY: clean all