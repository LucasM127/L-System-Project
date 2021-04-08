# $< - first dependency name   $@ - target name
GCC = g++ -g -Wall
OF = ../Obj
LF = ../InternalLib
#DEF = -D EVAL_DEBUG
OBJS = $(OF)/Alphabet.o $(OF)/RPNToken.o $(OF)/LSentence.o $(OF)/OstreamOperators.o

test: main.o $(LF)/LSContainers.a
	$(GCC) main.o $(OBJS) -o test

main.o: main.cpp
	$(GCC) main.cpp -c

$(LF)/LSContainers.a: $(OBJS) $(LF)
	ar rvs $@ $(OBJS)

$(OF)/OstreamOperators.o: OstreamOperators.cpp OstreamOperators.hpp\
							 ProductionData.hpp Alphabet.hpp LSentence.hpp
	$(GCC) $< -c -o $@

$(OF)/LSentence.o: LSentence.cpp LSentence.hpp $(OF)/Alphabet.o
	$(GCC) $< -c -o $@

$(OF)/RPNToken.o: RPNToken.cpp RPNToken.hpp
	$(GCC) $< -c -o $@

$(OF)/Alphabet.o: Alphabet.cpp Alphabet.hpp $(OF)
	$(GCC) $< -c -o $@

$(OF):
	[ -d $@ ] || mkdir $@

$(LF):
	[ -d $@ ] || mkdir $@

clean:
	rm -f $(OBJS) test main.o $(LF)/LSContainers.a