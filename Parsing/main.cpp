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
        "F=>F+G-F"
    };
    lsData.homomorphisms = 
    {
        "G=>F(x)"
    };//what can I do, if I infer....
    LSDataParser P;
    P.parse(lsData);

    for(auto &pd : P.productionDatas)
        std::cout<<pd<<"\n";

    return 0;
}