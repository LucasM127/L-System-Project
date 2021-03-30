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
    lsd.globalMap = 
    {
        {'c', 0.3f},
        {'z', 1.0f}
    };
    lsd.productions = 
    {
        "!(x) => !(x+0.1)",
        "G(x) => G(x*1.1)",
        "A => G(1)[+(25)!(0.1)XA][-(25)!(0.1)XA]",
        "X => X : 2",
        "X => A% : 1"
        //"A(x,t):t==0 => A(x*c,2)+A(x*(c*(z-c))^0.5,1)--A(x*(c*(z-c))^0.5,1)+A(x*(1-c),0)",
        //"A(x,t):t>0=>A(x,t-1)"
    };
    
    LSystem L(lsd);
srand(time(NULL));
    VLSentence axiom("A");
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
        for(int i = 0; i < 3; ++i)//15 iterations
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
