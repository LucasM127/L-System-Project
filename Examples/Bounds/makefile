GCC = g++ -g -Wall
SFML_DIR = /home/lucas/Headers/SFML-2.5.1
LINK= -L $(SFML_DIR)/lib -lsfml-system -lsfml-window -lsfml-graphics -Wl,-rpath=$(SFML_DIR)/lib
STATIC_LIB = ../../Lib/lsystem.a
OBJS=main.o

test: $(OBJS)
	$(GCC) $(OBJS) $(STATIC_LIB) -o test $(LINK)

main.o: main.cpp
	$(GCC) main.cpp -c -I $(SFML_DIR)/include

clean:
	rm -f *.o test