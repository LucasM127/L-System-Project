#ifndef PRODUCTION_HPP
#define PRODUCTION_HPP

#include "../Containers/ProductionData.hpp"
#include "../Containers/LSentence.hpp"

#include "ProductChooser.hpp"

namespace LSYSTEM
{

class ParametricProduct;

class ProductChooser;

class Product
{
public:
    Product(const std::string &productString, float w = 1.f);
    Product(ParametricProduct *P, const std::string &_productString);
    virtual ~Product(){}
    virtual void apply(LSentence &lsentence, const float * V = nullptr);
    virtual bool isValid(const float * V);
    const float getWeight();
    virtual const float calcWeight(const float * V);
    const std::string product;
protected:
    float m_stochasticWeight;
};

class BasicParametricProduction;

class BasicProduction
{
public:
    BasicProduction(const std::vector<ProductData> &pds);
    BasicProduction(BasicParametricProduction *P){}
    virtual ~BasicProduction();
    virtual ProductChooser *pass(const LSentence &lsentence, const unsigned int i, float * V = nullptr, unsigned int arrayDepth = 0);
    const std::string sentence;
    Product *getProduct();
protected:
    std::vector<Product*> m_products;
    ProductChooser *m_chooser;
};
//More a class trait?
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

//bool isSkippable(char c, const std::set<char> &skippableLetters);

} // namespace LSYSTEM

#endif