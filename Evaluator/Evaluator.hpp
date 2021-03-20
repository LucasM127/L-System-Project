#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <string>
#include <stack>
//#include "Numbers.hpp"
#include "RPNToken.hpp"
#include <array>

namespace EVAL
{

class Loader;

class Evaluator
{
    friend class Loader;
public:
    Evaluator(const std::string &exp, bool _isConst) : expression(exp), isConst(_isConst) {}
    virtual ~Evaluator(){}
    virtual float evaluate(float *v) = 0;
    const std::string expression;
    const bool isConst;
protected:
    static void add(float*, uint&);
    static void subtract(float*, uint&);
    static void multiply(float*, uint&);
    static void divide(float*, uint&);
    static void raiseByExponent(float*, uint&);
    static void sin(float*, uint&);
    static void cos(float*, uint&);
    static void tan(float*, uint&);
    static void cosecant(float*, uint&);
    static void secant(float*, uint&);
    static void cotangent(float*, uint&);
    static void random(float*, uint&);
    static void negate(float*, uint&);
    static void testIfEqual(float*, uint&);
    static void testIfGreaterThan(float*, uint&);
    static void testIfLessThan(float*, uint&);
    static void testIfGreaterOrEqualThan(float*, uint&);
    static void testIfLessThanOrEqual(float*, uint&);
    static void testAnd(float*, uint& numberStack);
    static void testOr(float*, uint& numberStack);
    static void maxFn(float*, uint&);
/*    static void add(std::stack<float>&);
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
    static void maxFn(std::stack<float>&);*/
};

class ConstEvaluator : public Evaluator
{
public:
    ConstEvaluator(const std::string &exp, float val) : Evaluator(exp, true), m_val(val) {}
    float evaluate(float *v) {return m_val;}
private:
    const float m_val;
};

class SimpleEvaluator : public Evaluator
{
public:
    SimpleEvaluator(const std::string &exp, Number * n) : Evaluator(exp, n->isConst), m_numPtr(n) {}
    ~SimpleEvaluator(){delete m_numPtr;}
    float evaluate(float *v) {return m_numPtr->getVal(v);}
private:
    const Number * m_numPtr;
};

//now we just need to know the max stack size in advance...
class ComplexEvaluator : public Evaluator
{
public:
    ComplexEvaluator(const std::string &exp, const RPNList &rpnList, uint offset)
                    : Evaluator(exp, false), m_rpnList(rpnList), m_offset(offset){}
    ~ComplexEvaluator(){}
    float evaluate(float *v)
    {
        uint top = m_offset;
        for(auto &T : m_rpnList)
        {
            if(T.isOp)
                T.fnPtr(v, top);
//                T.fnPtr(m_numStack);
            else
                v[top++] = T.numPtr->getVal(v);
//                m_numStack.push(T.numPtr->getVal(v));
        }
        float answer = v[m_offset];//m_numStack.top();
        //m_numStack.pop();
        return answer;
    }
private:
    const RPNList m_rpnList;
    const uint m_offset;
};
//stack -> array sped it up a bunch.  Iterators / if else switches / or virtual function calls(?) is the slow point now

}//namespace EVAL

#endif //EVALUATOR_HPP