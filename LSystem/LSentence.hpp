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

class LSentence
{
    friend std::ostream &operator<<(std::ostream &os, const LSentence &s);
public:
    ~LSentence();
    //LSentence();
    LSentence(const Alphabet &a);
    LSentence(const Alphabet &a, const std::string &string);
    void loadSimple(const std::string &string);
    //remove???
    bool next(const uint letterIndex, uint &nextIndex) const;
    uint next(const uint i) const;
    bool last(const uint letterIndex, uint &lastIndex) const;
    void push_back(char c, uint &letterIndex);
    void push_back(char c);
    void push_back(float f);
    void pop_back();
    void push_backSimple(char c);
    L32 &operator[](const uint i);
    const L32 &operator[](const uint i) const;
    uint getLastNumParams() const;
    uint size() const;
    uint getNumLetters() const;
    bool getLastLetterIndex(uint &i) const;
    bool compatible(const Alphabet &abc) const;
    void clear();
private:
    const Alphabet &abc;
    std::vector<L32> m_sentence;
    uint m_lastNumParams;
    uint m_numLetters;
//#ifdef DEBUG
    uint m_paramCtr;
//#endif
};

} //namespace LSYSTEM

#endif //LSENTENCE_HPP