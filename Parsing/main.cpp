#include "LSFileParser.hpp"
#include "LSParseFuncs.hpp"
#include "../Containers/OstreamOperators.hpp"
#include <iostream>

int main()
{
    LSYSTEM::LSentence L = loadLSentence("ab(3)cd(3,5.5.5,7)ba");

    std::cout<<L<<std::endl;

    //getParam?
    std::string param;
    std::string test = "(34.235,FFF,,ffma)ksjfd";
    uint i = 0;
    std::cout<<test<<"\n";
    while(LSPARSE::getNextParam(test,i,param))
    {
        std::cout<<param<<"\n";
        param.clear();
    }
    std::cout<<param<<"\n";

    return 0;
}