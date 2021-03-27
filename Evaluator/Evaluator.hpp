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
public:
    Evaluator(const std::string &exp, bool _isConst, RPNList &refList);
    virtual ~Evaluator(){}
    virtual float evaluate(float *v) = 0;
    virtual void update() = 0;
    const std::string expression;
    const bool isConst;
    //protected????
    RPNList m_refList;//unsimplified tree form list use 'globals'
//friend what????
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
};

Evaluator::Evaluator(const std::string &exp, bool _isConst, RPNList &refList)
                     : expression(exp), isConst(_isConst), m_refList(std::move(refList))
{}

//so is a list that simplifies to a 'constant' value
class ConstEvaluator : public Evaluator
{
public:
    ConstEvaluator(const std::string &exp, RPNList &refList, float val) : Evaluator(exp, true, refList), m_val(val) {}
    float evaluate(float *v) {return m_val;}
    void update() override;
private:
    float m_val;
};

//simplifies to a variable (singular)
class SimpleEvaluator : public Evaluator
{
public:
    SimpleEvaluator(const std::string &exp, RPNList &refList, uint i) : Evaluator(exp, false, refList), m_index(i) {}
    ~SimpleEvaluator(){}
    float evaluate(float *v) {return v[m_index];}
    void update() override {}//never holds a global A(x,3+y) 'x' is simpleEvaluator
private:
    const uint m_index;
};

//now we just need to know the max stack size in advance...
class ComplexEvaluator : public Evaluator
{
public:
    ComplexEvaluator(const std::string &exp, const std::vector<RPN> &rpnList, RPNList &refList, uint offset)
                    : Evaluator(exp, false, refList), m_rpnList(rpnList), m_offset(offset){}
    ~ComplexEvaluator(){}
    float evaluate(float *v)
    {
        uint top = m_offset;
        for(auto &T : m_rpnList)
        {
            switch(T.type)
            {
                case RPN::TYPE::OP:
                    T.fnPtr(v,top);
                    break;
                case RPN::TYPE::CONST:
                    v[top++] = T.value;
                    break;
                case RPN::TYPE::VAR:
                    v[top++] = v[T.index];
                    break;
                default:
                    break;
            }
        }

        return v[m_offset];
    }
    void update() override {}
private:
    std::vector<RPN> m_rpnList;
    const uint m_offset;
};
//stack -> array sped it up a bunch.  Iterators / if else switches / or virtual function calls(?) is the slow point now

}//namespace EVAL

#endif //EVALUATOR_HPP