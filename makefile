GCC = g++ -g -Wall
GLEWDIR = /home/lucas/Headers/glew-2.1.0
SFMLDIR = /home/lucas/Headers/SFML-2.5.1
GLMDIR = /home/lucas/Headers/glm
LIBS=-lsfml-system -lsfml-window -lsfml-graphics -lGLEW -lGLU -lGL -ldl
OBJS = main.o LSystem.o ParametricProduction.o Production.o LSLoader.o Evaluator.o OpenGLTurtle.o SFMLCameraController.o
LINK= $(LIBS) -L $(SFMLDIR)/lib -L$(GLEWDIR)/lib -Wl,-rpath=$(SFMLDIR)/lib -Wl,-rpath=$(GLEWDIR)/lib

test: $(OBJS)
	$(GCC) $(OBJS) -o test $(LINK)

main.o : main.cpp
	$(GCC) main.cpp -c -I $(SFMLDIR)/include -I $(GLMDIR) -I $(GLEWDIR)/include

OpenGLTurtle.o : OpenGLTurtle.cpp OpenGLTurtle.hpp LSInterpreter.hpp Turtle3D.hpp
	$(GCC) OpenGLTurtle.cpp -c -I $(GLMDIR) -I $(GLEWDIR)/include

SFMLCameraController.o : SFMLCameraController.cpp SFMLCameraController.hpp
	$(GCC) SFMLCameraController.cpp -c -I $(SFMLDIR)/include -I $(GLMDIR)

LSystem.o : LSystem.cpp LSystem.hpp ParametricProduction.o
	$(GCC) LSystem.cpp -c

ParametricProduction.o : ParametricProduction.cpp ParametricProduction.hpp Production.o
	$(GCC) ParametricProduction.cpp -c

Production.o : Production.cpp Production.hpp LSentence.hpp Containers.hpp
	$(GCC) Production.cpp -c

LSLoader.o : LSLoader.cpp LSLoader.hpp Containers.hpp
	$(GCC) LSLoader.cpp -c

Evaluator.o : Evaluator.cpp Evaluator.hpp
	$(GCC) Evaluator.cpp -c

clean:
	rm *.o test