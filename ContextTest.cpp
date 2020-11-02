#include <iostream>

#include "Production.hpp"

class PContextTest : public LSYSTEM::ProductionContext
{
public:
    PContextTest(const std::string &lc, const std::string &rc, const std::set<char> &_skippableLetters) : ProductionContext(lc, rc, _skippableLetters){}
    
    void printContext(const std::vector<char> &lsentence, const unsigned int loc)
    {
        std::cout<<"LContext : "<<lContext<<"\n";
        std::cout<<"           ";
        for(uint i = 0; i < lContext.size(); i++)
            std::cout<<i%10;
        std::cout<<"\n";
        std::cout<<"RContext : "<<rContext<<"\n";
        for(uint i = 0; i < rContext.size(); i++)
            std::cout<<i%10;
        std::cout<<"\n";
        
        bool passL = passLContext(lsentence, loc);
        bool passR = passRContext(lsentence, loc);
        if(passL) std::cout<<"Passes L Context"<<"\n";
        if(passR) std::cout<<"Passes R Context"<<"\n";

        //the sentence
        for(auto c : lsentence) std::cout<<c;
        std::cout<<"\n";
        if(passL && lContext.size())
        {
            uint k = 0;
            for(uint i = 0; i < loc; i++)
            {
                uint nextIndex = lContextIndices[k];
                if(i == nextIndex)
                {
                    std::cout<<i%10;
                    if( (k+1) < lContext.size()) k++;
                }
                else std::cout<<" ";
            }
        }
        else
            for(uint i = 0; i < loc; i++) std::cout<<" ";
        std::cout<<"^";
        if(passR && rContext.size())
        {
            uint k = 0;
            for(uint i = loc+1; i < lsentence.size(); i++)
            {
                uint nextIndex = rContextIndices[k];
                if(i == nextIndex)
                {
                    std::cout<<i%10;
                    if( (k+1) < rContext.size()) k++;
                }
                else std::cout<<" ";
            }
        }
        else
            for(uint i = 0; i < loc; i++) std::cout<<" ";
        std::cout<<std::endl;

        for(uint i = 0; i < lContext.size(); ++i) std::cout<<lContextIndices[i]<<" ";
        std::cout<<std::endl;
        for(uint i = 0; i < rContext.size(); ++i) std::cout<<rContextIndices[i]<<" ";
        std::cout<<std::endl;
    }
};

std::vector<char> convert(const std::string &s)
{
    std::vector<char> v;
    for(auto c : s) v.push_back(c);
    return v;
}

int main()
{
    std::set<char> skippable = {'+','-'};
    
    //std::vector<char> lcontext = {'A', 'B', 'C'};
    //std::vector<char> rcontext = {'E', 'F', 'G'};
    std::string lcontext = "A";
    std::string rcontext = "[F]";

    PContextTest test(lcontext, rcontext, skippable);

    std::vector<char> lsentence = convert("AF[+F][-F]");
//    {
//        'A','B','x','C','D','E','[','d','g',']','[','x','d','e','f',']','F','G'
//    };

    test.printContext(lsentence, 1);

    return 0;
}