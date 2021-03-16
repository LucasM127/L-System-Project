#include "ParametricProduction.hpp"

namespace LSYSTEM
{

ParametricProduct::ParametricProduct(const ProductData &pd, const Alphabet &pnm, VarIndiceMap &varIndiceMap, EVAL::Loader &ev, const unsigned int arrayDepth)
                                    : Product(this, pd.product)
{
    createProductEvaluations(pd, ev, varIndiceMap, arrayDepth);
//load is ok, getBasicEval is not
    if(pd.productWeight.size())
        stochasticWeightEvaluator = ev.load(pd.productWeight, varIndiceMap, arrayDepth, pd.rawStatement);
    else
        stochasticWeightEvaluator = ev.getBasicEval(pd.rawStatement, 1.f);
    if(pd.conditional.size())
        conditionalEvaluator = ev.load(pd.conditional, varIndiceMap, arrayDepth, pd.rawStatement);
    else
        conditionalEvaluator = ev.getBasicEval(pd.rawStatement, 1.f);//for true

    if(stochasticWeightEvaluator->isConst)
        m_stochasticWeight = stochasticWeightEvaluator->evaluate(nullptr);
}

const float ParametricProduct::calcWeight(const float * V)
{
    m_stochasticWeight = stochasticWeightEvaluator->evaluate(V);
    return m_stochasticWeight;
}

bool ParametricProduct::isValid(const float * V)
{
    return (bool)conditionalEvaluator->evaluate(V);
}

void ParametricProduct::createProductEvaluations(const ProductData& pd, EVAL::Loader &ev, VarIndiceMap &varIndiceMap, const unsigned int arrayDepth)
{
    const std::vector<std::vector<std::string> >& evalStrings = pd.evalStrings;
    productParameterEvaluators.resize(evalStrings.size());
    for(unsigned int i = 0;i<evalStrings.size();i++)
    {
        productParameterEvaluators[i].resize(evalStrings[i].size());
        for(unsigned int j = 0; j<evalStrings[i].size();j++)
        {
            productParameterEvaluators[i][j] = ev.load(evalStrings[i][j], varIndiceMap, arrayDepth,
                pd.rawStatement);
        }
    }
}

void ParametricProduct::apply(LSentence &lsentence, const float * V)
{
    for(unsigned int i = 0;i<product.size();++i)
    {
        unsigned int numParams = productParameterEvaluators[i].size();

        lsentence.push_back(product[i], numParams);
        for(unsigned int j = 0; j < numParams; ++j)
            lsentence.push_back(productParameterEvaluators[i][j]->evaluate(V));
    }
}

//eval loader, and arrayDepth
//vector of productDatas, to production... .... ???
BasicParametricProduction::BasicParametricProduction(const ProductionData &ppd, const Alphabet &pnm, EVAL::Loader &ev, const unsigned int arrayDepth)
                                                    : BasicProduction(this)
{   
    bool isConditional = false;
    bool isVariableWeight = false;
    ParametricProduct* temp;
    for(ProductData pd : ppd.products)
    {
        temp = new ParametricProduct(pd, pnm, pd.varIndiceMap, ev, arrayDepth);
        m_products.push_back(temp);
        if(pd.conditional.size()>0)
            isConditional = true;
        if(!temp->fixedWeight())
            isVariableWeight = true;
    }
    if(m_products.size()==1)
    {
        if(isConditional)
            m_chooser = new ConditionalProductChooser(m_products);
        else
            m_chooser = new ProductChooser(m_products);
    }
    else
    {
        if(isVariableWeight)
        {
            if(isConditional)
                m_chooser = new VariableStochasticConditionalProductChooser(m_products);
            else
                m_chooser = new VariableStochasticProductChooser(m_products);
        }
        else
        {
            if(isConditional)
                m_chooser = new StochasticConditionalProductChooser(m_products);
            else
                m_chooser = new StochasticProductChooser(m_products);
        }
        
    }
}

BasicParametricProduction::~BasicParametricProduction()
{}

ParametricProduction::ParametricProduction(const ProductionData& ppd,
                     const Alphabet &pnm, const std::set<char> &skip,
                     EVAL::Loader &ev, const unsigned int arrayDepth)
                     :BasicParametricProduction(ppd,pnm, ev, arrayDepth),
                     ProductionContext(ppd.lContext,ppd.rContext,skip)
{
    //std::cout<<"Creating ParametricProduction"<<std::endl;
}

ParametricProduction::~ParametricProduction()
{
//    std::cout<<"Destroying ParametricProduction"<<std::endl;
}

ProductChooser* BasicParametricProduction::pass(const LSentence &lsentence, const unsigned int i, float * V, unsigned int arrayDepth)
{
    updateValHolder(lsentence,i,0, V, arrayDepth);
    return m_chooser;
}

ProductChooser* ParametricProduction::pass(const LSentence &lsentence, const unsigned int i, float * V, unsigned int arrayDepth)
{
    if(lContext.size() && (!passLContext(lsentence,i))) return nullptr;
    if(rContext.size() && (!passRContext(lsentence,i))) return nullptr;

    for(unsigned int n = 0;n<lContext.size();n++) updateValHolder(lsentence,lContextIndices[n], n, V, arrayDepth);
    updateValHolder(lsentence,i, lContext.size(), V, arrayDepth);
    for(unsigned int n = 0;n<rContext.size();n++) updateValHolder(lsentence,rContextIndices[n], n + 1 + lContext.size(), V, arrayDepth);

    return m_chooser;
}

//work on passing by reference stuff next
void BasicParametricProduction::updateValHolder(const LSentence &lsentence, const unsigned int sentenceIndex, const unsigned int valHolderIndex,
                                                float * V, unsigned int arrayDepth)
{
    for(int i = 0; i < lsentence[sentenceIndex].numParams; ++i)
    {
        V[valHolderIndex * arrayDepth + i] = lsentence[sentenceIndex+i+1].value;
    }
}

}//namespace LSYSTEM