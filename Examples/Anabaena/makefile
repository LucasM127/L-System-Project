test: main.o
	g++ main.o ../../Lib/lsystem.a -o test -ldl

main.o: main.cpp
	g++ -g -Wall main.cpp -c

clean:
	rm *.o *.so test