#include "LSystem.hpp"

namespace LSYSTEM
{

//std::move????
LSystem::LSystem(const std::string& axiom, std::set<char> &skippableLetters,
                      std::vector<ProductionData> &productionDatas,
                      std::vector<ProductionData> &decompositionProductionDatas,
                      std::vector<ProductionData> &homomorphicProductionDatas)
                      : m_it_ctr(0), atMaxIteration(false), m_axiomString(axiom),
                      V(nullptr), m_arrayDepth(0), m_skippableLetters(skippableLetters)
{
    lsentenceA = new LSentence(axiom);
    lsentenceB = new LSentence;

    BasicProduction *tempProduction = 0;
    for(ProductionData &productionData : productionDatas)
    {
        try
        {
            if(productionData.lContext.size()||productionData.rContext.size())
                tempProduction = new Production(productionData.products, productionData.lContext, productionData.rContext, skippableLetters);
            else
                tempProduction = new BasicProduction(productionData.products);
        }
        catch(std::exception& e)
        {
            for(auto pMap : productions)
                for(BasicProduction* pRule : pMap.second)
                    delete pRule;
            throw e;
        }
        productions[productionData.letter[0]].push_back(tempProduction);
    }

    //non contextual by definition (?)
    for(ProductionData productionData : decompositionProductionDatas)
    {
        try {tempProduction = new BasicProduction(productionData.products);}
        catch(std::exception& e)
        {
            for(auto pMap : decompositionProductions)
                for(BasicProduction* pRule : pMap.second)
                    delete pRule;
            throw e;
        }

        decompositionProductions[productionData.letter[0]].push_back(tempProduction);
    }

    //non contextual by definition (?)
    for(ProductionData productionData : homomorphicProductionDatas)
    {
        try {tempProduction = new BasicProduction(productionData.products);}
        catch(std::exception& e)
        {
            for(auto pMap : homomorphicProductions)
                for(BasicProduction* pRule : pMap.second)
                    delete pRule;
            throw e;
        }

        homomorphicProductions[productionData.letter[0]].push_back(tempProduction);
    }
    
    oldSentence = lsentenceA;
    newSentence = lsentenceB;
}

LSystem::LSystem(PLSystem *PL, const std::string &axiom, std::set<char> &skip) : m_it_ctr(0), atMaxIteration(false), m_axiomString(axiom),
                                                                                V(nullptr), m_skippableLetters(skip)
{}

PLSystem::PLSystem(const std::string& axiom, std::set<char> &skip,
                      std::vector<ProductionData> &productionDatas,
                      std::vector<ProductionData> &decompositionProductionDatas,
                      std::vector<ProductionData> &homomorphicProductionDatas,
                      ParamNumMapping &pnm) : LSystem(this, axiom, skip), m_pnm(pnm)
{
    //get max size of array
    m_arrayDepth = 0;
    for(auto paramNum : m_pnm)
    {
        if(paramNum.second > m_arrayDepth) m_arrayDepth = paramNum.second;
    }
    unsigned int maxContextSize = 0;
    for(ProductionData &pd : productionDatas)
    {
        unsigned int contextSize = pd.lContext.size() + 1 + pd.rContext.size();
        if(contextSize > maxContextSize) maxContextSize = contextSize;
    }
    V = new float[maxContextSize * m_arrayDepth];

    m_evalLoader.init();

    lsentenceA = new PLSentence(axiom, m_pnm);
    lsentenceB = new PLSentence(m_pnm);

    BasicParametricProduction *tempProduction = 0;
    for(ProductionData &productionData : productionDatas)
    {
        try
        {
            if(productionData.lContext.size()||productionData.rContext.size())
                tempProduction = new ParametricProduction(productionData, m_pnm, m_skippableLetters, m_evalLoader, m_arrayDepth);
            else
                tempProduction = new BasicParametricProduction(productionData, m_pnm, m_evalLoader, m_arrayDepth);
        }
        catch(std::exception& e)
        {
            for(auto pMap : productions)
                for(BasicProduction* pRule : pMap.second)
                    delete pRule;
            throw e;
        }
        productions[productionData.letter[0]].push_back(tempProduction);
    }

    //non contextual by definition (?)
    for(ProductionData productionData : decompositionProductionDatas)
    {
        try {tempProduction = new BasicParametricProduction(productionData, m_pnm, m_evalLoader, m_arrayDepth);}
        catch(std::exception& e)
        {
            for(auto pMap : decompositionProductions)
                for(BasicProduction* pRule : pMap.second)
                    delete pRule;
            throw e;
        }

        decompositionProductions[productionData.letter[0]].push_back(tempProduction);
    }

    //non contextual by definition (?)
    for(ProductionData productionData : homomorphicProductionDatas)
    {
        try {tempProduction = new BasicParametricProduction(productionData, m_pnm, m_evalLoader, m_arrayDepth);}
        catch(std::exception& e)
        {
            for(auto pMap : homomorphicProductions)
                for(BasicProduction* pRule : pMap.second)
                    delete pRule;
            throw e;
        }

        homomorphicProductions[productionData.letter[0]].push_back(tempProduction);
    }

    m_evalLoader.generate();

    oldSentence = lsentenceA;
    newSentence = lsentenceB;
}

LSystem::~LSystem()
{
    for(auto pMap : productions)
                    for(BasicProduction* pRule : pMap.second)
                        delete pRule;
    
    for(auto pMap : decompositionProductions)
                    for(BasicProduction* pRule : pMap.second)
                        delete pRule;
    
    for(auto pMap : homomorphicProductions)
                    for(BasicProduction* pRule : pMap.second)
                        delete pRule;
}

PLSystem::~PLSystem()
{
    delete[] V;
    m_evalLoader.close();
}

void LSystem::reset()
{
    newSentence->clear();
    oldSentence->clear();
    oldSentence->set(m_axiomString);
}

void LSystem::iterate()
{
    if(atMaxIteration) return;
    m_it_ctr++;

    try
    {
        //Apply regular productions here, also the cut production
        for(unsigned int i = 0; i < oldSentence->size(); ++i)
        {
            applyCut(i);
            applyProduct(i);
        }
        oldSentence->clear();
        std::swap(newSentence, oldSentence);
        //apply decomposition productions afterwards...
        decompose();
    }
    catch(std::exception &e)
    {
        atMaxIteration = true;
    }
}

//that should do it...

void LSystem::applyCut(unsigned int &i)
{
    char c = oldSentence->getSentence()[i];
    bool poppedBack = false;
    if(c == '%')
    {
        unsigned int k, j;
        for(j = 1; j <= newSentence->size(); ++j)
        {
            k = newSentence->size() - j;
            c = newSentence->getSentence()[k];
            if(!isSkippable(c, m_skippableLetters))
                break;
        }
        if(c == '[')
        {
            poppedBack = true;
            //popback
            for(unsigned int k = 0; k < j; k++)
                newSentence->pop_back();
        }

        //REDUNDANT FUNCTION USE ELSEWHERES ALSO SHOULD TRY AND TAKE IT OUT?
        //get matching r bracket ] or end of sentence whichever is first
        int curLvl = 0;
        while(i < oldSentence->size())
        {
            ++i;
            c = oldSentence->getSentence()[i];
            if(c == '[') curLvl++;
            if(c == ']' && curLvl == 0) break;
            if(c == ']') curLvl--;
        }
        if(poppedBack) ++i;
    }
}

Product* LSystem::findMatch(const unsigned int i, LSentence* refLS, ProductionMapping &productionMap)
{
    char c = refLS->getSentence()[i];

    if(productionMap.find(c)==productionMap.end())
    {
        return nullptr;
    }

    std::vector<BasicProduction*> &pRules = productionMap[c];
    ProductChooser* chooser;

    for(unsigned int j = 0; j<pRules.size(); j++)
    {
        chooser = pRules[j]->pass(refLS,i,V,m_arrayDepth);//updates the variables in the array we pass
        if(chooser) return chooser->choose(V);
    }

    return nullptr;
}

//onto what sentence?
void LSystem::applyProduct(const unsigned int i)
{
    Product *curProduct = findMatch(i,oldSentence,productions);
    if(!curProduct)
    {
        newSentence->push_back(*oldSentence,i);
        return;
    }

    try {curProduct->apply(*newSentence, V);}
    catch (std::exception &e) {throw e;}
}

//homomorphic productions are 'recursive'
//apply it to the letter
//put it in the sentence
//feed turtle the new sentence...
//or use a stack.
void LSystem::applyHomomorphicProduction(LSInterpreter &T, LSentence* sentence, const unsigned int i)
{
    int numVals;
    LSentence tempSentence;
    Product *P = findMatch(i, sentence, homomorphicProductions);
    if(P == nullptr)
        T.feed(sentence->getSentence()[i], sentence->getVals(i,numVals),numVals);
    else
    {
        P->apply(tempSentence, V);
        for(unsigned int j = 0; j < tempSentence.size(); ++j)
            applyHomomorphicProduction(T, &tempSentence, j);
    }
}

void PLSystem::applyHomomorphicProduction(LSInterpreter &T, LSentence* sentence, const unsigned int i)
{
    //PLSentence *psentence = dynamic_cast<PLSentence*>(sentence);
    int numVals;
    PLSentence tempSentence(m_pnm);
    Product *P = findMatch(i, sentence, homomorphicProductions);
    if(P == nullptr)
        T.feed(sentence->getSentence()[i], sentence->getVals(i,numVals),numVals);
    else
    {
        P->apply(tempSentence, V);
        for(unsigned int j = 0; j < tempSentence.size(); ++j)
            applyHomomorphicProduction(T, &tempSentence, j);
    }
}

void LSystem::feed(LSInterpreter &T)
{
    for(unsigned int i = 0; i < oldSentence->size(); ++i)
    {
        applyHomomorphicProduction(T, oldSentence, i);
    }
}

void LSystem::decompose()
{
    if(decompositionProductions.size() == 0) return;
    
    for(unsigned int i = 0; i < oldSentence->size(); ++i)
    {
        applyDecompositionProduction(oldSentence, i);//same as with the homomorphic productions...
    }

    oldSentence->clear();
    std::swap(newSentence, oldSentence);
    int x = 0;
}

//a recursive approach to normal iteration
//just like applyProduct. find a match? y/no apply it, or apply it 'recursively'
void LSystem::applyDecompositionProduction(LSentence *sentence, const unsigned int i)
{
    int numVals;
    LSentence tempSentence;
    Product *P = findMatch(i, sentence, decompositionProductions);
    if(P == nullptr)
        newSentence->push_back(*sentence, i);
    else
    {
        P->apply(tempSentence, V);
        for(unsigned int j = 0; j < tempSentence.size(); ++j)
            applyDecompositionProduction(&tempSentence, j);
    }
}

void PLSystem::applyDecompositionProduction(LSentence *sentence, const unsigned int i)
{
    int numVals;
    PLSentence tempSentence(m_pnm);
    Product *P = findMatch(i, sentence, decompositionProductions);
    if(P == nullptr)
        newSentence->push_back(*sentence, i);
    else
    {
        P->apply(tempSentence, V);
        for(unsigned int j = 0; j < tempSentence.size(); ++j)
            applyDecompositionProduction(&tempSentence, j);
    }
}

}// namespace LSYSTEM