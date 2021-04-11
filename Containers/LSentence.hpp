#ifndef LSENTENCE_HPP
#define LSENTENCE_HPP

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

struct LModule;

class LSentence
{
public:
    LSentence();
    ~LSentence();
    unsigned int next(const unsigned int i) const;
    unsigned int last(const unsigned int i) const;
    unsigned int back() const;
    void push_back(char c, unsigned int numParams);
    void push_back(float f);
    void push_back(LModule &&module);
    L32 &operator[](const unsigned int i);
    const L32 &operator[](const unsigned int i) const;
    std::size_t size() const;
    void clear();
private:
    LString m_lstring;
    unsigned int m_lastNumParams;
    unsigned int m_paramCtr;//just for debug
};

struct LModule
{
    LModule(char id);
    LModule(char id, float *vals, unsigned int numVals);
    LModule(LSentence &sentence, unsigned int i);
    const char id;
    const float * const vals;
    const unsigned int numVals;
};

} //namespace LSYSTEM

#endif //LSENTENCE_HPP