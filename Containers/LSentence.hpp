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

class LSentence
{
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
    std::size_t size() const;
    void clear();
private:
    LString m_lstring;
    uint m_lastNumParams;
    uint m_paramCtr;//just for debug
};

struct LModule
{
    LModule(char id);
    LModule(char id, float *vals, uint numVals);
    LModule(LSentence &sentence, uint i);
    const char id;
    const float * const vals;
    const uint numVals;
};

} //namespace LSYSTEM

#endif //LSENTENCE_HPP