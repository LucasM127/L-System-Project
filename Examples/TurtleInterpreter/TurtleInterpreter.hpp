#ifndef TURTLE_INTERPRETER_HPP
#define TURTLE_INTERPRETER_HPP

#include "../../ls.hpp"
#include "Turtle3D.hpp"

#include <unordered_map>
#include <string>
#include <stack>

struct LineTurtleIntData
{
    float turnAngle = 90;
    float pitchAngle = 90;
    float rollAngle = 90;
    glm::vec3 color = glm::vec3(1,1,1);

    glm::vec3 tropismVector = glm::vec3(0,-1,0);
    float tropismAmount = 90;
};

//output 3d vertex data for gl LINES draw calls
class LineTurtleInterpreter : public LSYSTEM::LSInterpreter
{
public:
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color;
    };
public:
    LineTurtleInterpreter(Turtle3D Start = Turtle3D());
    const std::vector<Vertex> &data() const;
    void setStartTurtle(Turtle3D start);
    void setDefaults(const LineTurtleIntData &data);
    const Bounds &getBounds() const;
private:
    void interpret(const LSYSTEM::LModule &&M) override;
    void reset() override;
    std::stack<Turtle3D> m_turtleStack;//no width!
    std::stack<glm::vec3> m_colorStack;
    Turtle3D m_startTurtle;

    std::vector<Vertex> m_data;
    Bounds m_bounds;

    //functions
    void push();
    void pop();
    void move(float d);
    void draw(float d);
    void turn(float angle);
    void pitch(float angle);
    void roll(float angle);
    void applyTropism(float angle, glm::vec3 &tropismVec);
    void specialFunc();
    void setColor(glm::vec3);
//    void setWidth(float w);

    void updateBounds();

    LineTurtleIntData def;//default
};

#endif //TURTLE_INTERPRETER_HPP