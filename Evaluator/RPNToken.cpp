#include "RPNToken.hpp"

//#include <Logger.hpp>

//RPN Reverse Polish Notation

namespace EVAL
{

const bool OP = true;
const bool VAR = false;

RPNToken::RPNToken(){}//le sigh
RPNToken::RPNToken(char c, bool _isOp) : isOp(_isOp), isConst(false), isComplex(false), token(c)
{
    //LOG("Make char '", c);
}
RPNToken::RPNToken(float v) : isOp(false), isConst(true), isComplex(false), value(v)
{
    //LOG("Make float ", v);
}
RPNToken::RPNToken(const RPNList &list) : isOp(false), isConst(false), isComplex(true), rpnList(new RPNList(list)) {}
RPNToken::RPNToken(const RPNList &&list) : isOp(false), isConst(false), isComplex(true), rpnList(new RPNList(list)) {}

}//namespace EVAL