#include "RPNToken.hpp"

//#include <Logger.hpp>

//RPN Reverse Polish Notation

namespace EVAL
{

const bool OP = true;
const bool VAR = false;

//override copy constructor???
void deepCopy(const RPNList &list, RPNList &target)
{
    target.reserve(list.size());
    for(auto &tok : list)
    {
        if(tok.type == RPNToken::TYPE::COMPLEX)
        {
            target.emplace_back(*tok.rpnList);//calls new
            deepCopy(*tok.rpnList,*target.back().rpnList);
        }
        else
            target.push_back(tok);//just copy it over...
    }
}

RPNToken::RPNToken(){}//le sigh
RPNToken::RPNToken(char c, bool isOp) : type(isOp ? TYPE::OP : TYPE::VAR), token(c) {}
RPNToken::RPNToken(float v) : type(TYPE::CONST), value(v) {}
RPNToken::RPNToken(float *v) : type(TYPE::GLOBAL), global(v) {}
RPNToken::RPNToken(const RPNList &list) : type(TYPE::COMPLEX), rpnList(new RPNList(list)) {}
RPNToken::RPNToken(const RPNList &&list) : type(TYPE::COMPLEX), rpnList(new RPNList(list)) {}

}//namespace EVAL