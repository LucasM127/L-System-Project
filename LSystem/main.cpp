#include "LSentence.hpp"
#include <iostream>
//Sentence has its own 'local' alphabet
//Each LSystem Production algorithm set has its own 'local' alphabet
//if they speak the same language they can work together, if not they can't

//TODO Fix cut algorithm
//returning the next index to be out of range.
//Never 'tested' it properly anyways
using namespace LSYSTEM;

int main()
{
    std::string string = "ABC[DE][SG[HI[JK]L][xX]MNO]";
    std::string lstring = "BC";
    std::string rstring = "G[H]M";
    LSYSTEM::Alphabet abc;
    for(auto &c : string) abc[c] = 0;
    abc['['] = 2;
    abc[']'] = 1;
    abc['E'] = 4;
    LSYSTEM::VLSentence S("A(4,5)BA(3.33,2)B");
    std::cout<<S<<std::endl;
    //LSYSTEM::LSentence S(abc);
    //LSYSTEM::LSentence lS(abc);
    //LSYSTEM::LSentence rS(abc);
    //abc['A'] = 3;
    //S.loadSimple(string);
    //lS.loadSimple(lstring);
    //rS.loadSimple(rstring);
    //std::set<char> skipSet;

    //S.print();
    //printS(S);

    /*
    
    LSYSTEM::ProductionContext PC(lstring,rstring,skipSet);
    unsigned int i = 17;//13;
    bool Pleft = PC.passLContext(S, i);
    if(Pleft) std::cout<<"Pass left\n";
    bool Pright = PC.passRContext(S,i);
    if(Pright) std::cout<<"Pass right\n";

    LSYSTEM::LSystemData data;
    LSYSTEM::LSystem L(data);
    
//    for(i = 0; i < S.size(); ++i)
    {
        bool Pleft = PC.passLContext(S, i);
        bool Pright = PC.passRContext(S,i);
        //if(Pleft) std::cout<<"Pass left ";
        //if(Pright) std::cout<<"Pass right ";
        if(PC.passLContext(S, i) && PC.passRContext(S,i))
        {
            std::cout<<i<<" "<<S[i].id<<" Pass\n";
            for(uint j = 0; j < PC.rContext.size(); ++j)
            {
                std::cout<<PC.rContextIndices[j]<<" ";
            }
            std::cout<<"\n";
        }
        else
            std::cout<<i<<" "<<S[i].id<<" Fail\n";
    }
    */
   return 0;
}
