GCC = g++ -g -Wall
SFML_DIR = /home/lucas/Headers/SFML-2.5.1
LINK= -L $(SFML_DIR)/lib -lsfml-system -lsfml-window -lsfml-graphics -Wl,-rpath=$(SFML_DIR)/lib
OBJS=main.o

test: $(OBJS)
	$(GCC) $(OBJS) ../../Lib/lsystem.a -o test $(LINK)

main.o: main.cpp
	$(GCC) main.cpp -c -I $(SFML_DIR)/include

clean:
	rm *.o test