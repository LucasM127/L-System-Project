#include "EvalLoader.hpp"
#include <iostream>
#include <string>
#include <cmath>

#include <chrono>
//#define VAR false,false
//#define OP  true,false
//#define NUM false,true

int main()
{
    srand(time(NULL));
    
    std::string exp="(x+1)(x-1)";
    std::map<char, VarIndice> varMap =
    {
        {'x',{0,0}}
    };

    uint numVals = 1;
    float x_val = 4.f;
    
    EVAL::RuntimeLoader evR;
    evR.setOffset(numVals);

    evR.init();
    
    EVAL::Evaluator *evalR = evR.load(exp, varMap, 1, "foo");
    evR.generate();

    uint stackSz = evR.getMaxStackSz();
    float *V = new float[numVals + stackSz];
    V[0] = x_val;

    auto start = std::chrono::high_resolution_clock::now();

//    V[0] = rand()%100;
//while(true){
    float n;
    for(uint i = 0; i < 100000; ++i)
    n = evalR->evaluate(V);
//}
//    while(true)
//        evalR->evaluate(V);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end - start;
    std::cout<<n<<" is answer duration "<<duration.count()<<"\n";
    std::cout<<"Max stack size was "<<stackSz<<"\n";
    evR.close();

    return 0;
}

/*
    //5x+4sin(30x)
    RPNList rpnList = 
    {
        RPNToken(5.24f,NUM),//false,true),
        RPNToken('x',VAR),//false,false),
        RPNToken('*',OP),//true,false),
        RPNToken(4.f,NUM),//false,true),
        RPNToken({
            RPNToken(30.f,NUM),//false,true),
            RPNToken('x',VAR),//false,false),
            RPNToken('*',OP)//true,false)
        }),
        RPNToken(char(3),OP),//3.4f,NUM),//true,false),//sin
        RPNToken('*',OP),//true,false),
        RPNToken('+',OP)//true,false),
    };
*/