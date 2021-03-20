#ifndef VLSENTENCE_HPP
#define VLSENTENCE_HPP

#include "../Containers/LSentence.hpp"
#include "../Containers/Alphabet.hpp"

namespace LSYSTEM
{

//Validated LSentence
class VLSentence
{
    friend class LSystem;
    friend class LSReinterpreter;
public:
    VLSentence();
    VLSentence(const std::string &string);
    void operator=(const std::string &string);
    const L32 &operator[](const uint i) const;
    void clear();
    const Alphabet &getAlphabet() const;
    const LSentence &getLSentence() const;
private:
    //void parse(const std::string &string);
    void validate();
    Alphabet m_alphabet;
    LSentence m_lsentence;
};

} //namespace LSYSTEM

#endif //VLSENTENCE_HPP