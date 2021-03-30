#include "LSFileParser.hpp"
#include "LSParseFuncs.hpp"
#include "../Containers/OstreamOperators.hpp"
#include <iostream>
#include "ProductionParser.hpp"

int main()
{
    LSFile lsfile;
    lsfile.loadFile("test.ls");

    LSYSTEM::LSentence L = loadLSentence("A(3,5)B(44.24)C");
    std::cout<<L<<std::endl;
    std::cout<<OSManip::letter<<L<<std::endl;

    return 0;
}