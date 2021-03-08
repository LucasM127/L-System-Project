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

//LSentence::LSentence() : abc(*new Alphabet){}

LSentence::LSentence(const Alphabet &abc_) : abc(abc_), m_lastNumParams(0), m_numLetters(0)
//#ifdef DEBUG
, m_paramCtr(0)
//#endif
{}
//need a way to just push back a sequence...
LSentence::LSentence(const Alphabet &a, const std::string &string) : abc(a), m_lastNumParams(0), m_numLetters(0)
//#ifdef DEBUG
, m_paramCtr(0)
//#endif
{
    std::stringstream sstream;
    sstream << string;
    char c;
    float f;
    uint numParams;
    while(sstream >> c)
    {
        push_back(c);
        numParams = abc.at(c);

        if(numParams)
        {
            sstream >> c; //the '(' symbol
            for (uint i = 0; i < numParams; i++)
            {
                sstream >> f;
                push_back(f);
                sstream >> c; //the ',' symbol or the ')' symbol
            }
        }
    }
}

//Doesn't really check the alphabet... !!!!!
//WHY SHOULD WE ???
//NOT SURE ABOUT THIS GUY .. BLEGH
//Why can't we just load it... 
void LSentence::loadSimple(const std::string &string)
{
    for(auto c : string)
    {
        push_back(c);
        for(uint i = 0; i < m_lastNumParams; ++i)
        {
            float f = i;
            push_back(f);
        }
    }
}

L32 &LSentence::operator[](const uint i){return m_sentence[i];}

const L32 &LSentence::operator[](const uint i) const {return m_sentence[i];}

bool LSentence::next(const uint letterIndex, uint &nextIndex) const
{
    nextIndex = letterIndex + m_sentence[letterIndex].numParams + 1;
    return nextIndex < m_sentence.size();
}

uint LSentence::next(const uint i) const
{
    return i + m_sentence[i].numParams + 1;
}

bool LSentence::last(const uint letterIndex, uint &lastIndex) const
{
    lastIndex = letterIndex - m_sentence[letterIndex].lastNumParams - 1;
    return lastIndex < m_sentence.size();
}

void LSentence::push_back(char c)
{
//#ifdef DEBUG
    assert(m_paramCtr == m_lastNumParams);
    m_paramCtr = 0;
//#endif
    uint numParams = abc.at(c);
    m_sentence.emplace_back(c,numParams,m_lastNumParams);
    m_lastNumParams = numParams;
    ++m_numLetters;
}

void LSentence::push_back(float f)
{
//#ifdef DEBUG
    //assert(m_paramCtr < m_lastNumParams);
    ++m_paramCtr;
//#endif
    //can do an assertion for debugging
    m_sentence.emplace_back(f);
}

void LSentence::push_backSimple(char c)
{
    m_sentence.emplace_back(c,0,0);
}

void LSentence::push_back(char c, uint &letterIndex)
{
    letterIndex = m_sentence.size();
    push_back(c);
    for(uint i = 0; i < m_lastNumParams; ++i)
        push_back(0.f);
}

void LSentence::pop_back()
{
    if(m_sentence.size()==0) return;
    for(uint i = 0; i < m_lastNumParams; ++i)
        m_sentence.pop_back();
    m_lastNumParams = m_sentence.back().lastNumParams;
    m_sentence.pop_back();
}

uint LSentence::getLastNumParams() const
{
    return m_lastNumParams;
}

uint LSentence::getNumLetters() const
{
    return m_numLetters;
}

uint LSentence::size() const
{
    return m_sentence.size();
}

void LSentence::clear()
{
    m_sentence.clear();
    m_lastNumParams = 0;
    m_paramCtr = 0;
}

bool LSentence::getLastLetterIndex(uint &i) const
{
    i = m_sentence.size() - m_lastNumParams - 1;
    return i < m_sentence.size();
}

bool LSentence::compatible(const Alphabet &abc_) const
{
    for(auto &pair : abc)
    {
        if(abc_.find(pair.first) == abc_.end())
            return false;
        if(abc_.at(pair.first) != pair.second)
            return false;
    }
    return true;
}

std::ostream &operator<<(std::ostream &os, const LSentence &s)
{
    for(uint i = 0; i < s.m_sentence.size(); ++i)
    {
        os<<s.m_sentence[i].id;
        if(s.m_sentence[i].numParams)
        {
            os<<"("<<s.m_sentence[i+1].value;
            for(uint j = 1; j < s.m_sentence[i].numParams; ++j)
            {
                os<<","<<s.m_sentence[i+j+1].value;
            }
            os<<")";
            i += s.m_sentence[i].numParams;
        }
    }

    return os;
}

} //namespace LSYSTEM