#include "EvalLoader.hpp"
#include <iostream>
#include <string>
#include <cmath>
//#define VAR false,false
//#define OP  true,false
//#define NUM false,true

int main()
{
    srand(time(NULL));
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
//    std::string exp="5.2+3sin(0.7x)";
    std::string exp="10+rand()%x";//"x>70&&x<100";
    std::map<char, VarIndice> varMap =
    {
        {'x',{0,0}}
    };

    float V[1] = {2.f};
    exp = "2cos(90)";
    exp = "max(4max(3,2),10+1)";
    //exp = "2(1+4)";
    EVAL::RuntimeLoader evR;
//    LibEvalLoader evR;
    evR.init();
    
    EVAL::Evaluator *evalR = evR.load(exp, varMap, 1, "foo");
    evR.generate();

    float n = evalR->evaluate(V);
    std::cout<<n<<" is answer\n";
    n = evalR->evaluate(V);
    std::cout<<n<<" is answer\n";
    n = evalR->evaluate(V);
    std::cout<<n<<" is answer\n";
    
    evR.close();

    return 0;
}