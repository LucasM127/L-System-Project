#include "ProductChooser.hpp"
#include "Production.hpp"

namespace LSYSTEM
{

ProductChooser::ProductChooser(std::vector<Product> &_products):products(_products)
{}

StochasticProductChooser::StochasticProductChooser(std::vector<Product>& _products):ProductChooser(_products), m_weightTotal(0.f)
{
    update();
}

ConditionalProductChooser::ConditionalProductChooser(std::vector<Product>& pds):ProductChooser(pds)
{}

StochasticConditionalProductChooser::StochasticConditionalProductChooser(std::vector<Product>& pds):ProductChooser(pds)
{}//no need to prebalance the weights

VariableStochasticProductChooser::VariableStochasticProductChooser(std::vector<Product>& pds):ProductChooser(pds)
{}

VariableStochasticConditionalProductChooser::VariableStochasticConditionalProductChooser(std::vector<Product>& pds):ProductChooser(pds)
{}

Product* ProductChooser::choose(float *V){return &products[0];}

void StochasticProductChooser::update()
{
    for(Product &P : products)
        m_weightTotal += P.getWeight(nullptr);
}

//total is constant...
Product* StochasticProductChooser::choose(float *V)
{   
    float randomNum = (float(rand())/float(RAND_MAX))*m_weightTotal;
    float runningTotal = 0.f;
    for(Product &P : products)
    {
        runningTotal += P.getWeight(V);
        if(runningTotal > randomNum) return &P;
    }
    return nullptr;
}

Product* ConditionalProductChooser::choose(float *V)
{
    if(products[0].isValid(V)) return &products[0];
    return nullptr;
}
//weights cannot be 'zero or negative'
Product* StochasticConditionalProductChooser::choose(float *V)
{
    m_validProducts.clear();
    float runningTotal = 0;
    for(Product &P : products)
        if(P.isValid(V))
    {
        runningTotal += P.getWeight(V);
        m_validProducts.push_back(&P);
    }
    if(m_validProducts.size()==0) return nullptr;

    float randomNum = float(rand())/float(RAND_MAX)*runningTotal;
    runningTotal = 0;
    for(unsigned int i = 0;i<m_validProducts.size();++i)
    {//Yeah... I don't know about this one either.
        runningTotal += m_validProducts[i]->getWeight(V);//getWeight();
        if(runningTotal > randomNum) return m_validProducts[i];
    }

    return nullptr;
}

Product* VariableStochasticProductChooser::choose(float *V)
{
    float runningTotal = 0;
    for(Product &P : products) runningTotal += P.getWeight(V);

    float randomNum = float(rand())/float(RAND_MAX)*runningTotal;
    runningTotal = 0;
    for(unsigned int i = 0;i<products.size();++i)
    {
        runningTotal += products[i].getWeight(V);
        if(runningTotal > randomNum) return &products[i];
    }
    return nullptr;
}

Product* VariableStochasticConditionalProductChooser::choose(float *V)
{
    m_validProducts.clear();
    float runningTotal = 0;
    for(Product &P : products)
        if(P.isValid(V))
    {
        runningTotal += P.getWeight(V);
        m_validProducts.push_back(&P);
    }
    if(m_validProducts.size()==0) return nullptr;

    float randomNum = float(rand())/float(RAND_MAX)*runningTotal;
    runningTotal = 0;
    for(unsigned int i = 0;i<m_validProducts.size();++i)
    {
        runningTotal += m_validProducts[i]->getWeight(V);
        if(runningTotal > randomNum) return m_validProducts[i];
    }

    return nullptr;
}

} // namespace LSYSTEM