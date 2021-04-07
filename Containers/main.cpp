#include "OstreamOperators.hpp"
#include <iostream>
#include <stack>

using namespace EVAL;

class RPNTokenIterator
{
public:
    RPNTokenIterator(RPNList &list_) : i(-1), list(&list_){}
    unsigned int i;
    RPNList *list;
    std::stack<std::pair<unsigned int, RPNList&> > stack;
    bool next(RPNToken &T_next)
    {
        ++i;
        if(i >= list->size())
        {
            if(stack.empty())
                return false;
            list = &stack.top().second;
            i = stack.top().first;
            stack.pop();
            return next(T_next);
        }
        if((*list)[i].type == RPNToken::TYPE::COMPLEX)
        {
            stack.push({i,*list});
            list = (*list)[i].rpnList;
            i = -1;
            return next(T_next);
        }
        T_next = (*list)[i];
        return true;
    }
};

//just 'flatten it' ??
//find the common substrings
//How to replace?

int main()
{
    //5x^2+36
    RPNList A =
    {
        RPNToken(5.f),
        RPNToken({
            RPNToken(0u),
            RPNToken(2.f),
            RPNToken('^',OP)
        })
    };

    RPNList B = 
    {
        RPNToken(0u),
        RPNToken(2.f),
        RPNToken('^',OP)
    };

    RPNTokenIterator IA(A);
    RPNTokenIterator IB(B);
    RPNToken TA,TB;
    IA.next(TA);
    while(IB.next(TB) && IA.next(TA))
    {
        if(TA == TB)
            std::cout<<"Yes ";
        else std::cout<<"No ";
    }
    std::cout<<"\n";

    return 0;
}