#include "EvalLoader.hpp"
#include "../Containers/RPNToken.hpp"
#include <iostream>
#include <string>
#include <cmath>

#include <chrono>
//#define VAR false,false
//#define OP  true,false
//#define NUM false,true

//WILL HAVE TO UPDATE ... at the end.


int main()
{
    srand(time(NULL));

    
    //constant for whole iteration cycle
    std::unordered_map<char, float> globalMap = 
    {
        {'c', 1.f},
        {'z', 0.3f}
    };
    std::unordered_set<char> globalSet = {'c','z'};

    //changes with every module
    std::map<char, VarIndice> varMap =
    {
        {'x',{0,0}}
    };
//0.45826 YAY
    std::string exp="x*(z*(c-z))^0.5";//rowoftrees example algorithm
    EVAL::RPNList tokenizedExp = 
    {
        {(uint)0},//'x' mapped
        {'*',EVAL::OP},
        {'(',EVAL::SYM},
        {'z',EVAL::GLB},
        {'*',EVAL::OP},
        {'(',EVAL::SYM},
        {'c',EVAL::GLB},
        {'-',EVAL::OP},
        {'z',EVAL::GLB},
        {')',EVAL::SYM},
        {')',EVAL::SYM},
        {'^',EVAL::OP},
        {0.5f}
    };

    uint numVals = 1;
    float x_val = 4.f;
    
    EVAL::RuntimeLoader evR;
    evR.setOffset(numVals);
    evR.setGlobalMap(globalMap);//char float
    evR.init();
    
    //load
    EVAL::Evaluator *evalR = evR.load(exp, varMap, 1, "foo");
//    EVAL::Evaluator *evalR = evR.load(exp, tokenizedExp, "foo");
    evR.generate();

    evR.update();

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

    globalMap['z'] = 0.5f;//vs 0.3
    evR.update();
    n = evalR->evaluate(V);
    std::cout<<n<<" is answer duration "<<duration.count()<<"\n";
    globalMap['z'] = 0.75f;//vs 0.3
    evR.update();
    n = evalR->evaluate(V);
    std::cout<<n<<" is answer duration "<<duration.count()<<"\n";
//2 is answer...
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