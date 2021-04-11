#include "VLSentence.hpp"

#include "../Parsing/LSParseFuncs.hpp"
#include "../Containers/OstreamOperators.hpp"

namespace LSYSTEM
{

VLSentence::VLSentence() {}

VLSentence::VLSentence(const std::string &string)
{
    //parse(string);
    m_lsentence = loadLSentence(string);
    validate();
}

void VLSentence::operator=(const std::string &string)
{
    m_alphabet.clear();
    m_lsentence.clear();
    //parse(string);
    m_lsentence = loadLSentence(string);
    validate();
}

const L32 &VLSentence::operator[](const unsigned int i) const
{
    return m_lsentence[i];
}

void VLSentence::clear()
{
    m_alphabet.clear();
    m_lsentence.clear();
}

const Alphabet &VLSentence::getAlphabet() const {return m_alphabet;}
const LSentence &VLSentence::getLSentence() const {return m_lsentence;}

void VLSentence::validate()
{
    m_alphabet.clear();
    for(unsigned int i = 0; i < m_lsentence.size(); i = m_lsentence.next(i))
    {
        LModule M(m_lsentence, i);
        if(m_alphabet.find(M.id) != m_alphabet.end() && m_alphabet.at(M.id) != M.numVals)
            throw std::runtime_error("Invalid LSentence");
        m_alphabet[M.id] = M.numVals;
    }
}

} //namespace LSYSTEM

std::ostream &operator<<(std::ostream &os, const LSYSTEM::VLSentence &V)
{
    return os << V.getLSentence();
};

/*
#include "../Parsing/ParsingFuncs.hpp"
//THINKING
void LSYSTEM::VLSentence::parse(const std::string &string)
{
    uint i = -1;
    char c, c_next;
    const std::string errorString = "Invalid syntax. Expected comma separated numeric values between () brackets.";
    std::vector<float> vals;
    while(LSPARSE::next(string, i, c))
    {
        ++i;

        if(c == ' ')
        {
            continue;
        }

        vals.clear();
        
        if(LSPARSE::next(string, i, c_next) && c_next == '(')
        {
            i += 2;//skip the '('
            float number;
            if(!LSPARSE::readNumber(string, i, number))
                throw std::runtime_error(errorString);
            vals.emplace_back(number);
            while(LSPARSE::next(string, i, c_next) && c_next == ',')
            {
                i += 2;//skip the ','
                if(!LSPARSE::readNumber(string, i, number))
                    throw std::runtime_error(errorString);
                vals.emplace_back(number);
            }
            if(!LSPARSE::next(string, i, c_next) || c_next != ')')
                throw std::runtime_error(errorString);
            ++i;
        }
        uint numParams = vals.size();
        m_lsentence.push_back(c, numParams);
        for(auto f : vals)
            m_lsentence.push_back(f);
        if(m_alphabet.find(c) != m_alphabet.end() && m_alphabet.at(c) != numParams)
            throw std::runtime_error("Uneven number of parameters for letter '"+std::string(&c,1)+"'");
        m_alphabet[c] = numParams;
    }
}*/