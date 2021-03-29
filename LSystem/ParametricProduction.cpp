#include "ParametricProduction.hpp"

namespace LSYSTEM
{

BasicParametricProduction::BasicParametricProduction(const ProductionData &ppd, EVAL::Loader &ev, const unsigned int arrayDepth)
                                                    : BasicProduction(ppd, ev, arrayDepth)
{}

BasicParametricProduction::~BasicParametricProduction()
{}

ParametricProduction::ParametricProduction(const ProductionData& ppd,
                     EVAL::Loader &ev, const unsigned int arrayDepth,
                     const std::set<char> &skip)
                     :BasicParametricProduction(ppd, ev, arrayDepth),
                     ProductionContext(ppd.lContext, ppd.rContext, skip)
{
    //std::cout<<"Creating ParametricProduction"<<std::endl;
}

ParametricProduction::~ParametricProduction()
{
//    std::cout<<"Destroying ParametricProduction"<<std::endl;
}

Product* BasicParametricProduction::pass(const LSentence &lsentence, const unsigned int i, float * V, unsigned int *indiceHolder)
{
    updateValHolder(lsentence,i,0, V);
    return m_chooser->choose(V);
}

Product* ParametricProduction::pass(const LSentence &lsentence, const unsigned int i, float * V, unsigned int *indiceHolder)
{
    if(lContext.size() && (!passLContext(lsentence,i,indiceHolder))) return nullptr;
    if(rContext.size() && (!passRContext(lsentence,i,&indiceHolder[lContext.size()]))) return nullptr;

    for(unsigned int n = 0;n<lContext.size();n++) updateValHolder(lsentence,indiceHolder[n], n, V);
    updateValHolder(lsentence,i, lContext.size(), V);
    for(unsigned int n = 0;n<rContext.size();n++) updateValHolder(lsentence,indiceHolder[n], n + 1 + lContext.size(), V);

    return m_chooser->choose(V);
}

//work on passing by reference stuff next
void BasicParametricProduction::updateValHolder(const LSentence &lsentence, const unsigned int sentenceIndex, const unsigned int valHolderIndex,
                                                float * V)
{
    for(int i = 0; i < lsentence[sentenceIndex].numParams; ++i)
    {
        V[valHolderIndex * m_arrayDepth + i] = lsentence[sentenceIndex+i+1].value;
    }
}

}//namespace LSYSTEM