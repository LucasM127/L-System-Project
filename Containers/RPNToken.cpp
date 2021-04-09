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
//this should work

bool RPNToken::operator==(const RPNToken &other) const
{
    if(other.type != this->type)
        return false;
    switch(type)
    {
        case TYPE::COMPLEX:
            return *rpnList == *other.rpnList;
        case TYPE::VAR:
            return index == other.index;
        case TYPE::CONST:
            return value == other.value;//floats
        case TYPE::SYMBOL:
        case TYPE::GLOBAL:
        case TYPE::OP:
            return token == other.token;
        default:
            return false;
    }
    return false;
}

void destroy(RPNList &list)
{
    for(auto &tok : list)
    {
        if(tok.type == RPNToken::TYPE::COMPLEX)
        {
            destroy(*tok.rpnList);
            delete tok.rpnList;
        }
    }
}

void deepCopy(const RPNList &list, RPNList &target)
{
    target.reserve(list.size());
    for(auto &tok : list)
    {
        if(tok.type == RPNToken::TYPE::COMPLEX)
        {
            target.emplace_back(RPNList());//this is fine
            deepCopy(*tok.rpnList,*target.back().rpnList);
        }
        else
            target.emplace_back(tok);
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