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
        "A(a,b)<A(x,y)>A(c,d)=>A(a+x,b+y)B(c+x,d+y)"
    };
    
    LSystem L(lsd);
srand(time(NULL));
    VLSentence axiom("A(1,2)A(3,4)A(5,6)");
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
        for(int i = 0; i < 1; ++i)//15 iterations
        {
            L.iterate(*oldSentence, *newSentence);
            oldSentence->clear();
            std::swap(oldSentence,newSentence);
            std::cout<<*oldSentence<<"\n";
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
