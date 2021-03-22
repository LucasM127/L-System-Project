#include "LSystem.hpp"

#include "../Evaluator/LibEvalLoader.hpp"
//just include parsing here???
#include "../Parsing/ProductionParser.hpp"

namespace LSYSTEM
{

//need to add if can't find...????
LSystem::LSystem(const LSData &lsData)// : alphabet(lsData.abc), skippableLetters(lsData.skippableLetters),
                                        //        m_evalLoader(nullptr), 
                                        : m_maxDepth(0), m_maxWidth(0), m_valArray(nullptr)
{
    LSDataParser lsdp;
    lsdp.parse(lsData);
    m_alphabet = std::move(lsdp.alphabet);
    m_skippableLetters = std::move(lsdp.skippableLetters);

    //calculate size of containers needed to hold variables
    for(auto &pair : m_alphabet)
    {
        if(pair.second > m_maxDepth)
        {
            m_maxDepth = pair.second;
        }
    }
    for(const ProductionData &pd : lsdp.productionDatas)
    {
        unsigned int contextSize = pd.lContext.size() + 1 + pd.rContext.size();
        if(contextSize > m_maxWidth) m_maxWidth = contextSize;
    }
    for(const ProductionData &pd : lsdp.decompositionProductionDatas)
    {
        unsigned int contextSize = pd.lContext.size() + 1 + pd.rContext.size();
        if(contextSize > m_maxWidth) m_maxWidth = contextSize;
    }
    for(const ProductionData &pd : lsdp.homomorphicProductionDatas)
    {
        unsigned int contextSize = pd.lContext.size() + 1 + pd.rContext.size();
        if(contextSize > m_maxWidth) m_maxWidth = contextSize;
    }
    bool isSimple = (m_maxDepth == 0);

    if(isSimple)
    {
        BasicProduction *tempProduction = nullptr;
        for(const ProductionData &productionData : lsdp.productionDatas)
        {
            try
            {
                if(productionData.lContext.size()||productionData.rContext.size())
                    tempProduction = new Production(productionData.products, m_alphabet, productionData.lContext, productionData.rContext, m_skippableLetters);
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
        for(const ProductionData &productionData : lsdp.decompositionProductionDatas)
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
        for(const ProductionData &productionData : lsdp.homomorphicProductionDatas)
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
        m_evalLoader = new EVAL::RuntimeLoader;//EVAL::LibLoader;
        m_evalLoader->init();
        dynamic_cast<EVAL::RuntimeLoader*>(m_evalLoader)->setOffset(m_maxDepth*m_maxWidth);//ooops

        BasicParametricProduction *tempProduction = 0;
        for(const ProductionData &productionData : lsdp.productionDatas)
        {
            try
            {
                if(productionData.lContext.size()||productionData.rContext.size())
                    tempProduction = new ParametricProduction(productionData, m_alphabet, m_skippableLetters, *m_evalLoader, m_maxDepth);
                else
                    tempProduction = new BasicParametricProduction(productionData, m_alphabet, *m_evalLoader, m_maxDepth);
            }
            catch(std::exception& e)
            {
                for(auto pMap : m_productionMap)
                    for(BasicProduction* pRule : pMap.second)
                        delete pRule;
                throw e;
            }
            m_productionMap[productionData.letter[0]].push_back(tempProduction);
        }

        //non contextual by definition (?)
        for(const ProductionData &productionData : lsdp.decompositionProductionDatas)
        {
            try {tempProduction = new BasicParametricProduction(productionData, m_alphabet, *m_evalLoader, m_maxDepth);}
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
        for(const ProductionData &productionData : lsdp.homomorphicProductionDatas)
        {
            try {tempProduction = new BasicParametricProduction(productionData, m_alphabet, *m_evalLoader, m_maxDepth);}
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

        uint maxStackSz = dynamic_cast<EVAL::RuntimeLoader*>(m_evalLoader)->getMaxStackSz();
        m_valArray = new float[m_maxDepth * m_maxWidth + maxStackSz];
    }
//just singular atm
    
}

LSystem::~LSystem()
{
    if(m_valArray)
        delete[] m_valArray;
}

void LSystem::iterate(const VLSentence &oldSentence, VLSentence &newSentence)
{//try catch throw???
    if(!compatible(oldSentence.m_alphabet, m_alphabet))
        throw std::runtime_error("LSentence Incompatible with production alphabet");
    combine(m_alphabet, newSentence.m_alphabet);

    const LSentence &oldLSentence = oldSentence.m_lsentence;
    LSentence &newLSentence = newSentence.m_lsentence;

    LSentence tempSentence;

    for(uint i = 0; i < oldLSentence.size(); i = oldLSentence.next(i))
    {
        applyCut(oldLSentence, i);
        if(i >= oldLSentence.size()) break;
        tempSentence.clear();
        applyProduct(oldLSentence, tempSentence, i, m_productionMap, m_valArray);
        decompose(tempSentence, newLSentence, m_valArray);
    }
}

void LSystem::interpret(VLSentence &vlsentence, LSInterpreter &I, LSReinterpreter &R)
{
    if(!compatible(vlsentence.m_alphabet, m_alphabet))
        throw;
    combine(m_alphabet, vlsentence.m_alphabet);

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
    if(!compatible(vlsentence.m_alphabet, m_alphabet))
        throw;
    combine(m_alphabet, vlsentence.m_alphabet);

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
    if(!compatible(m_alphabet, vlsentence.m_alphabet))
        throw std::runtime_error("Incompatible alphabets");
    combine(m_alphabet, vlsentence.m_alphabet);
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

void LSystem::applyCut(const LSentence &oldLSentence, unsigned int &curIndex)
{
    char c = oldLSentence[curIndex].id;
    if(c != '%') return;
    int curLvl = 0;
    while ((curIndex = oldLSentence.next(curIndex)) < oldLSentence.size())
    {
        c = oldLSentence[curIndex].id;
        if(c == '[') curLvl++;
        if(c == ']' && curLvl == 0) break;
        if(c == ']') curLvl--;
    }
    //curIndex = oldLSentence.next(curIndex);
    //cur Index resumes after the 'brackets' need to check validity though
    return;
}
/*
//THIS CODE IS JUST WRONG... I can create a seperate function for removing 'empty' brackets later,
//but they may be needed later
void LSystem::applyCut(const LSentence &LString, LSentence &LString, unsigned int &curIndex)
{
    //just no skip letters
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