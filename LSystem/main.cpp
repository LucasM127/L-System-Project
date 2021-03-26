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
{
    LSData lsd;
    lsd.productions = 
    {
        "A(x,t):t==0 => A(x*0.3,2)+A(x*(0.3*(1-0.3))^0.5,1)--A(x*(0.3*(1-0.3))^0.5,1)+A(x*(1-0.3),0)",
        "A(x,t):t>0=>A(x,t-1)"
    };
    LSystem L(lsd);

    VLSentence axiom("A(5,0)");
    VLSentence A,B;
    VLSentence *oldSentence = &axiom;
    VLSentence *newSentence = &A;
//see the time difference for 13 iterations
    //while (true)
    {
        A = axiom;
        oldSentence = &A;
        newSentence = &B;
        auto start = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < 13; ++i)//15 iterations
        {
            L.iterate(*oldSentence, *newSentence);
            oldSentence->clear();
            std::swap(oldSentence,newSentence);
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
