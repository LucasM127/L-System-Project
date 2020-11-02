#include "Evaluator.hpp"
#include <iostream>
using namespace LSYSTEM;
//test the evaluator... see if i can get this to work right...

int main()
{
    EvalLoader EL;
    
    VarIndiceMapping map;
    //try some expressions
    //3x + 4b;

    VarIndice X(0,0);
    VarIndice B(1,0);
    VarIndice C(1,1);

    Array2D A(2,2);

    map['x'] = X;
    map['b'] = B;
    map['c'] = C;

    A(X) = 1.f;
    A(B) = 2.f;
    A(C) = 3.f;

    Evaluator &E = EL.add("3x+4b", map, A.depth);
    Evaluator &E2 = EL.add("x+b+c", map, A.depth);
    //don't forget to GENERATE at the end.
    EL.generate();

    std::cout<<"E evaluates to "<< E(A.vals) <<"\n";
    std::cout<<"E2 evaluates to "<< E2(A.vals) <<"\n";

    A(X) = 2.f;
    A(B) = 1.f;

    std::cout<<"E evaluates to "<< E(A.vals) <<"\n";
    std::cout<<"E2 evaluates to "<< E2(A.vals) <<"\n";

    return 0;
}