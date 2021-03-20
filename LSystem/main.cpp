#include "LSystem.hpp"
#include "../Containers/OstreamOperators.hpp"
#include "../Parsing/LSParseFuncs.hpp"
#include <iostream>
//Sentence has its own 'local' alphabet
//Each LSystem Production algorithm set has its own 'local' alphabet
//if they speak the same language they can work together, if not they can't

//TODO Fix cut algorithm
//returning the next index to be out of range.
//Never 'tested' it properly anyways
using namespace LSYSTEM;

int main()
{
    //Yay.  This worked nicely... now to do this with the LSystem Monstrosity
    //Load a LSData Structure... and have LSystem Validate it with alphabet and sort it out
    //but the data loaded will be pre-divided already.
    //The Eval loader will worry about expressions as usual.
    //or load it 'implicitly' in the constructor
    LSYSTEM::LSData LD;
    LD.productions =
    {
        "a=>b",
        "c=>a"
    };
    LSYSTEM::LSystem L(LD);
    //Production Datas
    LSYSTEM::VLSentence V("ab(5)cdb(2)");

    std::cout<<V.getLSentence()<<std::endl;
    std::cout<<OSManip::letter<<V.getLSentence()<<std::endl;
//    VLSentence A("ab(3)cdba");
    LSYSTEM::VLSentence B;
    L.iterate(V,B);

    std::cout<<B.getLSentence()<<std::endl;
    std::cout<<OSManip::letter<<B.getLSentence()<<std::endl;

    return 0;
};
