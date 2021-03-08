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

//tempsent idea
void LSystem::iterate(const LSentence &oldSentence, LSentence &newSentence)
{
    if(!oldSentence.compatible(alphabet) || !newSentence.compatible(alphabet))
        throw std::runtime_error("LSentence Incompatible with production alphabet");

    LSentence tempSentence(alphabet);

    //unsigned int curIndex = 0;
    for(uint curIndex = 0; curIndex < oldSentence.size(); curIndex = oldSentence.next(curIndex))
    {
        applyCut(oldSentence,newSentence, curIndex);//curIndex could be out of bounds now after the cut?
        if(curIndex >= oldSentence.size()) break;
        tempSentence.clear();
        applyProduct(oldSentence, tempSentence, curIndex, m_productionMap, m_valArray);
        decompose(tempSentence, newSentence, m_valArray);
    }
}

void LSystem::applyHomomorphisms(const LSentence &sentence, LSentence &newSentence)
{
    if(!sentence.compatible(alphabet) || !newSentence.compatible(alphabet))
        throw std::runtime_error("LSentence Incompatible with production alphabet");
    
    for(uint curIndex = 0; curIndex < sentence.size(); curIndex = sentence.next(curIndex))
    {
        applyHomomorphisms(sentence, curIndex, newSentence);
    }
}

void LSystem::applyBasicProduct(const LSentence &oldSentence, LSentence &newSentence, const unsigned int curIndex)
{
    newSentence.push_back((char)oldSentence[curIndex].id);
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
//getting the index to be 'out' of range
//need to test the 'cut' algorithm
//TOMORROW
void LSystem::applyCut(const LSentence &oldSentence, LSentence &newSentence, unsigned int &curIndex)
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
            applyCut(oldSentence, newSentence, curIndex);
        //CAN GO OUT OF BOUNDS
    }

//    std::cout<<"After cut at curIndex "<<curIndex<<" "<<(char)oldSentence[curIndex].id<<"\n";
}

//ie Same as iterate almost
void LSystem::applyHomomorphisms(const LSentence &sentence, unsigned int i, LSentence &product)
{
    //if(!sentence.compatible(m_alphabet) || !product.compatible(m_alphabet))
    //    throw std::runtime_error("LSentence Incompatible with production alphabet");
    
    applyProductionRecursively(sentence, i, product, m_homomorphismMap, m_valArray);
}

void LSystem::decompose(const LSentence &undecomposedSentence, LSentence &newSentence, float *V)
{
    for(uint curIndex = 0; curIndex < undecomposedSentence.size(); curIndex = undecomposedSentence.next(curIndex))
    {
        applyProductionRecursively(undecomposedSentence, curIndex, newSentence, m_decompositionMap, V);
//        decompose(undecomposedSentence, newSentence, curIndex, V);
    }
}

void LSystem::decompose(const LSentence &undecomposedSentence, LSentence &newSentence, const unsigned int curIndex, float *V)
{
    
    LSentence tempSentence(alphabet);
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
}

void LSystem::applyProductionRecursively(const LSentence &sentence, const unsigned int curIndex, LSentence &newSentence,
                                        ProductionMap &pm, float *V)
{   
    LSentence tempSentence(alphabet);
    Product *P = findMatch(curIndex, sentence, pm, V);
    if(P == nullptr)
    {
        applyBasicProduct(sentence, newSentence, curIndex);
        return;
    }
    
    P->apply(tempSentence, V);
    for(uint i = 0; i < tempSentence.size(); i = tempSentence.next(i))
    {
        applyProductionRecursively(tempSentence, i, newSentence, pm, V);
    }
}

} // namespace LSYSTEM