#ifndef EVAL_RPNTOKEN_SIMPLIFY_HPP
#define EVAL_RPNTOKEN_SIMPLIFY_HPP

#include "RPNToken.hpp"

namespace EVAL
{

class Simplify
{
public:
    void operator()(RPNList &rpnList);
private:
    float m_floatStack[2];//up to 2 args
    
    void simplify(RPNList& rpnList);
    void simplifyBinary(RPNList& rpnList);
    void simplifyUnary(RPNList& rpnList);
    void simplifyFunc(RPNList& rpnList);
    void distribute(RPNList &rpnList, int complexIt);
    
    #ifdef EVAL_DEBUG
    void testDistribute();
    #endif
};

} //namespace EVAL

#endif //EVAL_RPNTOKEN_SIMPLIFY_HPP