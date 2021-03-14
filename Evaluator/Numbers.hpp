#ifndef EVAL_NUMBERS_HPP
#define EVAL_NUMBERS_HPP

#include "../Containers/VarIndice.hpp"

namespace EVAL
{

class Number
{
public:
    Number(bool constant) : isConst(constant) {}
    virtual const float getVal() const = 0;
    virtual const float getVal(const float *v) const = 0;
    virtual ~Number(){}
    const bool isConst;
};

class Constant : public Number
{
public:
    Constant(float n) : Number(true), m_val(n) {}
    ~Constant() {}
    const float getVal() const override {return m_val;}
    const float getVal(const float *v) const override {return m_val;}
private:
    const float m_val;
};

class Global : public Number
{
public:
    Global(float *n) : Number(false), val(n) {}
    ~Global() {}
    const float getVal() const override {return *val;}
    const float getVal(const float *v) const override {return *val;}
private:
    const float *val;
};

class LVariable : public Number
{
public:
    LVariable(const VarIndice &vi, int depth)
        : Number(false), m_index(vi.letterNum * depth + vi.paramNum) {}
    ~LVariable() {}
    const float getVal() const override {return 0.0f;}
    const float getVal(const float *v) const override {return v[m_index];}
private:
    const int m_index;
};

}//namespace EVAL

#endif //EVAL_NUMBERS_HPP