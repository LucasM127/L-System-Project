#include "OpenGLTurtle.hpp"

//draw to the bound opengl context buffer
//create a line turtle... also...

void genCrossSection(std::vector<glm::vec3> &v, uint n)
{
    
    for(uint i = 0; i < n; ++i)
    {
        float dTheta = glm::radians(360.f / float(n)) * i;
        float r = 0.5f;
        float x = r * sinf(dTheta);
        float y = 0;
        float z = r * cosf(dTheta);
        
        v.emplace_back(x,y,z);
    }
    //v.emplace_back(0,0,0.5);
    //v.emplace_back(0,0,-0.5);
}

void genTriangles(std::vector<glm::vec3> &branchVertices, std::vector<glm::vec3> & modelVertices, uint crossSectionSize)
{
    uint j_top = branchVertices.size() - 1;//last one
    uint i_top = j_top - crossSectionSize + 1;//first one
    for(uint i = 0; i < crossSectionSize; ++i)
    {
        glm::vec3 &a = branchVertices[j_top - crossSectionSize];
        glm::vec3 &a_ = branchVertices[j_top];
        glm::vec3 &b = branchVertices[i_top - crossSectionSize];
        glm::vec3 &b_ = branchVertices[i_top];

        modelVertices.push_back(a);
        modelVertices.push_back(a_);
        modelVertices.push_back(b_);

        modelVertices.push_back(b_);
        modelVertices.push_back(b);
        modelVertices.push_back(a);

        j_top = i_top;
        ++i_top;

        if(crossSectionSize == 2) break;
    }
}

void addSegment(std::vector<glm::vec3> &crossSectionVertices, std::vector<glm::vec3> &branchVertices, Turtle3D &T, float width)
{
    std::vector<glm::vec3> transformedCrossSectionVertices;
    //create a change of basis matrix from unit direction.... (up = x), (y = dir), (z = cross(up, dir)  x cross y)
    glm::mat3 changeOfBasis(T.up, T.dir, glm::cross(T.up,T.dir));
    
    for(auto &v : crossSectionVertices)
    {
        glm::vec3 v_ = changeOfBasis * v;
        v_ *= width;
        v_ += T.pos;
        transformedCrossSectionVertices.push_back(v_);
    }

    branchVertices.insert(branchVertices.end(), transformedCrossSectionVertices.begin(), transformedCrossSectionVertices.end());
}

TurtleGL::TurtleGL() : m_defaultAngle(90.f), m_defaultWidth(0.2f), m_defaultColor(1.f,1.f,1.f), m_defaultTropismAngle(0), m_tropismVector(0,-1,0)
{
    genCrossSection(m_crossSectionVertices, 6);
    glm::vec3 pos(0.f,0.f,0.f);
    glm::vec3 dir(0.f,1.f,0.f);
    glm::vec3  up(0.f,0.f,1.f);
    m_startTurtle = Turtle3D(pos,dir,up);
    m_turtleStack.push(Turtle3D(pos, dir, up));

    m_vao = 0;
    m_vertexColorBuffer = m_vertexBuffer = 0;
    glGenVertexArrays(1, &m_vao);
    
    glGenBuffers(1,&m_vertexBuffer);
    glGenBuffers(1,&m_vertexColorBuffer);

    //shader???
}

TurtleGL::TurtleGL(Turtle3D startTurtle) : m_defaultAngle(90.f), m_defaultWidth(0.2f), m_defaultColor(1.f,1.f,1.f), m_defaultTropismAngle(0), m_tropismVector(1,0,0)
{
    
    m_startTurtle = startTurtle;
    genCrossSection(m_crossSectionVertices, 3);
    m_turtleStack.push(startTurtle);

    m_vao = 0;
    m_vertexColorBuffer = m_vertexBuffer = 0;
    glGenVertexArrays(1, &m_vao);
    
    glGenBuffers(1,&m_vertexBuffer);
    glGenBuffers(1,&m_vertexColorBuffer);
}


void TurtleGL::reset()
{
    //assert(m_turtleStack.size()==1);
    m_vertices.clear();
    m_vColors.clear();
    m_turtleStack.empty();
    m_turtleStack.push(m_startTurtle);
    //updateGLBuffers();
}

void TurtleGL::push()
{
    m_turtleStack.push( m_turtleStack.top() );
}

void TurtleGL::pop()
{
    m_turtleStack.pop();
}

void TurtleGL::move(float dist)
{
    Turtle3D &T = m_turtleStack.top();
    T.advance(dist);
}

void TurtleGL::draw(float dist)
{
    //or add a defined shape of scale width, and scale depth at the point...
    Turtle3D &T = m_turtleStack.top();
    std::vector<glm::vec3> branchVertices;
    addSegment(m_crossSectionVertices, branchVertices, T, T.scale);
    T.advance(dist);
    addSegment(m_crossSectionVertices, branchVertices, T, T.scale);//m_defaultWidth);
    genTriangles(branchVertices, m_vertices, m_crossSectionVertices.size());

    unsigned int n = 6 * m_crossSectionVertices.size();
    if(m_crossSectionVertices.size()==2) n = 6;
    for(uint i = 0; i < n; ++i)
    {
        m_vColors.emplace_back(m_defaultColor);
    }
}

void TurtleGL::turnLeft(float angle)
{
    Turtle3D &T = m_turtleStack.top();
    T.turn(-angle);
}

void TurtleGL::turnRight(float angle)
{
    Turtle3D &T = m_turtleStack.top();
    T.turn(angle);
}

void TurtleGL::pitchUp(float angle)
{
    Turtle3D &T = m_turtleStack.top();
    T.pitch(angle);
}

void TurtleGL::pitchDown(float angle)
{
    Turtle3D &T = m_turtleStack.top();
    T.pitch(-angle);
}

void TurtleGL::rollCW(float angle)
{
    Turtle3D &T = m_turtleStack.top();
    T.roll(angle);
}

void TurtleGL::rollCCW(float angle)
{
    Turtle3D &T = m_turtleStack.top();
    T.roll(-angle);
}

void TurtleGL::setColor(float r, float g, float b)
{
    m_defaultColor.r = r;
    m_defaultColor.g = g;
    m_defaultColor.b = b;
}

void TurtleGL::setWidth(float w)
{
    Turtle3D &T = m_turtleStack.top();
    T.scale = w;
    //m_defaultWidth = w;
}

void TurtleGL::applyTropism(float amount, glm::vec3 &tropismVec)
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
void TurtleGL::specialFunc()
{
    //rolls the turtle so that its left direction vector is brought to a horizontal plane
    Turtle3D &T = m_turtleStack.top();
    glm::vec3 V(0,-1,0);//direction opposite gravity
    glm::vec3 VxH = glm::cross(V, T.dir);
    glm::vec3 L = glm::normalize(VxH);
    T.up = glm::cross(T.dir, L);

    //change of basis...
    //glm::mat3 changeOfBasis(T.up, T.dir, glm::cross(T.up,T.dir));
}

void TurtleGL::feed(const char c,float *v,int& numVals)//why by ref?
{
    switch (c)
    {
    case 'F':
        if(numVals) draw(v[0]);
        else draw(1.f);
        break;
    case 'f':
        if(numVals) move(v[0]);
        else move(1.f);
        break;
    case '+':
        if(numVals) turnLeft(v[0]);//*PI/180.0f);
        else turnLeft(m_defaultAngle);//*PI/180.0f);
        break;
    case '-':
        if(numVals) turnRight(v[0]);//*PI/180.0f);
        else turnRight(m_defaultAngle);//*PI/180.0f);//or transform before
        break;

        //3D Commands for the next 4...
    case '&':
        if(numVals) pitchDown(v[0]);//*PI/180.0f);
        else pitchDown(m_defaultAngle);//*PI/180.0f);
        break;
    case '^':
        if(numVals) pitchUp(v[0]);//*PI/180.0f);
        else pitchUp(m_defaultAngle);//*PI/180.0f);
        break;
    case '\\':
        if(numVals) rollCCW(v[0]);//*PI/180.0f);
        else rollCCW(m_defaultAngle);//*PI/180.0f);
        break;
    case '/':
        if(numVals) rollCW(v[0]);//*PI/180.0f);
        else rollCW(m_defaultAngle);//*PI/180.0f);
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
        if(numVals == 3) setColor( v[0], v[1], v[2] );
        else if(numVals) setColor( v[0], v[0], v[0] );
        else setColor(1.f,1.f,1.f);//sf::color
        break;
    case '!':
        if(numVals) setWidth(v[0]);
        else setWidth(0.1f);
        break;
    case 'T':
    {
        float tropismAngle = m_defaultTropismAngle;
        if(numVals > 0) tropismAngle = v[0];
        glm::vec3 tropismVector = m_tropismVector;
        applyTropism(tropismAngle, tropismVector);
    }
        break;
    default:
        break;
    }
}

void TurtleGL::updateGLBuffers()
{
//    GLenum error;
    //glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    //glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &m_vertices[0], GL_STATIC_DRAW);

    //glGenBuffers(1, &m_vertexColorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexColorBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_vColors.size() * sizeof(glm::vec3), &m_vColors[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexColorBuffer);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    glBindVertexArray(0);//done with information
}

void TurtleGL::draw()
{
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    glBindVertexArray(0);
}

TurtleGL::~TurtleGL()
{
    if(m_vertexBuffer) glDeleteBuffers(1, &m_vertexBuffer);
    if(m_vertexColorBuffer) glDeleteBuffers(1, &m_vertexColorBuffer);
    if(m_vao) glDeleteVertexArrays(1, &m_vao);
}