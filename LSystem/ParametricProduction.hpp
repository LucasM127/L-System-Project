#ifndef PARAMETRIC_PRODUCTION_HPP
#define PARAMETRIC_PRODUCTION_HPP

#include "Production.hpp"

#include "../Evaluator/EvalLoader.hpp"

namespace LSYSTEM
{

class ParametricProduct : public Product
{
public:
    ParametricProduct(const ProductData &pd, const Alphabet &pnm, VarIndiceMap &varIndiceMap, EvalLoader &ev, const unsigned int arrayDepth);
    ~ParametricProduct(){}
    void apply(LSentence &lsentence, const float * V) override;
    const float calcWeight(const float *) override;
    bool isValid(const float *) override;
    bool fixedWeight(){return stochasticWeightEvaluator->isConst;}
private:
    void createProductEvaluations(const ProductData &pd, EvalLoader &ev, VarIndiceMap &varIndiceMap, const unsigned int arrayDepth);
    std::vector< std::vector<Evaluator*> > productParameterEvaluators;
    Evaluator* stochasticWeightEvaluator;
    Evaluator* conditionalEvaluator;
};

class ConditionalProductChooser:public ProductChooser
{
public:
    ConditionalProductChooser(std::vector<Product*>& pds):ProductChooser(pds){}
    Product* choose(const float *V) override;
};

class StochasticConditionalProductChooser:public ProductChooser//stochastic with conditionals
{
public:
    StochasticConditionalProductChooser(std::vector<Product*>& pds):ProductChooser(pds){}//no need to prebalance the weights
    Product* choose(const float *V) override;
private:
    std::vector<Product*> m_validProducts;//subset of products dependant on context and conditional
};

class VariableStochasticProductChooser:public ProductChooser//stochastic with unknown variable weights, no conditionals
{
public:
    VariableStochasticProductChooser(std::vector<Product*>& pds):ProductChooser(pds){}
    Product* choose(const float *V) override;
};

class VariableStochasticConditionalProductChooser:public ProductChooser
{
public:
    VariableStochasticConditionalProductChooser(std::vector<Product*>& pds):ProductChooser(pds){}
    Product* choose(const float *V) override;
private:
    std::vector<Product*> m_validProducts;
};

class BasicParametricProduction:public BasicProduction
{
public:
    BasicParametricProduction(const ProductionData&, const Alphabet &pnm, EvalLoader &ev, const unsigned int arrayDepth);
    virtual ~BasicParametricProduction();

    virtual ProductChooser* pass(const LSentence &lsentence, const unsigned int i, float * V, unsigned int arrayDepth) override;
protected:
    void updateValHolder(const LSentence &lsentence, const unsigned int sentenceIndice, const unsigned int valHolderIndice, float * V, unsigned int arrayDepth);
};

class ParametricProduction:public BasicParametricProduction, public ProductionContext
{
public:
    ParametricProduction(const ProductionData&, const Alphabet &pnm,
                          const std::set<char>& skip, EvalLoader &ev, const unsigned int arrayDepth);
    ~ParametricProduction();

    ProductChooser* pass(const LSentence &lsentence, const unsigned int i, float * V, unsigned int arrayDepth) override;
};

}// namespace LSYSTEM

#endif //PARAMETRIC_PRODUCTION_HPP