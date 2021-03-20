#ifndef PRODUCT_CHOOSER_HPP
#define PRODUCT_CHOOSER_HPP

#include "Production.hpp"

namespace LSYSTEM
{

class Product;

class ProductChooser
{
public:
    ProductChooser(std::vector<Product*> &_products);
    virtual ~ProductChooser(){}
    virtual Product* choose(float *V);
protected:
    std::vector<Product*> &products;
};

class StochasticProductChooser: public ProductChooser
{
public:
    StochasticProductChooser(std::vector<Product*>& _products);
    Product *choose(float *V) override;
protected:
    float m_weightTotal;
};


class ConditionalProductChooser:public ProductChooser
{
public:
    ConditionalProductChooser(std::vector<Product*>& pds);
    Product* choose(float *V) override;
};

class StochasticConditionalProductChooser:public ProductChooser//stochastic with conditionals
{
public:
    StochasticConditionalProductChooser(std::vector<Product*>& pds);
    Product* choose(float *V) override;
private:
    std::vector<Product*> m_validProducts;//subset of products dependant on context and conditional
};

class VariableStochasticProductChooser:public ProductChooser//stochastic with unknown variable weights, no conditionals
{
public:
    VariableStochasticProductChooser(std::vector<Product*>& pds);
    Product* choose(float *V) override;
};

class VariableStochasticConditionalProductChooser:public ProductChooser
{
public:
    VariableStochasticConditionalProductChooser(std::vector<Product*>& pds);
    Product* choose(float *V) override;
private:
    std::vector<Product*> m_validProducts;
};

} // namespace LSYSTEM

#endif //PRODUCT_CHOOSER_HPP