#ifndef SHUNT_YARD_ALGORITHM_HPP
#define SHUNT_YARD_ALGORITHM_HPP

#include "RPNToken.hpp"
#include <stack>

namespace EVAL
{

class ShuntYardAlgorithm
{
public:
    RPNList &apply(const RPNList &tokenizedExpression);
private:
    std::stack< RPNList > m_rpnListStack;
    void evaluate(std::stack<char>& opStack);
};

} //namespace EVAL

#endif //SHUNT_YARD_ALGORITHM_HPP