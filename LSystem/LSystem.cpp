#include "LSystem.hpp"

#include "../Evaluator/LibEvalLoader.hpp"
//just include parsing here???

namespace LSYSTEM
{

LSystem::LSystem(const LSData &lsData)
                                        : m_maxDepth(0), m_maxWidth(0), amSimple(true)
{
    LSDataParser lsdp;
    lsdp.parse(lsData);
    m_alphabet = std::move(lsdp.data.abc);
    m_globalMap = std::move(lsdp.data.globalMap);
    m_skippableLetters = std::move(lsdp.data.skippableLetters);

    //calculate size of containers needed to hold variables
    for(auto &pair : m_alphabet)
    {
        if(pair.second > m_maxDepth)
        {
            m_maxDepth = pair.second;
        }
    }
    for(const ProductionData &pd : lsdp.data.productionDatas)
    {
        unsigned int contextSize = pd.lContext.size() + 1 + pd.rContext.size();
        if(contextSize > m_maxWidth) m_maxWidth = contextSize;
    }
    for(const ProductionData &pd : lsdp.data.decompositionProductionDatas)
    {
        unsigned int contextSize = pd.lContext.size() + 1 + pd.rContext.size();
        if(contextSize > m_maxWidth) m_maxWidth = contextSize;
    }
    for(const ProductionData &pd : lsdp.data.homomorphicProductionDatas)
    {
        unsigned int contextSize = pd.lContext.size() + 1 + pd.rContext.size();
        if(contextSize > m_maxWidth) m_maxWidth = contextSize;
    }
    amSimple = (m_maxDepth == 0);

    m_evalLoader.init();
    m_evalLoader.setOffset(m_maxDepth*m_maxWidth);
    m_evalLoader.setGlobalMap(m_globalMap);//for parsing and simplifying

    loadProductions(lsdp);

    m_evalLoader.generate();
    update();

    m_maxStackSz = m_evalLoader.getMaxStackSz();
}

LSystem::~LSystem()
{}//YAY is empty

//May refactor one day but is ok for now.
void LSystem::iterate(const VLSentence &oldSentence, VLSentence &newSentence, bool(*fcont)())
{
    std::vector<float> V(m_maxWidth*m_maxDepth+m_maxStackSz);
    std::vector<unsigned int> indiceHolder(m_maxWidth);

    if(!compatible(oldSentence.m_alphabet, m_alphabet))
        throw std::runtime_error("LSentence Incompatible with production alphabet");
    combine(m_alphabet, newSentence.m_alphabet);
    newSentence.m_alphabet = oldSentence.m_alphabet;

    const LSentence &oldLSentence = oldSentence.m_lsentence;
    LSentence &newLSentence = newSentence.m_lsentence;

    //match appropiate iteration 'type'
    bool applyDecompositions = m_decompositionMap.size() > 0;
    //unsigned int ctr = 0;
    if(applyDecompositions)
    {
        LSentence tempSentence;
        for(unsigned int i = 0; i < oldLSentence.size(); i = oldLSentence.next(i))
        {
            applyCut(oldLSentence, i);
            if(i >= oldLSentence.size()) break;
            tempSentence.clear();
            applyProduct(oldLSentence, tempSentence, i, m_productionMap, V.data(), indiceHolder.data());
            decompose(tempSentence, newLSentence, V.data(), indiceHolder.data());
            //++ctr;
            //if(ctr > 1000)
            {
                //ctr = 0;
                if(!fcont())
                    return;
            }
        }
        return;
    }
    for(unsigned int i = 0; i < oldLSentence.size(); i = oldLSentence.next(i))
    {
        applyCut(oldLSentence, i);
        if(i >= oldLSentence.size()) break;
        applyProduct(oldLSentence, newLSentence, i, m_productionMap, V.data(), indiceHolder.data());
        //++ctr;
        //if(ctr > 1000)
        {
            //ctr = 0;
            if(!fcont())
                return;
        }
    }
}

//iterate once from old -> new
void LSystem::iterate(const VLSentence &oldSentence, VLSentence &newSentence)
{
    std::vector<float> V(m_maxWidth*m_maxDepth+m_maxStackSz);
    std::vector<unsigned int> indiceHolder(m_maxWidth);

    if(!compatible(oldSentence.m_alphabet, m_alphabet))
        throw std::runtime_error("LSentence Incompatible with production alphabet");
    combine(m_alphabet, newSentence.m_alphabet);
    newSentence.m_alphabet = oldSentence.m_alphabet;

    const LSentence &oldLSentence = oldSentence.m_lsentence;
    LSentence &newLSentence = newSentence.m_lsentence;

    //match appropiate iteration 'type'
    bool applyDecompositions = m_decompositionMap.size() > 0;
    
    if(applyDecompositions)
    {
        LSentence tempSentence;
        for(unsigned int i = 0; i < oldLSentence.size(); i = oldLSentence.next(i))
        {
            applyCut(oldLSentence, i);
            if(i >= oldLSentence.size()) break;
            tempSentence.clear();
            applyProduct(oldLSentence, tempSentence, i, m_productionMap, V.data(), indiceHolder.data());
            decompose(tempSentence, newLSentence, V.data(), indiceHolder.data());
        }
        return;
    }
    for(unsigned int i = 0; i < oldLSentence.size(); i = oldLSentence.next(i))
    {
        applyCut(oldLSentence, i);
        if(i >= oldLSentence.size()) break;
        applyProduct(oldLSentence, newLSentence, i, m_productionMap, V.data(), indiceHolder.data());
    }
}

//iterate many times into the same sentence
void LSystem::iterate(VLSentence &sentence, unsigned int n)
{
    float *V = new float[m_maxWidth*m_maxDepth+m_maxStackSz];
    unsigned int *indiceHolder = new unsigned int[m_maxWidth];

    if(!compatible(sentence.m_alphabet, m_alphabet))
        throw std::runtime_error("LSentence Incompatible with production alphabet");
    combine(m_alphabet, sentence.m_alphabet);

    LSentence newSentence;

    LSentence *oldLSentence = &sentence.m_lsentence;
    LSentence *newLSentence = &newSentence;

    //match appropiate iteration 'type'
    bool applyDecompositions = m_decompositionMap.size() > 0;
    
    for(unsigned int i = 0; i < n; ++i)
    {
        if(applyDecompositions)
        {
            LSentence tempSentence;
            for(unsigned int i = 0; i < oldLSentence->size(); i = oldLSentence->next(i))
            {
                applyCut(*oldLSentence, i);
                if(i >= oldLSentence->size()) break;
                tempSentence.clear();
                applyProduct(*oldLSentence, tempSentence, i, m_productionMap, V, indiceHolder);
                decompose(tempSentence, *newLSentence, V, indiceHolder);
            }
        }
        else
        for(unsigned int i = 0; i < oldLSentence->size(); i = oldLSentence->next(i))
        {
            applyCut(*oldLSentence, i);
            if(i >= oldLSentence->size()) break;
            applyProduct(*oldLSentence, *newLSentence, i, m_productionMap, V, indiceHolder);
        }
        oldLSentence->clear();
        std::swap(oldLSentence, newLSentence);
    }

    if(n%2 == 1)
    {
        sentence.m_lsentence = std::move(newSentence);
    }

    delete[] V;
    delete[] indiceHolder;
}

void LSystem::interpret(VLSentence &vlsentence, LSInterpreter &I, LSReinterpreter &R)
{
    std::vector<float> V(m_maxWidth*m_maxDepth+m_maxStackSz);

    if(!compatible(vlsentence.m_alphabet, m_alphabet))
        throw;
    combine(m_alphabet, vlsentence.m_alphabet);

    R.contract(vlsentence);

    I.reset();

    LSentence &lsentence = vlsentence.m_lsentence;
    LSentence tempLSentence;

    for(unsigned int i = 0; i < lsentence.size(); i = lsentence.next(i))
    {
        tempLSentence.clear();
        applyProductionRecursively(lsentence, i, tempLSentence, m_homomorphismMap, V.data(), nullptr);
        for(unsigned int j = 0; j < tempLSentence.size(); j = tempLSentence.next(j))
            I.interpret({tempLSentence, j});//tempLSentence, j);
        R.reinterpret(lsentence, i, vlsentence);
    }
}

void LSystem::interpret(VLSentence &vlsentence, LSInterpreter &I, bool(*fcont)())
{
    std::vector<float> V(m_maxWidth*m_maxDepth+m_maxStackSz);

    if(!compatible(vlsentence.m_alphabet, m_alphabet))
        throw;
    combine(m_alphabet, vlsentence.m_alphabet);

    I.reset();

    LSentence &lsentence = vlsentence.m_lsentence;
    LSentence tempLSentence;

    //unsigned int ctr = 0;
    for(unsigned int i = 0; i < lsentence.size(); i = lsentence.next(i))
    {
        tempLSentence.clear();
        applyProductionRecursively(lsentence, i, tempLSentence, m_homomorphismMap, V.data(), nullptr);
        for(unsigned int j = 0; j < tempLSentence.size(); j = tempLSentence.next(j))
            I.interpret({tempLSentence, j});//tempLSentence, j);
        //++ctr;
        //if(ctr > 1000)
        {
            //ctr = 0;
            if(!fcont())
                return;
        }
    }
}

void LSystem::interpret(VLSentence &vlsentence, LSInterpreter &I)
{
    std::vector<float> V(m_maxWidth*m_maxDepth+m_maxStackSz);

    if(!compatible(vlsentence.m_alphabet, m_alphabet))
        throw;
    combine(m_alphabet, vlsentence.m_alphabet);

    I.reset();

    LSentence &lsentence = vlsentence.m_lsentence;
    LSentence tempLSentence;

    for(unsigned int i = 0; i < lsentence.size(); i = lsentence.next(i))
    {
        tempLSentence.clear();
        applyProductionRecursively(lsentence, i, tempLSentence, m_homomorphismMap, V.data(), nullptr);
        for(unsigned int j = 0; j < tempLSentence.size(); j = tempLSentence.next(j))
            I.interpret({tempLSentence, j});//tempLSentence, j);
    }
}

//SLOW POINT is modulus of the hash function (i think)
Product* LSystem::findMatch(const unsigned int i, const LSentence &refLS, std::unordered_map<char, std::vector<BasicProduction*> > &productionMap,
                            float *V, unsigned int *indiceHolder)
{
    char c = refLS[i].id;

    auto it = productionMap.find(c);//still takes a while here
    if(it == productionMap.end())
    {
        return nullptr;
    }

    std::vector<BasicProduction*> &pRules = it->second;

    for(unsigned int j = 0; j<pRules.size(); ++j)
    {
        Product* product = pRules[j]->pass(refLS,i,V,indiceHolder);//updates the variables in the array we pass
        if(product) return product;
    }

    return nullptr;
}

void LSystem::applyBasicProduct(const LSentence &oldSentence, LSentence &newSentence, const unsigned int curIndex)
{
    newSentence.push_back(oldSentence[curIndex].id, oldSentence[curIndex].numParams);
    for(unsigned int i = 0; i < oldSentence[curIndex].numParams; ++i)
    {
        newSentence.push_back(oldSentence[curIndex+i+1].value);
    }
    return;
}

void LSystem::applyProduct(const LSentence &oldSentence, LSentence &newSentence, unsigned int &curIndex,
                            std::unordered_map<char, std::vector<BasicProduction*> > &productMap, float *V, unsigned int *indiceHolder)
{
    Product *product = findMatch(curIndex, oldSentence, productMap, V, indiceHolder);
    if(product == nullptr)
    {//Just duplicate it
        applyBasicProduct(oldSentence, newSentence, curIndex);
        return;
    }

    product->apply(newSentence, V);
}

void LSystem::applyCut(const LSentence &oldLSentence, unsigned int &curIndex)
{
    char c = oldLSentence[curIndex].id;
    if(c != '%') return;
    int curLvl = 0;
    while ((curIndex = oldLSentence.next(curIndex)) < oldLSentence.size())
    {
        c = oldLSentence[curIndex].id;
        if(c == '[') ++curLvl;
        if(c == ']' && curLvl == 0) break;
        if(c == ']') --curLvl;
    }
    //curIndex = oldLSentence.next(curIndex);
    //cur Index resumes after the 'brackets' need to check validity though
    return;
}

void LSystem::decompose(const LSentence &undecomposedSentence, LSentence &newSentence, float *V, unsigned int *indiceHolder)
{
    for(unsigned int i = 0; i < undecomposedSentence.size(); i = undecomposedSentence.next(i))
    {
        applyProductionRecursively(undecomposedSentence, i, newSentence, m_decompositionMap, V, indiceHolder);
    }
}

void LSystem::applyProductionRecursively(const LSentence &lsentence, const unsigned int i, LSentence &newLSentence,
                                        ProductionMap &pm, float *V, unsigned int *indiceHolder)
{   
    LSentence tempSentence;
    Product *P = findMatch(i, lsentence, pm, V, indiceHolder);
    if(P == nullptr)
    {
        applyBasicProduct(lsentence, newLSentence, i);
        return;
    }
    
    P->apply(tempSentence, V);
    for(unsigned int j = 0; j < tempSentence.size(); j = tempSentence.next(j))
    {
        applyProductionRecursively(tempSentence,j, newLSentence, pm, V, indiceHolder);
    }
}

//some way... to communicate...
void LSystem::updateGlobal(char globalId, const float val)
{
    auto it = m_globalMap.find(globalId);
    if(it == m_globalMap.end()) return;
    it->second = val;
}

void LSystem::update()
{
    m_evalLoader.update();
    for(auto &pm : m_productionMap)
        for(auto &p : pm.second)
            p->update();
    for(auto &pm : m_decompositionMap)
        for(auto &p : pm.second)
            p->update();
    for(auto &pm : m_homomorphismMap)
        for(auto &p : pm.second)
            p->update();
}

void LSystem::loadProductions(LSDataParser &lsdp)
{
    if(amSimple)
    {
        BasicProduction *tempProduction = nullptr;
        for(const ProductionData &productionData : lsdp.data.productionDatas)
        {
            try
            {
                if(productionData.lContext.size()||productionData.rContext.size())
                    tempProduction = new Production(productionData, m_evalLoader, m_maxDepth, m_skippableLetters);
                else
                    tempProduction = new BasicProduction(productionData, m_evalLoader, m_maxDepth);
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
        for(const ProductionData &productionData : lsdp.data.decompositionProductionDatas)
        {
            try {tempProduction = new BasicProduction(productionData, m_evalLoader, m_maxDepth);}
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
        for(const ProductionData &productionData : lsdp.data.homomorphicProductionDatas)
        {
            try {tempProduction = new BasicProduction(productionData, m_evalLoader, m_maxDepth);}
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
        BasicParametricProduction *tempProduction = 0;
        for(const ProductionData &productionData : lsdp.data.productionDatas)
        {
            try
           {
                if(productionData.lContext.size()||productionData.rContext.size())
                    tempProduction = new ParametricProduction(productionData, m_evalLoader, m_maxDepth, m_skippableLetters);
                else
                    tempProduction = new BasicParametricProduction(productionData, m_evalLoader, m_maxDepth);
            }
            catch(std::exception& e)
            {
                for(auto pMap : m_productionMap)
                    for(BasicProduction* pRule : pMap.second)
                        delete pRule;
                throw std::runtime_error(e.what());
            }
            m_productionMap[productionData.letter[0]].push_back(tempProduction);
        }

        //non contextual by definition (?)
        for(const ProductionData &productionData : lsdp.data.decompositionProductionDatas)
        {
            try {tempProduction = new BasicParametricProduction(productionData, m_evalLoader, m_maxDepth);}
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
        for(const ProductionData &productionData : lsdp.data.homomorphicProductionDatas)
        {
            try {tempProduction = new BasicParametricProduction(productionData, m_evalLoader, m_maxDepth);}
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
}

} // namespace LSYSTEM