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

uint LSentence::next(const uint i) const
{
    return i + m_lstring[i].numParams + 1;
}

uint LSentence::last(const uint i) const
{
    return i - m_lstring[i].lastNumParams - 1;
}

uint LSentence::back() const
{
    return m_lstring.size() - m_lastNumParams - 1;
}

//pass by r reference?
void LSentence::push_back(char c, uint numParams)
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
    ++m_paramCtr;
//#endif
    m_lstring.emplace_back(f);
}

L32 &LSentence::operator[](const uint i){return m_lstring[i];}

const L32 &LSentence::operator[](const uint i) const {return m_lstring[i];}

uint LSentence::size() const
{
    return m_lstring.size();
}

void LSentence::clear()
{
    m_lstring.clear();
    m_lastNumParams = 0;
    m_paramCtr = 0;
}

bool compatible(const Alphabet &a, const Alphabet &b)
{
    for(auto &pair : a)
    {
        if(b.find(pair.first) == b.end())
            continue;
        if(b.at(pair.first) != pair.second)
            return false;
    }
    for(auto &pair : b)
    {
        if(a.find(pair.first) == a.end())
            continue;
        if(a.at(pair.first) != pair.second)
            return false;
    }
    return true;
}

//REALLY THINK SHOULD PUT IN CONTAINERS
void combine(const Alphabet &a, Alphabet &b)
{
    for(auto &pair : a)
        b[pair.first] = pair.second;
}

VLSentence::VLSentence() {}

VLSentence::VLSentence(const std::string &string)
{
    parse(string);
}

void VLSentence::operator=(const std::string &string)
{
    m_alphabet.clear();
    m_lsentence.clear();
    parse(string);
}

const L32 &VLSentence::operator[](const uint i) const
{
    return m_lsentence[i];
}

//only thing IS, I do not know if it 'is' a letter
void VLSentence::setLetter(const uint i, char c)
{
    if(i >= m_lsentence.size()) return;
    m_lsentence[i].id = c;
    isValid = false;
}

//or if it 'is' a value
void VLSentence::setParam(const uint i, const uint paramNum, float f)
{
    if(i >= m_lsentence.size()) return;
    if(paramNum > m_lsentence[i].numParams ) return;//throw?
    m_lsentence[i].value = f;
}

std::ostream &operator<<(std::ostream &os, const VLSentence &l)
{
    for(uint i = 0; i < l.m_lsentence.size(); ++i)
    {
        os<<l.m_lsentence[i].id;
        if(l.m_lsentence[i].numParams)
        {
            os<<"("<<l.m_lsentence[i+1].value;
            for(uint j = 1; j < l.m_lsentence[i].numParams; ++j)
            {
                os<<","<<l.m_lsentence[i+j+1].value;
            }
            os<<")";
            i += l.m_lsentence[i].numParams;
        }
    }

    return os;
}

LModule::LModule(){}
LModule::LModule(LSentence &sentence, uint i) : id(sentence[i].id), vals((float*)&sentence[i+1]), numVals(sentence[i].numParams)
{}

} //namespace LSYSTEM
//if I put in namespace LSYSTEM won't work :?
//Parsing Function
//friend function so can put in Parsing?
#include "../Parsing/ParsingFuncs.hpp"
void LSYSTEM::VLSentence::parse(const std::string &string)
{
    uint i = -1;
    char c, c_next;
    const std::string errorString = "Invalid syntax. Expected comma separated numeric values between brackets.";
    std::vector<float> vals;
    while(LSPARSE::next(string, i, c))
    {
        if(c == ' ')
        {
            ++i;
            continue;
        }

        vals.clear();
        
        if(LSPARSE::next(string, i+1, c_next) && c_next == '(')
        {
            i += 3;
            float number;
            if(!LSPARSE::readNumber(string, i, number))
                throw std::runtime_error(errorString);
            vals.emplace_back(number);
            while(LSPARSE::next(string, i, c_next) && c_next == ',')
            {
                i += 2;
                if(!LSPARSE::readNumber(string, i, number))
                    throw std::runtime_error(errorString);
                vals.emplace_back(number);
            }
            if(!LSPARSE::next(string, i, c_next))
                throw std::runtime_error(errorString);
            else if(c_next != ')')
                throw std::runtime_error(errorString);
        }
        uint numParams = vals.size();
        m_lsentence.push_back(c, numParams);
        for(auto f : vals)
            m_lsentence.push_back(f);
        if(m_alphabet.find(c) != m_alphabet.end() && m_alphabet.at(c) != numParams)
            throw std::runtime_error("Uneven number of parameters for letter '"+std::string(&c,1)+"'");
        m_alphabet[c] = numParams;
        ++i;
    }
}