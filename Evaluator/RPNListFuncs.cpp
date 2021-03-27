#include "RPNListFuncs.hpp"
#include "../Logger/Logger.hpp"

namespace EVAL
{

#ifdef EVAL_DEBUG
void printStack(std::stack<char> &opStack)
{
    LOG_S("opStack is: ");
    {
        std::stack<char> tempStack;
        while(opStack.size())
        {
            if(opStack.top() < 30)
                LOG_S("op",int(opStack.top())," ");
            else
                LOG_S(opStack.top()," ");
            tempStack.push(opStack.top());
            opStack.pop();
        }
        while(tempStack.size())
        {
            opStack.push(tempStack.top());
            tempStack.pop();
        }
    }
    LOG_S("\n");
}

void printList(const RPNList &rpnList)
{
    for(auto &T : rpnList)
    {
        LOG_S(" ");
        if(T.isOp)
        {
            if(T.token < 30)
                LOG_S("op",int(T.token));
            else
                LOG_S(char(T.token));
        }
        else
        { 
            if(T.isConst)
                LOG_S(T.value);
            else if(T.isComplex)
            {
                LOG_S("[");
                printList(*T.rpnList);
                LOG_S("]");
            }
            else
                LOG_S(char(T.token));
        }
    }
}
#else
void printStack(std::stack<char> &opStack){}
void printList(const RPNList &rpnList){}
#endif

void expand(RPNList &rpnList, RPNList &rpnListFinal)
{
    for(std::size_t i = 0; i < rpnList.size(); ++i)
    {
        auto &T = rpnList[i];
        if(T.type == RPNToken::TYPE::COMPLEX)
        {
                expand(*T.rpnList, rpnListFinal);
                delete T.rpnList;
        }
        else
        {
            rpnListFinal.push_back(T);
        }
    }
}

} //namespace EVAL