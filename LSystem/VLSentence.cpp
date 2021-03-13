#include "VLSentence.hpp"

namespace LSYSTEM
{

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

void VLSentence::clear()
{
    m_alphabet.clear();
    m_lsentence.clear();
}

const Alphabet &VLSentence::getAlphabet() const {return m_alphabet;}
const LSentence &VLSentence::getLSentence() const {return m_lsentence;}

} //namespace LSYSTEM

#include "../Parsing/ParsingFuncs.hpp"

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
}