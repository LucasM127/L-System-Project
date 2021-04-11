#include <iostream>
#include "../../ls.hpp"

//Example from
//pdf "Modeling plant development with L-systems" pg 5
//"L-system modeling the development of a vegetative segment ofAnabaena"
//http://algorithmicbotany.org/papers/modeling-plant-development-with-l-systems.pdf

//[][ ]
class CellTurtle : public LSYSTEM::LSInterpreter
{
public:
    std::string &getString() {return m_string;}
    void reset() override
    {
        m_string.clear();
    }
    void interpret(const LSYSTEM::LModule &&M) override
    {
        if(M.id == 'W')
        {
            assert(M.numVals == 1);
        }
        if(M.id == 'L')
        {
            assert(M.numVals == 0);
            m_string.append("[ ]");
        }
        if(M.id == 'S')
        {
            assert(M.numVals == 0);
            m_string.append("[]");
        }
    }
private:
    std::string m_string;
};

int main(int argc, char **v)
{
    unsigned int n = 10;
    unsigned int terminalWidth = 80;

    if(argc == 2)
    {
        //uint temp;
        try{
        terminalWidth = std::stoul(v[1]);
        }
        catch(std::invalid_argument&)
        {
            terminalWidth = 80;
        }
    }

    LSYSTEM::LSData lsData;
    lsData.productions = 
    {
        "W(a) => W(a+1)",
        "W(l) < L > W(r) : l >= r => SW(0)L",
        "W(l) < L > W(r) : l < r => LW(0)S",
        "S => L"
    };

    std::vector<std::string> S;
    
    try
    {
    LSYSTEM::LSystem ls(lsData);
    CellTurtle C;
    std::vector<LSYSTEM::VLSentence> A;
    A.resize(n);
    A[0] = "W(0)LW(1)";

    ls.interpret(A[0],C);
    S.emplace_back(std::move(C.getString()));
    for(unsigned int i = 1; i < n; ++i)
    {
        ls.iterate(A[i-1],A[i]);
        ls.interpret(A[i],C);
        S.emplace_back(std::move(C.getString()));
    }

    }
    catch(std::exception &e)
    {
        std::cout<<e.what()<<"\n";
        return 0;
    }

    for(auto &string : S)
    {
        unsigned int offset = (terminalWidth - string.size()) / 2;
        if(string.size() > terminalWidth) break;
        for(unsigned int i = 0; i < offset; ++i)
            std::cout<<" ";
        std::cout<<string<<"\n";
    }

    return 0;    
}