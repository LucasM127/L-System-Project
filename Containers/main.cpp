#include "OstreamOperators.hpp"
#include <iostream>

int main()
{
    LSYSTEM::Alphabet abc = 
    {
        {'b', 4},
        {'f', 42},
        {'j', 0}
    };

    std::cout<<abc<<"\n";

    LSYSTEM::LSentence l;
    l.push_back('a',2);
    l.push_back(5);
    l.push_back(7);

    l.push_back('b',0);

    l.push_back('c',1);
    l.push_back(2);

    l.push_back('a',0);
    l.push_back('d',0);

    std::cout<<OSManip::letter<<l<<" "<<l<<"\n";

    return 0;
}