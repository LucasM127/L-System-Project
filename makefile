LF=InternalLib
STATIC_LIBS=($LF)/LSystem.a ($LF)/EvalLib.a ($LF)/LSParser.a ($LF)/LSContainers.a

all: $(STATIC_LIBS)
	ar rvs Lib/lsystem.a Obj/*.o

($LF)/LSystem.a: ($LF)/EvalLib.a
	make -C LSystem -f makefile

($LF)/EvalLib.a: ($LF)/LSParser.a
	make -C Evaluator -f makefile

($LF)/LSParser.a: ($LF)/LSContainers.a
	make -C Parsing -f makefile

($LF)/LSContainers.a:
	make -C Containers -f makefile

clean:
	make clean -C LSystem -f makefile
	make clean -C Evaluator -f makefile
	make clean -C Parsing -f makefile
	make clean -C Containers -f makefile
	rm -f Lib/lsystem.a

.PHONY: clean all
