#include "LSFileParser.hpp"
#include "../Containers/OstreamOperators.hpp"
#include <iostream>

int main()
{
    std::string fileName = "test.ls";

    LSParser lsparser;
    LSYSTEM::LSFileData fd;
    LSYSTEM::LSystemData lsData;
    lsparser.parseFile(fileName,fd,lsData);

    std::cout<<"After parsing "<<fileName<<"\n";

    std::cout << fd << std::endl;

    std::cout << lsData << std::endl;

    return 0;
}