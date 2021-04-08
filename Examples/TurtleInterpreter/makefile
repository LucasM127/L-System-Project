GLEW = /home/lucas/Headers/glew-2.1.0
GLM=/home/lucas/Headers/glm/
IMGUI=./IMGUI/
SFML_DIR = /home/lucas/Headers/SFML-2.5.1
LIBS=-lsfml-system -lsfml-window -lsfml-graphics -lGLEW -lGLU -lGL
LINK= -L $(SFML_DIR)/lib -L$(GLEW)/lib $(LIBS) -Wl,-rpath=$(SFML_DIR)/lib -Wl,-rpath=$(GLEW)/lib
OBJS=main.o GLShaders.o TurtleInterpreter.o SFMLOrthoCamera.o SFMLSphericalCamera.o AppViewer.o

test: $(OBJS)
	g++ -g -Wall $(OBJS) ../../Lib/lsystem.a imguiSFML.a -o test $(LINK)

main.o: main.cpp
	g++ -g -Wall main.cpp -c -I $(SFML_DIR)/include -I $(GLM) -I $(GLEW)/include $(INCIMGUI)

AppViewer.o: AppViewer.cpp AppViewer.hpp
	g++ -g -Wall $< -c -o $@ -I $(GLEW)/include -I $(SFML_DIR)/include -I $(GLM) -I $(IMGUI)

GLShaders.o: GLShaders.cpp GLShaders.hpp
	g++ -g -Wall $< -c -o $@ -I $(GLEW)/include

SFMLOrthoCamera.o: SFMLOrthoCamera.cpp SFMLOrthoCamera.hpp
	g++ -g -Wall $< -c -o $@ -I $(GLM) -I $(SFML_DIR)/include -I $(GLEW)/include

SFMLSphericalCamera.o: SFMLSphericalCamera.cpp SFMLSphericalCamera.hpp
	g++ -g -Wall $< -c -o $@ -I $(GLM) -I $(SFML_DIR)/include -I $(GLEW)/include

TurtleInterpreter.o: TurtleInterpreter.cpp TurtleInterpreter.hpp
	g++ -g -Wall $< -c -o $@ -I $(GLM)

clean:
	rm -f *.o test