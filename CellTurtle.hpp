#include "Turtle.hpp"
#include <cassert>

//terminal is 80 wide
class CellTurtle : public TURTLE
{
public:
    void draw() override
    {
        int lbuffersz = (80 - m_string.size()) / 2;
        for(int i = 0; i < lbuffersz; ++i) std::cout<<' ';
        std::cout<<m_string<<"\n";
    }
    void init() override
    {
        m_string.clear();
    }
    void feed(const char c, float * v, int & numVals) override
    {
        if(c == 'W')
        {
            assert(numVals == 1);
            //draw the wall |
            if(v[0] == 0) m_string.push_back('.');
            else if(v[0] == 1) m_string.push_back('|');
            else if(v[0] >= 2) m_string.append("||");
        }
        if(c == 'L') //m_string.append("___");
        {
            assert(numVals == 1);
            m_string.push_back('_');
            m_string.append(std::to_string(int(v[0])));
            m_string.push_back('_');
        }
        if(c == 'S') //m_string.push_back('-');
        {
            assert(numVals == 1);
            m_string.append(std::to_string(int(v[0])));
        }
    }
private:
    std::string m_string;
};