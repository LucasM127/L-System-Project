//#include "EvalLoader.hpp"
#include "../Containers/RPNToken.hpp"
#include "../Containers/OstreamOperators.hpp"
#include <iostream>
#include <string>
#include <cmath>
#include "ShuntYardAlgorithm.hpp"
#include "../Parsing/RPNTokenize.hpp"

#include <chrono>

void findAndReplace(const EVAL::RPNList &subList, EVAL::RPNList &list)
{
//    EVAL::RPNList::const_iterator it = subList.begin();
    //next...? Tree iterator...
}

int main()
{
    VarIndiceMap varMap = 
    {
        {'x',{0,0}},
        {'y',{1,0}}
    };

    //std::unordered_map<char,fl

    const std::string A = "5x^2+36";
    const std::string B = "2y+36(x^2)y";//try and find x^2 is a common 'substring'
    const std::string C = "x^2";

    EVAL::RPNList TokenAList = EVALPARSE::tokenize(A, varMap, 1, {});
    EVAL::RPNList TokenBList = EVALPARSE::tokenize(B, varMap, 1, {});
    EVAL::RPNList TokenCList = EVALPARSE::tokenize(C, varMap, 1, {});

    std::cout<<TokenAList<<"\n";
    std::cout<<TokenBList<<"\n\n";

    EVAL::ShuntYardAlgorithm SYA;
    EVAL::RPNList Atree = SYA.apply(TokenAList);
    EVAL::RPNList Btree = SYA.apply(TokenBList);

    EVAL::RPNList Afinal, Bfinal;

    std::cout<<Atree<<"\n";
    std::cout<<Btree<<"\n\n";

    EVAL::expand(Atree,Afinal);//calls delete..
    EVAL::expand(Btree,Bfinal);//maybe should have separate
//should be able to find matching 'substrings'
//next is next in the list.
    std::cout<<Afinal<<"\n";
    std::cout<<Bfinal<<"\n";

    return 0;
}

/*
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
    std::string exp="xcz";//"x*(z*(c-z))^0.5";//rowoftrees example algorithm
    EVAL::RPNList tokenizedExp = 
    {
        {(unsigned int)0},//'x' mapped
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

    unsigned int numVals = 1;
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

    unsigned int stackSz = evR.getMaxStackSz();
    float *V = new float[numVals + stackSz];
    V[0] = x_val;

    auto start = std::chrono::high_resolution_clock::now();

//    V[0] = rand()%100;
//while(true){
    float n;
    for(unsigned int i = 0; i < 100000; ++i)
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
    globalMap['c'] = 2.f;//vs 1
    globalMap['z'] = 0.3f;
    evR.update();
    n = evalR->evaluate(V);
    std::cout<<n<<" is answer duration "<<duration.count()<<"\n";
    globalMap['c'] = 1.f;//vs 0.3
    evR.update();
    n = evalR->evaluate(V);
    std::cout<<n<<" is answer duration "<<duration.count()<<"\n";
//2 is answer...
    evR.close();

    delete[] V;

    return 0;
}
*/
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