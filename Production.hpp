#ifndef PRODUCTION_HPP
#define PRODUCTION_HPP

#include "LSentence.hpp"
#include "Containers.hpp"
#include <set>

namespace LSYSTEM
{

class ParametricProduct;

class Product;

void adjustWeights(std::vector<Product*>& products);

class Product
{
    friend void adjustWeights(std::vector<Product*>& products);
public:
    Product(const std::string &productString, float w = 100.f);
    Product(ParametricProduct *P){}
    virtual ~Product(){delete product;}
    virtual const LSentence &get(const float * V = nullptr);
    virtual void apply(LSentence &newLSentence, const float * V = nullptr);
    virtual bool isValid(const float * V){return true;}
    const float getWeight(){return m_stochasticWeight;}
    virtual const float calcWeight(const float * V){return m_stochasticWeight;}
    const std::string productString;
protected:
    LSentence *product;
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
    StochasticProductChooser(std::vector<Product*>& _products):ProductChooser(_products)
    {
        //adjustWeights(products);
    }
    Product *choose(const float *V) override;
};

class BasicParametricProduction;

class BasicProduction
{
public:
    BasicProduction(const std::vector<ProductData> &pds);
    BasicProduction(BasicParametricProduction *P){}
    virtual ~BasicProduction();
    virtual ProductChooser *pass(LSentence *const lsentence, const unsigned int i, float * V = nullptr, unsigned int arrayDepth = 0){return m_chooser;}
    const std::string sentence;
    Product *getProduct(){return m_chooser->choose(nullptr);}
protected:
    std::vector<Product*> m_products;
    ProductChooser *m_chooser;
};

//more functional !
bool isSkippable(char, const std::set<char> &skippableLetters);
unsigned int getLastLetter(const std::vector<char> &lsentence, const unsigned int i, const std::set<char> &skippableLetters);

class ProductionContext
{
public:
    ProductionContext(const std::string &lc, const std::string &rc, const std::set<char> &_skippableLetters);
    virtual ~ProductionContext();
protected:
    const std::string lContext;
    const std::string rContext;
    unsigned int* lContextIndices;
    unsigned int* rContextIndices;
    bool passLContext(const std::vector<char> &lsentence, const unsigned int i);
    bool passRContext(const std::vector<char> &lsentence, const unsigned int i);
    const std::set<char>& skippableLetters;
    //bool isSkippable(char);
};

class Production : public BasicProduction, public ProductionContext
{
public:
    Production(const std::vector<ProductData> &pds, const std::string &lc, const std::string &rc, const std::set<char> &_skippableLetters);
    ~Production();
    ProductChooser *pass(LSentence *const lsentence, const unsigned int i, float * V, unsigned int arrayDepth) override;
};

} // namespace LSYSTEM

#endif