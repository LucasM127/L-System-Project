#ifndef OPENGL_TURTLE_HPP
#define OPENGL_TURTLE_HPP

#include "LSInterpreter.hpp"//LSYSTEM INTERFACE
#include "Turtle3D.hpp"//Geometric interface
#include <GL/glew.h>

#include <vector>
#include <stack>

//some way to get environmental data from the turtle interpreter... 
//PROCESSING

//Uses a single big vertice array
//Other option, use a single big indiced array, with 'turtle' offsets for each segment, calculated on the fly...
//so when apply T() tropism, just update the matrix of 'bones' and use that in the shaders to know where to draw them ?
//not sure if is faster or not
class TurtleGL : public LSInterpreter
{
public:
    TurtleGL();
    TurtleGL(Turtle3D startTurtle);
    ~TurtleGL();
    void draw();//draws it thinking...
    void reset();
    void feed(const char,float*,int& numVals) override;
    void updateGLBuffers();
    Turtle3D &turtle(){return m_turtleStack.top();}

    void setDefaultAngle(float a) { m_defaultAngle = a; }
    void setDefaultWidth(float w) { m_defaultWidth = w; }
    void setDefaultColor(glm::vec3 color) { m_defaultColor = color; }
    void setDefaultTropism(float t) { m_defaultTropismAngle = t; }
    void setDefaultTropism(glm::vec3 v) { m_tropismVector = glm::normalize(v); }
    void setStartTurtle(Turtle3D T) {m_startTurtle = T;}
private:
    Turtle3D m_startTurtle;
    std::stack<Turtle3D> m_turtleStack;
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_vColors;

    std::vector<glm::vec3> m_crossSectionVertices;

    void push();
    void pop();
    void move(float d);
    void draw(float d);
    void turnRight(float angle);
    void turnLeft(float angle);
    void pitchUp(float angle);
    void pitchDown(float angle);
    void rollCW(float angle);
    void rollCCW(float angle);
    void applyTropism(float angle, glm::vec3 &tropismVec);
    void specialFunc();

    void setColor(float r, float g, float b);
    void setWidth(float w);

    float m_defaultAngle, m_defaultWidth;
    glm::vec3 m_defaultColor;
    float m_defaultTropismAngle;
    glm::vec3 m_tropismVector;//if is global
    

    GLuint m_vao;
    GLuint m_vertexBuffer;
    GLuint m_vertexColorBuffer;
};

#endif //OPENGL_TURTLE_HPP