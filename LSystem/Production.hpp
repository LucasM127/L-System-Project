#ifndef PRODUCTION_HPP
#define PRODUCTION_HPP

#include "../Containers/ProductionData.hpp"
#include "LSentence.hpp"

namespace LSYSTEM
{

class ParametricProduct;

class Product;

class Product
{
public:
    Product(const std::string &productString, float w = 1.f);
    Product(ParametricProduct *P, const std::string &_productString);
    virtual ~Product(){}
    virtual void apply(LSentence &lsentence, const float * V = nullptr);
    virtual bool isValid(const float * V){return true;}
    const float getWeight(){return m_stochasticWeight;}
    virtual const float calcWeight(const float * V){return m_stochasticWeight;}
    const std::string product;
protected:
    float m_stochasticWeight;
};

class ProductChooser
{
public:
    ProductChooser(std::vector<Product*> &_products):products(_products){}
    virtual ~ProductChooser(){}
    virtual Product* choose(const float *V){return products[0];}
protected:
    std::vector<Product*> &products;
};

class StochasticProductChooser: public ProductChooser
{
public:
    StochasticProductChooser(std::vector<Product*>& _products);
    Product *choose(const float *V) override;
protected:
    float m_weightTotal;
};

class BasicParametricProduction;

class BasicProduction
{
public:
    BasicProduction(const std::vector<ProductData> &pds);
    BasicProduction(BasicParametricProduction *P){}
    virtual ~BasicProduction();
    virtual ProductChooser *pass(const LSentence &lsentence, const unsigned int i, float * V = nullptr, unsigned int arrayDepth = 0){return m_chooser;}
    const std::string sentence;
    Product *getProduct(){return m_chooser->choose(nullptr);}
protected:
    std::vector<Product*> m_products;
    ProductChooser *m_chooser;
};
//use to fill var holder
class ProductionContext
{
public:
    ProductionContext(const std::string &lc, const std::string &rc, const std::set<char> &_skippableLetters);
    virtual ~ProductionContext();
    bool passLContext(const LSentence &lsentence, const unsigned int i);
    bool passRContext(const LSentence &lsentence, const unsigned int i);
protected:
//Pass it a varHolder??? worry about that Later...
    unsigned int* lContextIndices;
    unsigned int* rContextIndices;
    const std::string lContext;
    const std::string rContext;
    const std::set<char>& skippableLetters;
};

class Production : public BasicProduction, public ProductionContext
{
public:
    Production(const std::vector<ProductData> &pds, const Alphabet &abc, const std::string &lc, const std::string &rc, const std::set<char> &_skippableLetters);
    ~Production();
    ProductChooser *pass(const LSentence &lsentence, const unsigned int i, float * V, unsigned int arrayDepth) override;
};

//or lambda it???
bool isSkippable(char c, const std::set<char> &skippableLetters);

} // namespace LSYSTEM

#endif