#ifndef PRODUCTION_HPP
#define PRODUCTION_HPP

#include "../Containers/ProductionData.hpp"
#include "../Containers/LSentence.hpp"

#include "ProductChooser.hpp"
#include "Product.hpp"

namespace LSYSTEM
{

class ProductChooser;

class BasicProduction
{
public:
    BasicProduction(const ProductionData &ppd, EVAL::Loader &ev, const unsigned int arrayDepth);
    virtual ~BasicProduction();
    virtual Product *pass(const LSentence &lsentence, const unsigned int i, float * V, unsigned int *indiceHolder);
    void update();
protected:
    std::vector<Product> m_products;
    ProductChooser *m_chooser;
    unsigned int m_arrayDepth;
};

//More a class trait?
class ProductionContext
{
public:
    ProductionContext(const std::string &lc, const std::string &rc, const std::set<char> &_skippableLetters);
    virtual ~ProductionContext();
    bool passLContext(const LSentence &lsentence, const unsigned int i, unsigned int *lContextIndices);
    bool passRContext(const LSentence &lsentence, const unsigned int i, unsigned int *rContextIndices);
protected:
    const std::string lContext;
    const std::string rContext;
    const std::set<char>& skippableLetters;
};

class Production : public BasicProduction, public ProductionContext
{
public:
    Production(const ProductionData &ppd, EVAL::Loader &ev, const unsigned int arrayDepth,
                const std::set<char> &_skippableLetters);
    ~Production();
    Product *pass(const LSentence &lsentence, const unsigned int i, float * V, unsigned int *indiceHolder) override;
};

} // namespace LSYSTEM

#endif