#include "LSInterpreter.hpp"

namespace LSYSTEM
{

LSInterpreter::LSInterpreter(){}
LSInterpreter::~LSInterpreter(){}

LSReinterpreter::LSReinterpreter(const LSYSTEM::Alphabet a) : alphabet(a){}

void LSReinterpreter::contract(LSYSTEM::VLSentence &vlsentence)
{
    if(!compatible(alphabet, vlsentence.m_alphabet))
        throw std::runtime_error("Incompatible alphabets");
    combine(alphabet, vlsentence.m_alphabet);
}

//I NEED TO TEST THIS SHIT BADLY
//Ensure that new letter module we are creating matches what we have agreed in contract with the vlsentence
void LSReinterpreter::modify(LSYSTEM::VLSentence &vlsentence, const uint i, const LModule &&M)
{
    assert(alphabet.find(M.id) != alphabet.end());
    assert(alphabet.at(M.id) == M.numVals);

    LSYSTEM::LSentence &lsentence = vlsentence.m_lsentence;

    lsentence[i].id = M.id;
    for(uint j = 1; j <= M.numVals; ++j)
    {
        lsentence[i+j].value = M.vals[j];
    }
}

//Don't need to ensure with contract, modifying params does not change a modules identity
void LSReinterpreter::modifyParams(LSYSTEM::VLSentence &vlsentence, const uint i, float * newVals, const uint numVals)
{
    LSYSTEM::LSentence &lsentence = vlsentence.m_lsentence;

    assert(lsentence[i].numParams >= numVals);

    for(uint j = 1; j <= numVals; ++j)
    {
        lsentence[i+j].value = newVals[j];
    }
}


} //namespace LSYSTEM