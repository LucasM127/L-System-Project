#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <string>
#include <stack>
//#include "Numbers.hpp"
#include "RPNToken.hpp"

namespace EVAL
{

class Loader;

class Evaluator
{
    friend class Loader;
public:
    Evaluator(const std::string &exp, bool _isConst) : expression(exp), isConst(_isConst) {}
    virtual ~Evaluator(){}
    virtual float evaluate(const float *v) = 0;
    const std::string expression;
    const bool isConst;
protected:
    static void add(std::stack<float>&);
    static void subtract(std::stack<float>&);
    static void multiply(std::stack<float>&);
    static void divide(std::stack<float>&);
    static void raiseByExponent(std::stack<float>&);
    static void sin(std::stack<float>&);
    static void cos(std::stack<float>&);
    static void tan(std::stack<float>&);
    static void cosecant(std::stack<float>&);
    static void secant(std::stack<float>&);
    static void cotangent(std::stack<float>&);
    static void random(std::stack<float>&);
    static void negate(std::stack<float>&);
    static void testIfEqual(std::stack<float>&);
    static void testIfGreaterThan(std::stack<float>&);
    static void testIfLessThan(std::stack<float>&);
    static void testIfGreaterOrEqualThan(std::stack<float>&);
    static void testIfLessThanOrEqual(std::stack<float>&);
    static void testAnd(std::stack<float>& numberStack);
    static void testOr(std::stack<float>& numberStack);
};

class ConstEvaluator : public Evaluator
{
public:
    ConstEvaluator(const std::string &exp, float val) : Evaluator(exp, true), m_val(val) {}
    float evaluate(const float *v) {return m_val;}
private:
    const float m_val;
};

class SimpleEvaluator : public Evaluator
{
public:
    SimpleEvaluator(const std::string &exp, Number * n) : Evaluator(exp, n->isConst), m_numPtr(n) {}
    ~SimpleEvaluator(){delete m_numPtr;}
    float evaluate(const float *v) {return m_numPtr->getVal(v);}
private:
    const Number * m_numPtr;
};

class ComplexEvaluator : public Evaluator
{
public:
    ComplexEvaluator(const std::string &exp, const RPNList &rpnList) : Evaluator(exp, false), m_rpnList(rpnList) {}
    float evaluate(const float *v)
    {
        for(auto &T : m_rpnList)
        {
            if(T.isOp)
                T.fnPtr(m_numStack);
            else
                m_numStack.push(T.numPtr->getVal(v));
        }
        float answer = m_numStack.top();
        m_numStack.pop();
        return answer;
    }
private:
    const RPNList m_rpnList;
    std::stack<float> m_numStack;
};

}//namespace EVAL

#endif //EVALUATOR_HPP