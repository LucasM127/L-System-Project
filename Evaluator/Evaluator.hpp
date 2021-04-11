#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <string>
#include <unordered_map>
#include "../Containers/RPNToken.hpp"

namespace EVAL
{

class Loader;

//call update with varMap and maxDepth.
class Evaluator
{
public:
    Evaluator(const std::string &exp, bool _isConst, bool global, RPNList &&refList);
    virtual ~Evaluator();
    virtual float evaluate(float *v) = 0;
    void update(const std::unordered_map<char, float> &globalMap);
    virtual void updateLocal() = 0;
    virtual unsigned int maxStackSz();
    const std::string expression;
    const bool isConst;
    const bool hasGlobal;
    //protected????
    RPNList m_refList;//unsimplified tree form list use 'globals'
    RPNList m_tempList;//expanded simplified refList
//friend what????  //Who owns?
    static void add(float*, unsigned int&);
    static void subtract(float*, unsigned int&);
    static void multiply(float*, unsigned int&);
    static void divide(float*, unsigned int&);
    static void raiseByExponent(float*, unsigned int&);
    static void sin(float*, unsigned int&);
    static void cos(float*, unsigned int&);
    static void tan(float*, unsigned int&);
    static void cosecant(float*, unsigned int&);
    static void secant(float*, unsigned int&);
    static void cotangent(float*, unsigned int&);
    static void random(float*, unsigned int&);
    static void negate(float*, unsigned int&);
    static void testIfEqual(float*, unsigned int&);
    static void testIfGreaterThan(float*, unsigned int&);
    static void testIfLessThan(float*, unsigned int&);
    static void testIfGreaterOrEqualThan(float*, unsigned int&);
    static void testIfLessThanOrEqual(float*, unsigned int&);
    static void testAnd(float*, unsigned int& numberStack);
    static void testOr(float*, unsigned int& numberStack);
    static void maxFn(float*, unsigned int&);
};

//so is a list that simplifies to a 'constant' value
class ConstEvaluator : public Evaluator
{
public:
    ConstEvaluator(const std::string &exp, RPNList &&refList, bool global);
    float evaluate(float *v);
private:
    void updateLocal() override;//globals have been changed... so 'recalculate it out'
    float m_val;
};


//simplifies to a variable (singular)
class SimpleEvaluator : public Evaluator
{
public:
    SimpleEvaluator(const std::string &exp, RPNList &&refList);
    float evaluate(float *v);
private:
    void updateLocal() override;
    unsigned int m_index;
};

//now we just need to know the max stack size in advance...
class ComplexEvaluator : public Evaluator
{
public:
    ComplexEvaluator(const std::string &exp, RPNList &&refList, bool global, unsigned int offset);
    float evaluate(float *v);
    void updateLocal() override;
    unsigned int maxStackSz() override;
private:
    std::vector<RPN> m_rpnList;
    const unsigned int m_offset;
};

}//namespace EVAL

#endif //EVALUATOR_HPP