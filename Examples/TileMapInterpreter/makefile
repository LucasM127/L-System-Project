GCC = g++ -g -Wall
SFML_DIR = /home/lucas/Headers/SFML-2.5.1
STATIC_LIBS=../../Lib/lsystem.a
LINK= -L $(SFML_DIR)/lib -lsfml-system -lsfml-window -lsfml-graphics -Wl,-rpath=$(SFML_DIR)/lib
OBJS=GridInterpreter.o Grid.o EnvironmentalGridMap.o main.o

test: $(OBJS)
	$(GCC) $(OBJS) $(STATIC_LIBS) -o test $(LINK)

main.o: main.cpp
	$(GCC) main.cpp -c -I $(SFML_DIR)/include -I ../Logger/

EnvironmentalGridMap.o: EnvironmentalGridMap.cpp EnvironmentalGridMap.hpp GridInterpreter.o
	$(GCC) EnvironmentalGridMap.cpp -c -I $(SFML_DIR)/include

GridInterpreter.o: GridInterpreter.cpp GridInterpreter.hpp
	$(GCC) GridInterpreter.cpp -c -I ../Logger/ -I $(SFML_DIR)/include

Grid.o: Grid.hpp Grid.cpp
	$(GCC) Grid.cpp -c -I $(SFML_DIR)/include

clean:
	rm -f *.o test