#include "LSInterpreter.hpp"
/*
LSInterpreter::LSInterpreter(LSYSTEM::LSystem &ls_) : ls(ls_)
{}

//reinterpret main sentence after homomorphisms applied...
//other option, reinterpret while homomorphisms applied (?) or before
//Usually, query modules don't have homomorphisms applicable so is a non-issue
void LSInterpreter::interpret(LSYSTEM::LSentence &sentence)
{
    LSYSTEM::LSentence tempSentence(ls.alphabet);

    reset();

    for(uint i = 0; i < sentence.size(); i=sentence.next(i))
    {
        tempSentence.clear();
        ls.applyHomomorphisms(sentence, i, tempSentence);
        
        for(uint j = 0; j < tempSentence.size(); j = tempSentence.next(j))
        {
            interpret(tempSentence, j);
        }
        reinterpret(sentence, i);
    }

    apply();
}
*/