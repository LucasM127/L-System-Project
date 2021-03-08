#ifndef LSINTERPRETER_HPP
#define LSINTERPRETER_HPP

#include "LSystem.hpp"

//draw needs a canvas and data (ie brush strokes)
//interpret, convert data (LSentence) -> other data
//T &getData(){return T;}

//Interpret and or reinterpret an lsentence using any homomorphisms that may apply
//someway to assert that interpret/reinterpret won't mess up ls.alphabet
template <class T>
class LSInterpreter
{
public:
    LSInterpreter(LSYSTEM::LSystem &ls_) : ls(ls_) {}
    virtual ~LSInterpreter(){}
    virtual void reset(){}
    void interpret(LSYSTEM::LSentence &sentence);
    const T &data() const {return m_data;}
protected:
    virtual void interpret(const LSYSTEM::LSentence &sentence, const uint i){}//convert homomorphisms//module (singular)
    virtual void reinterpret(LSYSTEM::LSentence &sentence, const uint i){}//modify module (singular)//better way to just pass the one module?
    LSYSTEM::LSystem &ls;//for homorphism production mapping
    T m_data;
};

template <class T>
class LSInterpreterSimple
{
public:
    LSInterpreterSimple(){}
    virtual ~LSInterpreterSimple(){}
    virtual void reset(){}
    void interpret(LSYSTEM::LSentence &sentence);
    const T &data() const {return m_data;}
protected:
    virtual void interpret(const LSYSTEM::LSentence &sentence, const uint i){}//convert homomorphisms//module (singular)
    T m_data;
};

//reinterpret main sentence after homomorphisms applied...
//other option, reinterpret while homomorphisms applied (?) or before
//Usually, query modules don't have homomorphisms applicable so is a non-issue
template <class T>
void LSInterpreter<T>::interpret(LSYSTEM::LSentence &sentence)
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
//            reinterpret(sentence, i, tempSentence, j);//perhaps???
        }
        reinterpret(sentence, i);
    }
}

template <class T>
void LSInterpreterSimple<T>::interpret(LSYSTEM::LSentence &sentence)
{
    reset();

    for(uint i = 0; i < sentence.size(); i=sentence.next(i))
    {
        interpret(sentence, i);
    }
}

#endif // LSINTERPRETER_HPP

//homomorphism are ephemeral... such is the way of life
//come and go and whither the wind blow... OH NO!