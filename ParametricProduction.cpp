#include "ParametricProduction.hpp"

namespace LSYSTEM
{

ParametricProduct::ParametricProduct(const ProductData &pd, ParamNumMapping &pnm, VarIndiceMapping &varIndiceMap, EvalLoader &ev, const unsigned int arrayDepth)
                                    : Product(this)
{
    product = new PLSentence(pnm);
    for(auto c : pd.product) product->push_back(c);
    createProductEvaluations(pd, ev, varIndiceMap, arrayDepth);

    if(pd.productWeight.size())
        stochasticWeightEvaluator = ev.add(pd.productWeight, varIndiceMap, arrayDepth, pd.production->rawStatement);
    else
    {
        stochasticWeightEvaluator = ev.addBasicEvaluator();
        //stochasticWeightEvaluator = ev.add("1.f", varIndiceMap, arrayDepth);//create a function that returns 1.f
        //m_stochasticWeight = 1.f;
    }
    if(pd.conditionalExpression.size())
        conditionalEvaluator = ev.add(pd.conditionalExpression, varIndiceMap, arrayDepth, pd.production->rawStatement);
    else
        conditionalEvaluator = ev.addBasicEvaluator();//ev.add("1.f", varIndiceMap, arrayDepth);//or some default ... no need to 'add'
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

void ParametricProduct::createProductEvaluations(const ProductData& pd, EvalLoader &ev, VarIndiceMapping &varIndiceMap, const unsigned int arrayDepth)
{
    const std::vector<std::vector<std::string> >& evalStrings = pd.evalStrings;
    productParameterEvaluators.resize(evalStrings.size());
    for(unsigned int i = 0;i<evalStrings.size();i++)
    {
        productParameterEvaluators[i].resize(evalStrings[i].size());
        for(unsigned int j = 0; j<evalStrings[i].size();j++)
        {
            productParameterEvaluators[i][j] = ev.add(evalStrings[i][j], varIndiceMap, arrayDepth,
                pd.production->lContext + "<" + pd.production->letter + ">" + pd.production->rContext + ":" + pd.conditionalExpression
                    + "=>" + pd.product); 
                                                   // pd.production->rawStatement);
        }
    }
}//this depth thingy...

//THIS COULD BE AN ISSUE
//is shared state-> need to be more 'functional'
//FIX THE GET
const LSentence& ParametricProduct::get(const float * V)
{
    for(unsigned int i = 0;i<productParameterEvaluators.size();i++)
    {
        for(unsigned int j = 0;j<productParameterEvaluators[i].size();j++)
        {
            product->getVals(i)[j] = productParameterEvaluators[i][j]->evaluate(V);
        }
    }
    return *product;
}

//here we push it back onto the new sentence...
//that is not terrible
void ParametricProduct::apply(LSentence &newlsentence, const float * V)
{
    unsigned int sz = product->getSentence().size();
    int numParams;
    float* arrayVals;
//push_back and update the values...
    for(unsigned int i = 0;i<sz;++i)
    {
        arrayVals = newlsentence.push_back(product->getSentence()[i],numParams);
        for(int j = 0;j<numParams;j++) arrayVals[j] = productParameterEvaluators[i][j]->evaluate(V);
    }
}

//return first conditional product found that passes condition
Product* ConditionalProductChooser::choose(const float *V)
{
    for(Product *P : products) if(P->isValid(V)) return P;
    return nullptr;
}

//Fix rand()
Product* StochasticConditionalProductChooser::choose(const float *V)
{
    validProducts.clear();
    int runningTotal = 0;
    for(Product *P : products)
        if(P->isValid(V))
    {
        runningTotal += (int)P->calcWeight(V);
        validProducts.push_back(P);
    }
    if(validProducts.size()==0) return nullptr;
//ASSUMED that weights are non-zero
    int randomNum = rand()%runningTotal;
    runningTotal = 0;
    for(unsigned int i = 0;i<validProducts.size();i++)
    {
        runningTotal += (int)validProducts[i]->getWeight();//though if is a friend won't have to do this
        if(runningTotal > randomNum) return validProducts[i];
    }

    return nullptr;
}

Product* VariableStochasticProductChooser::choose(const float *V)
{
    int runningTotal = 0;
    for(Product *P : products) runningTotal += (int)P->calcWeight(V);

    int randomNum = rand()%runningTotal;
    runningTotal = 0;
    for(unsigned int i = 0;i<products.size();i++)
    {
        runningTotal += (int)products[i]->getWeight();
        if(runningTotal > randomNum) return products[i];
    }
    return nullptr;
}

//eval loader, and arrayDepth
BasicParametricProduction::BasicParametricProduction(ProductionData &ppd, ParamNumMapping &pnm, EvalLoader &ev, const unsigned int arrayDepth)
                                                    : BasicProduction(this)
{//FIX ME!
    //Load the products into the productions and determine kind of chooser needed...
    bool isConditional = false;
    bool isWeighted = false;
    ParametricProduct* temp;
    for(ProductData pd : ppd.products)
    {
        if(pd.conditionalExpression.size()>0) isConditional = true;
        if(pd.productWeight.size()>0)
        {
            isWeighted = true;
            //HOW test if is a static weight or a variable weight... got the varIndiceMap... test the evaluator?
        }
        temp = new ParametricProduct(pd, pnm, ppd.varIndiceMap, ev, arrayDepth);
        m_products.push_back(temp);
    }
    if(isConditional)
    {
        if(isWeighted)m_chooser = new StochasticConditionalProductChooser(m_products);
        else m_chooser = new ConditionalProductChooser(m_products); //Just Assume the conditionals are exclusive unless we write down a weight explicitly
    }
    else if(m_products.size()>1)
    {
        //if(isVariableWeights) chooser = new VariableStochasticProductChooser(products);
        m_chooser = new StochasticProductChooser(m_products);//unless it is variable weights... another flag
    }
    else m_chooser = new ProductChooser(m_products);//the deterministic simplistic case, no testing
}

BasicParametricProduction::~BasicParametricProduction()
{}

ParametricProduction::ParametricProduction(ProductionData& ppd,
                     ParamNumMapping &pnm, std::set<char> &skip,
                     EvalLoader &ev, const unsigned int arrayDepth)
                     :BasicParametricProduction(ppd,pnm, ev, arrayDepth),
                     ProductionContext(ppd.lContext,ppd.rContext,skip)
{
    //std::cout<<"Creating ParametricProduction"<<std::endl;
}

ParametricProduction::~ParametricProduction()
{
//    std::cout<<"Destroying ParametricProduction"<<std::endl;
}

ProductChooser* BasicParametricProduction::pass(LSentence *const lsentence, const unsigned int i, float * V, unsigned int arrayDepth)
{
    updateValHolder(lsentence,i,0, V, arrayDepth);
    return m_chooser;
}

ProductChooser* ParametricProduction::pass(LSentence *const lsentence, const unsigned int i, float * V, unsigned int arrayDepth)
{
    if(lContext.size())if(!passLContext(lsentence->getSentence(),i)) return nullptr;
    if(rContext.size())if(!passRContext(lsentence->getSentence(),i)) return nullptr;

    for(unsigned int n = 0;n<lContext.size();n++) updateValHolder(lsentence,lContextIndices[n], n, V, arrayDepth);
    updateValHolder(lsentence,i, lContext.size(), V, arrayDepth);
    for(unsigned int n = 0;n<rContext.size();n++) updateValHolder(lsentence,rContextIndices[n], n + 1 + lContext.size(), V, arrayDepth);

    return m_chooser;
}

//work on passing by reference stuff next
void BasicParametricProduction::updateValHolder(LSentence *const lsentence, const unsigned int sentenceIndex, const unsigned int valHolderIndex,
                                                float * V, unsigned int arrayDepth)
{
    int numParams;
    float* arrayVals= lsentence->getVals(sentenceIndex,numParams);
    for(int i = 0;i<numParams;i++) V[valHolderIndex * arrayDepth + i] = arrayVals[i];//valueHolder[valHolderIndex][i] = arrayVals[i];
}

}//namespace LSYSTEM