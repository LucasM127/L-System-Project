#include "LSentence.hpp"

#include <sstream>
#include <iostream>

//#define DEBUG
//#ifdef DEBUG
#include <cassert>
//#endif

namespace LSYSTEM
{

L32::L32(char c, uint8_t numParams_, uint8_t lastNumParams_)
        : id(c), numParams(numParams_), lastNumParams(lastNumParams_)
{}

L32::L32(float f) : value(f) {}

LSentence::~LSentence(){}

LSentence::LSentence() : m_lastNumParams(0), m_paramCtr(0) {}

unsigned int LSentence::next(const unsigned int i) const
{
    return i + m_lstring[i].numParams + 1;
}

unsigned int LSentence::last(const unsigned int i) const
{
    return i - m_lstring[i].lastNumParams - 1;
}

unsigned int LSentence::back() const
{
    return m_lstring.size() - m_lastNumParams - 1;
}

//pass by r reference?
void LSentence::push_back(char c, unsigned int numParams)
{
//#ifdef DEBUG
    assert(m_paramCtr == m_lastNumParams);
    m_paramCtr = 0;
//#endif
    m_lstring.emplace_back(c, numParams, m_lastNumParams);
    m_lastNumParams = numParams;
}

void LSentence::push_back(float f)
{
//#ifdef DEBUG
    assert(m_paramCtr < m_lastNumParams);
    ++m_paramCtr;
//#endif
    m_lstring.emplace_back(f);
}

void LSentence::push_back(LModule &&module)
{
    push_back(module.id, module.numVals);
    for(unsigned int i = 0; i < module.numVals; ++i)
        push_back(module.vals[i]);
}

L32 &LSentence::operator[](const unsigned int i){return m_lstring[i];}

const L32 &LSentence::operator[](const unsigned int i) const {return m_lstring[i];}

std::size_t LSentence::size() const
{
    return m_lstring.size();
}

void LSentence::clear()
{
    m_lstring.clear();
    m_lastNumParams = 0;
    m_paramCtr = 0;
}

LModule::LModule(LSentence &sentence, unsigned int i) : id(sentence[i].id), vals(sentence[i].numParams > 0 ? reinterpret_cast<float*>(&sentence[i+1]) : nullptr), numVals(sentence[i].numParams)
{}

LModule::LModule(char id_) : id(id_), vals(nullptr), numVals(0)
{}

LModule::LModule(char id_, float *vals_, unsigned int numVals_) : id(id_), vals(vals_), numVals(numVals_)
{}

} //namespace LSYSTEM