//#include "EvalLoader.hpp"
//#include "../Containers/RPNToken.hpp"
//#include "../Containers/OstreamOperators.hpp"
#include <iostream>
#include <string>
//#include <cmath>
//#include "ShuntYardAlgorithm.hpp"
//#include "../Parsing/RPNTokenize.hpp"
#include "EvalLoader.hpp"

#include <chrono>
#include <cassert>


//Usage
//VarMap and global map both let the loader know which token ids are valid and what type they are (variable or global)
//setGlobalMap before loading else won't know to look for those globals
//varMap is set when load each expression, as can be different for each expression what each token means (not global but local)
//setOffset in evalLoader called before loading each expression
//offset = how much space to allocate in stack buffer for the variables to reside in and is used in creation of complex evaluators
//then the buffer is generated with a call to getMaxStackSize so know how big a buffer needed to hold the variables, and stack op calculations
//update is called after globals have been changed, so the complex evaluators can re-simplify for the new constant terms
//generate doesn't really do anything. but if doing it with generating a dynamic lib file generates the file for libevaloader
//same as init
//multiplication is implied
int main()
{
    VarIndiceMap varMap = 
    {
        {'x',{0,0}},
        {'y',{1,0}}
    };//Use a mapper function inside???? (needs depth)
    std::unordered_map<char, float> globals =
    {
        {'g', 5.f},
        {'h', 7.f}
    };

{
    const std::string s1 = "5+x";
    const std::string s2 = "5x^2y";
    const std::string s3 = "-(x+1)(x-1)^2";
    const std::string s4 = "x+1+g+(-3+y)";
    const std::string s5 = "2max(4,2x)";
    const std::string s6 = "randg";
    const std::string s7 = "5x^(2y)";
    //const std::string s8 = "5x^2y";
    //const std::string s9 = "5x^2y";

    EVAL::RuntimeLoader R;
    R.init();
    R.setGlobalMap(globals);
    R.setOffset(2);

    EVAL::Evaluator *e1 = R.load(s1, varMap, 1, "test1");
    EVAL::Evaluator *e2 = R.load(s2, varMap, 1, "test2");
    EVAL::Evaluator *e3 = R.load(s3, varMap, 1, "test3");
    EVAL::Evaluator *e4 = R.load(s4, varMap, 1, "test4");
    EVAL::Evaluator *e5 = R.load(s5, varMap, 1, "test5");
    EVAL::Evaluator *e6 = R.load(s6, varMap, 1, "test6");
    EVAL::Evaluator *e7 = R.load(s7, varMap, 1, "test7");
    //EVAL::Evaluator *e8 = R.load(s8, varMap, 1, "test8");
    //EVAL::Evaluator *e9 = R.load(s9, varMap, 1, "test9");

    R.generate();
    R.update();

    std::vector<float> v(R.getMaxStackSz() + 2);
    v[0] = 3.f;//x
    v[1] = 4.f;//y
    
    float f1 = e1->evaluate(v.data());
    assert(f1 == 8.f);
    float f2 = e2->evaluate(v.data());//5x^2y
    assert(f2 == 180.f);
    float f3 = e3->evaluate(v.data());//-(x+1)(x-1)^2
    assert(f3 == -16.f);
    float f4 = e4->evaluate(v.data());//x+1+g+(-3+y)
    assert(f4 == 10.f);//should be 3 pushes and 2 pops
    float f5 = e5->evaluate(v.data());//2max(4,2x)
    assert(f5 == 12.f);
    float f6 = e6->evaluate(v.data());//randg
    assert(f6 < globals['g']);//0-4
    float f7 = e7->evaluate(v.data());//5x^(2y)
    assert(f7 == 32805.f);
}

    return 0;
}

/*

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
*/
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