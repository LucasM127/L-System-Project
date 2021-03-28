#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <string>
#include <map>
#include "../Containers/RPNToken.hpp"

namespace EVAL
{

class Loader;

//call update with varMap and maxDepth.
class Evaluator
{
public:
    Evaluator(const std::string &exp, bool _isConst, bool global, RPNList &&refList);
    virtual ~Evaluator(){}
    virtual float evaluate(float *v) = 0;
    void update(const std::map<char, float*> &globalMap);
    virtual void updateLocal() = 0;
    virtual uint maxStackSz();
    const std::string expression;
    const bool isConst;
    const bool hasGlobal;
    //protected????
    RPNList m_refList;//unsimplified tree form list use 'globals'
    RPNList m_tempList;//expanded simplified refList
//friend what????  //Who owns?
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
    uint m_index;
};

//now we just need to know the max stack size in advance...
class ComplexEvaluator : public Evaluator
{
public:
    ComplexEvaluator(const std::string &exp, RPNList &&refList, bool global, uint offset);
    float evaluate(float *v);
    void updateLocal() override;
    uint maxStackSz() override;
private:
    std::vector<RPN> m_rpnList;
    const uint m_offset;
};

}//namespace EVAL

#endif //EVALUATOR_HPP