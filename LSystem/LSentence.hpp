#ifndef LSENTENCE_HPP
#define LSENTENCE_HPP

#include "../Containers/Alphabet.hpp"

#include <vector>

#include <string>

#include <ostream>

namespace LSYSTEM
{

union L32
{
    float value;
    //uint32_t testId;
    struct
    {
        uint8_t id;
        uint8_t filler_8;
        uint8_t numParams;
        uint8_t lastNumParams;
    };

    L32(char c, uint8_t numParams_, uint8_t lastNumParams_);
    L32(float f);
};

typedef std::vector<L32> LString;

bool compatible(const Alphabet &a, const Alphabet &b);
//b = a + b
//for use iff compatible
void combine(const Alphabet &a, Alphabet &b);

//For Internal use of LSystem
class LSentence
{
    friend class LSystem;
    friend class VLSentence;
public:
    LSentence();
    ~LSentence();
    uint next(const uint i) const;
    uint last(const uint i) const;
    uint back() const;
    void push_back(char c, uint numParams);
    void push_back(float f);
    L32 &operator[](const uint i);
    const L32 &operator[](const uint i) const;
    uint size() const;
    void clear();
private:
    LString m_lstring;
    uint m_lastNumParams;
    uint m_paramCtr;//just for debug
};

//Should I put in Containers?  Same qualities ostream<< and parsing to load...
//Validated LSentence
//External of LSystem.  Can input user defined axiom string, validates
class VLSentence
{
    friend class LSystem;
    friend class LSReinterpreter;
    friend std::ostream &operator<<(std::ostream &os, const VLSentence &vlsentence);
public:
    VLSentence();
    VLSentence(const std::string &string);
    void operator=(const std::string &string);
    const L32 &operator[](const uint i) const;
    void setLetter(const uint i, char c);
    void setParam(const uint i, const uint paramNum, float f);
private:
    bool isValid;//
    void parse(const std::string &string);
    Alphabet m_alphabet;
    LSentence m_lsentence;
};

struct LModule
{
    LModule();
    LModule(LSentence &sentence, uint i);
    char id;
    float * vals;
    uint numVals;
};

} //namespace LSYSTEM

#endif //LSENTENCE_HPP