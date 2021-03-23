#include "LSFileParser.hpp"
#include "LSParseFuncs.hpp"
#include "../Containers/OstreamOperators.hpp"
#include <iostream>
#include "ProductionParser.hpp"

int main()
{
    LSYSTEM::LSData lsData;
    lsData.productions = 
    {
        //"F(a,b)=>F(0,a)+G-F(b,2)"
    };
    lsData.homomorphisms = 
    {
        "G=>F(1,1+(22)1)"
    };//what can I do, if I infer....
    LSDataParser P;
    P.parse(lsData);

    for(auto &pd : P.productionDatas)
        std::cout<<pd<<"\n";
    for(auto &pd : P.homomorphicProductionDatas)
        std::cout<<pd<<"\n";

    LSYSTEM::LSentence L = loadLSentence("A(3,5)B(44.24)C");
    std::cout<<L<<std::endl;
    std::cout<<OSManip::letter<<L<<std::endl;

    return 0;
}