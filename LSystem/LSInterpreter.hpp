#ifndef LSINTERPRETER_HPP
#define LSINTERPRETER_HPP

#include "VLSentence.hpp"

#include <cassert>

namespace LSYSTEM
{

//Hook into the homomorphically transformed lsentence
class LSInterpreter
{
public:
    LSInterpreter();
    virtual ~LSInterpreter();
    virtual void reset() = 0;
    virtual void interpret(const LModule &&M) = 0;//(const LSYSTEM::LSentence &sentence, const uint i) = 0;//convert homomorphisms, per module
};

//For use if need to modify an lsentence. (ie, an environmental query)
class LSReinterpreter
{
public:
    LSReinterpreter(const LSYSTEM::Alphabet a);
    const LSYSTEM::Alphabet alphabet;
    void contract(LSYSTEM::VLSentence &vlsentence);//update the lsentence
    virtual void reinterpret(const LSYSTEM::LSentence &sentence, const uint i, LSYSTEM::VLSentence &vlsentence) = 0;
protected:
    void modify(LSYSTEM::VLSentence &vlsentence, const uint i, const LModule &&M);
    void modifyParams(LSYSTEM::VLSentence &vlsentence, const uint i, float * newVals, const uint numVals);
};

} //namespace LSYSTEM

#endif // LSINTERPRETER_HPP

//homomorphism are ephemeral... such is the way of life
//come and go and whither the wind blow... OH NO!