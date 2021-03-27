#include "RPNToken.hpp"

//#include <Logger.hpp>

//RPN Reverse Polish Notation

namespace EVAL
{

const RPNToken::TYPE OP  = RPNToken::TYPE::OP;
//const RPNToken::TYPE VAR = RPNToken::TYPE::VAR;
const RPNToken::TYPE SYM = RPNToken::TYPE::SYMBOL;
const RPNToken::TYPE GLB = RPNToken::TYPE::GLOBAL;

RPNToken::RPNToken(){}//le sigh
RPNToken::RPNToken(char c, TYPE _type)   : type(_type),         token(c) {}
RPNToken::RPNToken(float v)              : type(TYPE::CONST),   value(v) {}
RPNToken::RPNToken(unsigned int i)       : type(TYPE::VAR),     index(i) {}
RPNToken::RPNToken(const RPNList &list)  : type(TYPE::COMPLEX), rpnList(new RPNList(list)) {}//points to the address passed
RPNToken::RPNToken(const RPNList &&list) : type(TYPE::COMPLEX), rpnList(new RPNList(list)) {}

void destroy(RPNList &list)
{
    for(auto &tok : list)
    {
        if(tok.type == RPNToken::TYPE::COMPLEX)
            destroy(*tok.rpnList);
        delete tok.rpnList;
    }
}

void deepCopy(const RPNList &list, RPNList &target)
{
    target.reserve(list.size());
    for(auto &tok : list)
    {
        target.push_back(tok);//just copy it over
        if(tok.type == RPNToken::TYPE::COMPLEX)
        {//the pointer points to the old .. make a new personal list...
            target.back().rpnList = new RPNList();
            deepCopy(*tok.rpnList,*target.back().rpnList);
        }
    }
}

void expand(const RPNList &rpnList, RPNList &rpnListFinal)
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

RPN::RPN(float v)        : type(RPN::TYPE::CONST), value(v) {}
RPN::RPN(unsigned int i) : type(RPN::TYPE::VAR),   index(i) {}
RPN::RPN(opFnPtr p)      : type(RPN::TYPE::OP),    fnPtr(p) {}

}//namespace EVAL