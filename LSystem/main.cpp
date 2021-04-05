#include "LSystem.hpp"
#include <iostream>


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
    LSData lsd;
    lsd.productions = 
    {
        "F=>FF",
        "X=>F[+X][-X]+FX"
    };
    
    LSystem L(lsd);
srand(time(NULL));
    VLSentence axiom("F");
    VLSentence A,B;
    VLSentence *oldSentence = &axiom;
    VLSentence *newSentence = &A;
    std::cout<<*oldSentence<<"\n";
//see the time difference for 13 iterations
    //while (true)
    {
        A = axiom;
        oldSentence = &A;
        newSentence = &B;
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < 20; ++i)//15 iterations
        {
            L.iterate(*oldSentence, *newSentence);
            oldSentence->clear();
            std::swap(oldSentence,newSentence);
            //std::cout<<*oldSentence<<"\n";
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = end - start;
        std::cout<<"Duration "<<duration.count()<<"\n";
        
        A.clear();
        B.clear();
    }

    //where is the time getting taken up by?

    return 0;
};
