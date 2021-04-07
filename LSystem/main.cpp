#include "LSystem.hpp"
#include <iostream>
#include "../Parsing/LSFileParser.hpp"//hmm

#include <chrono>
//Sentence has its own 'local' alphabet
//Each LSystem Production algorithm set has its own 'local' alphabet
//if they speak the same language they can work together, if not they can't

//TODO???
//Fix context array of indices to be a parameter, remove the maxDepth param (as const)
//Decompose recursivity is painful to performance
using namespace LSYSTEM;

int main()
{//Fix the MakeFiles now
    //LSFile lsfile;
    //lsfile.loadFile("../Examples/TurtleInterpreter/PineConeIdea.ls");
    LSData lsd;
    lsd.productions = 
    {
        "A(x) => BA(x)"
    };
    lsd.decompositions = 
    {
        "A(x) : x > 0 => A(x-1)A(x-1)"
    };
    
    LSystem L(lsd);
srand(time(NULL));
    VLSentence axiom("A(2)");//"F");
    VLSentence A,B;
    VLSentence *oldSentence = &axiom;
    VLSentence *newSentence = &A;
    //std::cout<<*oldSentence<<"\n";
//see the time difference for 13 iterations
    //while (true)
    {
        A = axiom;
        oldSentence = &A;
        newSentence = &B;
        auto start = std::chrono::high_resolution_clock::now();
        //for(int i = 0; i < 3; ++i)//15 iterations
        {
//            std::cout<<A<<"\n";
            //L.iterate(*oldSentence,*newSentence);
            //oldSentence->clear();
            //std::swap(oldSentence,newSentence);
            //std::cout<<*oldSentence<<"\n";
            L.iterate(A,3);
            std::cout<<A<<"\n";
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = end - start;
        std::cout<<"Duration "<<duration.count()<<"\n";
        //std::cout<<*oldSentence<<"\n";
        A.clear();
        B.clear();
    }

    //where is the time getting taken up by?

    return 0;
};
