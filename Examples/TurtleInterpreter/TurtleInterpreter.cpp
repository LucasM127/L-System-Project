#include "TurtleInterpreter.hpp"

LineTurtleInterpreter::LineTurtleInterpreter(Turtle3D start) : m_startTurtle(start)
{
    reset();
}

void LineTurtleInterpreter::setStartTurtle(Turtle3D start)
{
    m_startTurtle = start;
    reset();
}

const std::vector<LineTurtleInterpreter::Vertex> &LineTurtleInterpreter::data() const
{
    return m_data;
}

const Bounds &LineTurtleInterpreter::getBounds() const
{
    return m_bounds;
}

void LineTurtleInterpreter::setDefaults(const LineTurtleIntData &data)
{
    def = data;
}

void LineTurtleInterpreter::reset()
{
    m_data.clear();
    while(!m_turtleStack.empty())
        m_turtleStack.pop();
    while(!m_colorStack.empty())
        m_colorStack.pop();
    m_turtleStack.push(m_startTurtle);
    m_colorStack.push(def.color);

    m_bounds.max_x = std::numeric_limits<float>::min();
    m_bounds.min_x = std::numeric_limits<float>::max();
    m_bounds.max_y = std::numeric_limits<float>::min();
    m_bounds.min_y = std::numeric_limits<float>::max();
    m_bounds.max_z = std::numeric_limits<float>::min();
    m_bounds.min_z = std::numeric_limits<float>::max();

    updateBounds();
}

void LineTurtleInterpreter::interpret(const LSYSTEM::LModule &&M)
{
    switch (M.id)
    {
    case 'F':
        if(M.numVals) draw(M.vals[0]);
        else draw(1.f);
        break;
    case 'f':
        if(M.numVals) move(M.vals[0]);
        else move(1.f);
        break;
    case '+':
        if(M.numVals) turn(-M.vals[0]);
        else turn(-def.turnAngle);
        break;
    case '-':
        if(M.numVals) turn(M.vals[0]);
        else turn(def.turnAngle);
        break;

        //3D Commands for the next 4...
    case '&':
        if(M.numVals) pitch(-M.vals[0]);
        else pitch(-def.pitchAngle);
        break;
    case '^':
        if(M.numVals) pitch(M.vals[0]);
        else pitch(def.pitchAngle);
        break;
    case '\\'://CCW
        if(M.numVals) roll(-M.vals[0]);
        else roll(-def.rollAngle);
        break;
    case '/'://CW
        if(M.numVals) roll(M.vals[0]);
        else roll(def.rollAngle);
        break;
    case '$':
        specialFunc();
        break;
    case '[':
        push();
        break;
    case ']':
        pop();
        break;
    case '~':
        if(M.numVals == 3) setColor( glm::vec3(M.vals[0], M.vals[1], M.vals[2]) );
        else if(M.numVals) setColor( glm::vec3(M.vals[0], M.vals[0], M.vals[0]) );
        else setColor(def.color);
        break;
/*    case '!':
        if(M.numVals) setWidth(M.vals[0]);
        else setWidth(m_defaultWidth);
        break;*/
    case 'T':
    {
        float tropismAngle = def.tropismAmount;
        if(M.numVals > 0) tropismAngle = M.vals[0];
        glm::vec3 tropismVector = def.tropismVector;
        applyTropism(tropismAngle, tropismVector);
    }
        break;
    default:
        break;
    }
}

void LineTurtleInterpreter::push()
{
    m_turtleStack.push(m_turtleStack.top());
    m_colorStack.push(m_colorStack.top());
}

void LineTurtleInterpreter::pop()
{
    m_turtleStack.pop();
    m_colorStack.pop();
}

void LineTurtleInterpreter::move(float d)
{
    m_turtleStack.top().advance(d);
}

void LineTurtleInterpreter::draw(float d)
{
    m_data.push_back({m_turtleStack.top().pos, m_colorStack.top()});
    m_turtleStack.top().advance(d);
    m_data.push_back({m_turtleStack.top().pos, m_colorStack.top()});
    updateBounds();
}

void LineTurtleInterpreter::turn(float angle)
{
    m_turtleStack.top().turn(angle);
}

void LineTurtleInterpreter::pitch(float angle)
{
    m_turtleStack.top().pitch(angle);
}

void LineTurtleInterpreter::roll(float angle)
{
    m_turtleStack.top().roll(angle);
}

//confirm this
void LineTurtleInterpreter::applyTropism(float amount, glm::vec3 &tropismVec)
{
    Turtle3D &T = m_turtleStack.top();
    //glm::normalize(tropismVec);
    glm::vec3 cross = glm::cross(T.dir, tropismVec);
    
    float L = glm::length(cross);
    if(L == 0) return;//totally parallel, if epsilon off is ok, as will rotate by a very tiny amount
    float angle = amount * L;//cross.length();
    cross = glm::normalize(cross);
    //rotate me this angle...
    T.rotate(angle, cross);
    //T.rotate(amount, cross);
}

//roll turtle so up vector is (0,1,0)
void LineTurtleInterpreter::specialFunc()
{
    //rolls the turtle so that its left direction vector is brought to a horizontal plane
    Turtle3D &T = m_turtleStack.top();
    glm::vec3 V(0,-1,0);//direction opposite gravity
    glm::vec3 VxH = glm::cross(V, T.dir);
    glm::vec3 L = glm::normalize(VxH);
    T.up = glm::cross(T.dir, L);
}

void LineTurtleInterpreter::setColor(glm::vec3 color)
{
    m_colorStack.top() = color;
}

void LineTurtleInterpreter::updateBounds()
{
    glm::vec3 &pos = m_turtleStack.top().pos;
    if(pos.x > m_bounds.max_x) m_bounds.max_x = pos.x;
    if(pos.x < m_bounds.min_x) m_bounds.min_x = pos.x;
    if(pos.y > m_bounds.max_y) m_bounds.max_y = pos.y;
    if(pos.y < m_bounds.min_y) m_bounds.min_y = pos.y;
    if(pos.z > m_bounds.max_z) m_bounds.max_z = pos.z;
    if(pos.z < m_bounds.min_z) m_bounds.min_z = pos.z;
}

/*
void LineTurtleInterpreter::setWidth(float w)
{
    //stretch x and y (orthoganol to direction)
    m_transformStack.top()[0][0] = w;
    m_transformStack.top()[1][1] = w;
}*/