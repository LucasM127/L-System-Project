test: main.o
	g++ main.o ../../Lib/lsystem.a -o test

main.o: main.cpp
	g++ -g -Wall main.cpp -c

clean:
	rm *.o test