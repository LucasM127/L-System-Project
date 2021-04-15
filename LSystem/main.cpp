#include "LSystem.hpp"
#include <iostream>
#include "../Parsing/LSFileParser.hpp"//hmm

#include <chrono>
#include <thread>
//Sentence has its own 'local' alphabet
//Each LSystem Production algorithm set has its own 'local' alphabet
//if they speak the same language they can work together, if not they can't

//TODO???
//Fix context array of indices to be a parameter, remove the maxDepth param (as const)
//Decompose recursivity is painful to performance
using namespace LSYSTEM;

std::chrono::duration<int64_t,std::milli> runTime;
bool cont()
{
    return runTime.count()<1000;
}

void longFunction(LSystem &L, VLSentence &A, VLSentence &B)
{
    VLSentence *oldSentence = &A;
    VLSentence *newSentence = &B;
    for(int i = 0; i < 100; ++i)//15 iterations
    {
        L.iterate(*oldSentence,*newSentence, cont);
        oldSentence->clear();
        std::swap(oldSentence,newSentence);
    }
}

int main()
{//Fix the MakeFiles now
    LSFile lsfile;
    lsfile.loadFile("../LSFiles/rowoftrees.ls");
    LSData lsd;
    lsd.productions = 
    {
        "A(x) => BA(x)"
    };
    lsd.decompositions = 
    {
        "A(x) : x > 0 => A(x-1)A(x-1)"
    };
    
    LSystem L(lsfile.lsData());
srand(time(NULL));
    VLSentence axiom(lsfile.axiom());//"A()");//"F");
    VLSentence A,B;
    A = axiom;
//THIS WORKS FINE
//NOW TO DO THIS IN THE INTERPRETER FUNCTION TOO
    std::thread th(longFunction, std::ref(L), std::ref(A), std::ref(B));
    runTime = std::chrono::milliseconds(0);
    auto start = std::chrono::high_resolution_clock::now();
    while(cont())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto duration = std::chrono::high_resolution_clock::now() - start;
        runTime = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    }
        
    th.join();

    return 0;
};
