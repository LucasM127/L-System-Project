#ifndef SIMPLIFIER_HPP
#define SIMPLIFIER_HPP

#include "RPNToken.hpp"

namespace EVAL
{

class Simplifier
{
public:
    void operator()(RPNList &rpnList);
private:
    float m_floatStack[2];
    
    void simplify(RPNList& rpnList);
    void simplifyBinary(RPNList& rpnList);
    void simplifyUnary(RPNList& rpnList);
    void simplifyFunc(RPNList& rpnList);
    void distribute(RPNList &rpnList, int complexIt);
    float m_floatStack[2];//make bigger to support bigger functions with 3+ args?
    #ifdef EVAL_DEBUG
    void testDistribute();
    #endif
};

} //namespace EVAL

#endif //SIMPLIFIER_HPP