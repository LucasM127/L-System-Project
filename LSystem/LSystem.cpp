#include "LSystem.hpp"

namespace LSYSTEM
{

//need to add if can't find...????
LSystem::LSystem(const LSystemData &lsData) : alphabet(lsData.abc), skippableLetters(lsData.skippableLetters),
                                                m_evalLoader(nullptr), m_maxDepth(0), m_maxWidth(0)
{
    for(auto &pair : alphabet)
    {
        if(pair.second > m_maxDepth)
        {
            m_maxDepth = pair.second;
        }
    }
    for(const ProductionData &pd : lsData.productionDatas)
    {
        unsigned int contextSize = pd.lContext.size() + 1 + pd.rContext.size();
        if(contextSize > m_maxWidth) m_maxWidth = contextSize;
    }
    for(const ProductionData &pd : lsData.decompositionProductionDatas)
    {
        unsigned int contextSize = pd.lContext.size() + 1 + pd.rContext.size();
        if(contextSize > m_maxWidth) m_maxWidth = contextSize;
    }
    for(const ProductionData &pd : lsData.homomorphicProductionDatas)
    {
        unsigned int contextSize = pd.lContext.size() + 1 + pd.rContext.size();
        if(contextSize > m_maxWidth) m_maxWidth = contextSize;
    }
    bool isSimple = (m_maxDepth == 0);

    if(isSimple)
    {
        BasicProduction *tempProduction = nullptr;
        for(const ProductionData &productionData : lsData.productionDatas)
        {
            try
            {
                if(productionData.lContext.size()||productionData.rContext.size())
                    tempProduction = new Production(productionData.products, alphabet, productionData.lContext, productionData.rContext, skippableLetters);
                else
                    tempProduction = new BasicProduction(productionData.products);
            }
            catch(const std::exception& e)
            {
                for(auto pMap : m_productionMap)
                for(BasicProduction* pRule : pMap.second)
                    delete pRule;
                throw e;
            }
            m_productionMap[productionData.letter[0]].push_back(tempProduction);
        }
        for(const ProductionData &productionData : lsData.decompositionProductionDatas)
        {
            try {tempProduction = new BasicProduction(productionData.products);}
            catch(std::exception& e)
            {
                for(auto pMap : m_decompositionMap)
                    for(BasicProduction* pRule : pMap.second)
                        delete pRule;
                throw e;
            }

            m_decompositionMap[productionData.letter[0]].push_back(tempProduction);
        }

        //non contextual by definition (?)
        for(const ProductionData &productionData : lsData.homomorphicProductionDatas)
        {
            try {tempProduction = new BasicProduction(productionData.products);}
            catch(std::exception& e)
            {
                for(auto pMap : m_homomorphismMap)
                    for(BasicProduction* pRule : pMap.second)
                        delete pRule;
                throw e;
            }

            m_homomorphismMap[productionData.letter[0]].push_back(tempProduction);
        }
    }
    else
    {
        m_evalLoader = new LibEvalLoader;//RuntimeEvalLoader;
        m_evalLoader->init();

        BasicParametricProduction *tempProduction = 0;
        for(const ProductionData &productionData : lsData.productionDatas)
        {
            try
            {
                if(productionData.lContext.size()||productionData.rContext.size())
                    tempProduction = new ParametricProduction(productionData, alphabet, skippableLetters, *m_evalLoader, m_maxDepth);
                else
                    tempProduction = new BasicParametricProduction(productionData, alphabet, *m_evalLoader, m_maxDepth);
            }
            catch(std::exception& e)
            {
                LOG("exception caught: ", e.what());
                for(auto pMap : m_productionMap)
                    for(BasicProduction* pRule : pMap.second)
                        delete pRule;
                throw e;
            }
            m_productionMap[productionData.letter[0]].push_back(tempProduction);
        }

        //non contextual by definition (?)
        for(const ProductionData &productionData : lsData.decompositionProductionDatas)
        {
            try {tempProduction = new BasicParametricProduction(productionData, alphabet, *m_evalLoader, m_maxDepth);}
            catch(std::exception& e)
            {
                for(auto pMap : m_decompositionMap)
                    for(BasicProduction* pRule : pMap.second)
                        delete pRule;
                throw e;
            }

            m_decompositionMap[productionData.letter[0]].push_back(tempProduction);
        }

        //non contextual by definition (?)
        for(const ProductionData &productionData : lsData.homomorphicProductionDatas)
        {
            try {tempProduction = new BasicParametricProduction(productionData, alphabet, *m_evalLoader, m_maxDepth);}
            catch(std::exception& e)
            {
                for(auto pMap : m_homomorphismMap)
                    for(BasicProduction* pRule : pMap.second)
                        delete pRule;
                throw e;
            }

            m_homomorphismMap[productionData.letter[0]].push_back(tempProduction);
        }

        m_evalLoader->generate();
    }
//just singular atm
    m_valArray = new float[m_maxDepth * m_maxWidth];
}

LSystem::~LSystem()
{
    delete[] m_valArray;
}

void LSystem::iterate(const VLSentence &oldSentence, VLSentence &newSentence)
{
    if(!compatible(oldSentence.m_alphabet, alphabet))
        throw std::runtime_error("LSentence Incompatible with production alphabet");
    combine(alphabet, newSentence.m_alphabet);

    const LSentence &oldLSentence = oldSentence.m_lsentence;
    LSentence &newLSentence = newSentence.m_lsentence;

    LSentence tempSentence;

    for(uint i = 0; i < oldLSentence.size(); i = oldLSentence.next(i))
    {
//        applyCut(oldSentence,newSentence, curIndex);
//        if(curIndex >= oldSentence.lstring.size()) break;
        tempSentence.clear();
        applyProduct(oldLSentence, tempSentence, i, m_productionMap, m_valArray);
        decompose(tempSentence, newLSentence, m_valArray);
    }
}

void LSystem::interpret(VLSentence &vlsentence, LSInterpreter &I, LSReinterpreter &R)
{
    if(!compatible(vlsentence.m_alphabet, alphabet))
        throw;
    combine(alphabet, vlsentence.m_alphabet);

    R.contract(vlsentence);

    I.reset();

    LSentence &lsentence = vlsentence.m_lsentence;
    LSentence tempLSentence;

    for(uint i = 0; i < lsentence.size(); i = lsentence.next(i))
    {
        tempLSentence.clear();
        applyProductionRecursively(lsentence, i, tempLSentence, m_homomorphismMap, m_valArray);
        for(uint j = 0; j < tempLSentence.size(); j = tempLSentence.next(j))
            I.interpret({tempLSentence, j});//tempLSentence, j);
        R.reinterpret(lsentence, i, vlsentence);
    }
}

void LSystem::interpret(VLSentence &vlsentence, LSInterpreter &I)
{
    if(!compatible(vlsentence.m_alphabet, alphabet))
        throw;
    combine(alphabet, vlsentence.m_alphabet);

    I.reset();

    LSentence &lsentence = vlsentence.m_lsentence;
    LSentence tempLSentence;

    for(uint i = 0; i < lsentence.size(); i = lsentence.next(i))
    {
        tempLSentence.clear();
        applyProductionRecursively(lsentence, i, tempLSentence, m_homomorphismMap, m_valArray);
        for(uint j = 0; j < tempLSentence.size(); j = tempLSentence.next(j))
            I.interpret({tempLSentence, j});//tempLSentence, j);
    }
}

void LSystem::contract(LSYSTEM::VLSentence &vlsentence)
{
    if(!compatible(alphabet, vlsentence.m_alphabet))
        throw std::runtime_error("Incompatible alphabets");
    combine(alphabet, vlsentence.m_alphabet);
}

Product* LSystem::findMatch(const unsigned int i, const LSentence &refLS, std::unordered_map<char, std::vector<BasicProduction*> > &productionMap, float *V)
{
    char c = refLS[i].id;

    if(productionMap.find(c)==productionMap.end())
    {
        return nullptr;
    }

    std::vector<BasicProduction*> &pRules = productionMap[c];
    ProductChooser* chooser;

    for(unsigned int j = 0; j<pRules.size(); j++)
    {
        chooser = pRules[j]->pass(refLS,i,V,m_maxDepth);//updates the variables in the array we pass
        if(chooser) return chooser->choose(V);
    }

    return nullptr;
}

void LSystem::applyBasicProduct(const LSentence &oldSentence, LSentence &newSentence, const unsigned int curIndex)
{
    newSentence.push_back(oldSentence[curIndex].id, oldSentence[curIndex].numParams);
    for(uint i = 0; i < oldSentence[curIndex].numParams; ++i)
    {
        newSentence.push_back(oldSentence[curIndex+i+1].value);
    }
    return;
}

void LSystem::applyProduct(const LSentence &oldSentence, LSentence &newSentence, unsigned int &curIndex,//why by reference? why not const???????
                            std::unordered_map<char, std::vector<BasicProduction*> > &productMap, float *V)
{
    Product *product = findMatch(curIndex, oldSentence, productMap, V);
    if(product == nullptr)
    {//Just duplicate it
        applyBasicProduct(oldSentence, newSentence, curIndex);
        return;
    }

    try {product->apply(newSentence, V);}
    catch (std::exception &e) {throw e;}
}

//void LSystem::applyCut(const LSentence &lsentence, LSentence &lsentence, unsigned int &curIndex)
//{}

/*THIS CODE IS JUST WRONG... I can create a seperate function for removing 'empty' brackets later,
//but they may be needed later
void LSystem::applyCut(const LSentence &LString, LSentence &LString, unsigned int &curIndex)
{
    char c = oldSentence[curIndex].id;
    bool poppedBack = false;
    if(c != '%') return;

    //go back in newSentence, see if had a prior '[' to remove
    unsigned int newIndex;
    unsigned int numToPop = 0;
    if(newSentence.getLastLetterIndex(newIndex))
    {
        do
        {
            ++numToPop;
            c = newSentence[newIndex].id;
            if(!isSkippable(c, skippableLetters))
                break;
        } while (newSentence.last(newIndex,newIndex));
    }
    if(c == '[')
    {//remove '[' if the whole branch is cut off anyways ahead, so no "[]" in string
        poppedBack = true;
        for(uint i = 0; i < numToPop; ++i)
            newSentence.pop_back();
    }
    
    int curLvl = 0;
    while (oldSentence.next(curIndex,curIndex))
    {
        c = oldSentence[curIndex].id;
        if(c == '[') curLvl++;
        if(c == ']' && curLvl == 0) break;
        if(c == ']') curLvl--;
    }
    if(poppedBack)
    {
        if(oldSentence.next(curIndex,curIndex) && oldSentence[curIndex].id == '%')
            applyCut(oldSentence, newSentence, curIndex);//tail recursion
        //CAN GO OUT OF BOUNDS
    }

//    std::cout<<"After cut at curIndex "<<curIndex<<" "<<(char)oldSentence[curIndex].id<<"\n";
}*/

void LSystem::decompose(const LSentence &undecomposedSentence, LSentence &newSentence, float *V)
{
    for(uint i = 0; i < undecomposedSentence.size(); i = undecomposedSentence.next(i))
    {
        applyProductionRecursively(undecomposedSentence, i, newSentence, m_decompositionMap, V);
//        decompose(undecomposedSentence, newSentence, curIndex, V);
    }
}

void LSystem::applyProductionRecursively(const LSentence &lsentence, const unsigned int i, LSentence &newLSentence,
                                        ProductionMap &pm, float *V)
{   
    LSentence tempSentence;
    Product *P = findMatch(i, lsentence, pm, V);
    if(P == nullptr)
    {
        applyBasicProduct(lsentence, newLSentence, i);
        return;
    }
    
    P->apply(tempSentence, V);
    for(uint j = 0; j < tempSentence.size(); j = tempSentence.next(j))
    {
        applyProductionRecursively(tempSentence,j, newLSentence, pm, V);
    }
}

/*
void LSystem::decompose(const LString &undecomposedSentence, LString &newSentence, const unsigned int curIndex, float *V)
{
    
    LString tempSentence;
    Product *P = findMatch(curIndex, undecomposedSentence, m_decompositionMap, V);
    if(P == nullptr)
    {
        applyBasicProduct(undecomposedSentence, newSentence, curIndex);
        return;
    }
    
    P->apply(tempSentence, V);
    for(uint i = 0; i < tempSentence.size(); i = tempSentence.next(i))
    {
        decompose(tempSentence, newSentence, i, V);
    }
}*/

} // namespace LSYSTEM