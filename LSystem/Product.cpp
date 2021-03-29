#include "Product.hpp"

namespace LSYSTEM
{

Product::Product(const ProductData &pd, EVAL::Loader &ev, const unsigned int arrayDepth)
                : m_product(pd.product), amParametric(false)
{
    createProductEvaluations(pd, ev, arrayDepth);
    if(pd.productWeight.size())
        stochasticWeightEvaluator = ev.load(pd.productWeight, pd.varIndiceMap, arrayDepth, pd.rawStatement);
    else
        stochasticWeightEvaluator = ev.getBasicEval(pd.rawStatement, 1.f);//default weight is 1
    if(pd.conditional.size())
        conditionalEvaluator = ev.load(pd.conditional, pd.varIndiceMap, arrayDepth, pd.rawStatement);
    else
        conditionalEvaluator = ev.getBasicEval(pd.rawStatement, 1.f);//for true
}

Product::~Product(){}

void Product::apply(LSentence &lsentence, float * V)
{//This is the main difference now. Not sure if is worth inheritance or even having this confusion
//With globals, the other shit needs to be recalculated if globals change, and if is const, uses a basic eval anyways so that's fine
//    if(amParametric)
//    {
        for(unsigned int i = 0;i<m_product.size();++i)
        {
            unsigned int numParams = productParameterEvaluators[i].size();

            lsentence.push_back(m_product[i], numParams);
            for(unsigned int j = 0; j < numParams; ++j)
                lsentence.push_back(productParameterEvaluators[i][j]->evaluate(V));
        }
//    }
//    else
//    {
//        for(auto c : m_product)
//        {
//            lsentence.push_back(c,0);
//        }
//    }
}

float Product::getWeight(float *V)
{
    return stochasticWeightEvaluator->evaluate(V);
}

bool Product::isValid(float *V)
{
    return conditionalEvaluator->evaluate(V);
}

void Product::createProductEvaluations(const ProductData &pd, EVAL::Loader &ev, const unsigned int arrayDepth)
{
    const std::vector<std::vector<std::string> >& evalStrings = pd.evalStrings;
    productParameterEvaluators.resize(evalStrings.size());
    for(unsigned int i = 0;i<evalStrings.size();i++)
    {
        productParameterEvaluators[i].resize(evalStrings[i].size());
        for(unsigned int j = 0; j<evalStrings[i].size();j++)
        {
            amParametric = true;
            productParameterEvaluators[i][j] = ev.load(evalStrings[i][j], pd.varIndiceMap, arrayDepth,
                pd.rawStatement);
        }
    }
}

} // namespace LSYSTEM