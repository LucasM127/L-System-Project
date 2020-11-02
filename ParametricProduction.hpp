#ifndef PARAMETRIC_PRODUCTION_HPP
#define PARAMETRIC_PRODUCTION_HPP

#include "Production.hpp"

#include "Evaluator.hpp"

namespace LSYSTEM
{

//Is coming along, the more I remove the state from these functions,the more I can define them just as functions!
class ParametricProduct : public Product
{
public:
    ParametricProduct(const ProductData &pd, ParamNumMapping &pnm, VarIndiceMapping &varIndiceMap, EvalLoader &ev, const unsigned int arrayDepth);
    ~ParametricProduct(){}
    const LSentence &get(const float * V) override;
    void apply(LSentence &newLSentence, const float * V) override;
    const float calcWeight(const float *) override;
    bool isValid(const float *) override;
private:
    void createProductEvaluations(const ProductData &pd, EvalLoader &ev, VarIndiceMapping &varIndiceMap, const unsigned int arrayDepth);
    std::vector< std::vector<Evaluator*> > productParameterEvaluators;
    Evaluator* stochasticWeightEvaluator;
    Evaluator* conditionalEvaluator;
};

class ConditionalProductChooser:public ProductChooser//deterministic with conditionals
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
    std::vector<Product*> validProducts;//subset of products dependant on context and conditional
};

class VariableStochasticProductChooser:public ProductChooser//stochastic with unknown variable weights, no conditionals
{
public:
    VariableStochasticProductChooser(std::vector<Product*>& pds):ProductChooser(pds){}
    Product* choose(const float *V) override;
};

class BasicParametricProduction:public BasicProduction
{
public:
    BasicParametricProduction(ProductionData&, ParamNumMapping &pnm, EvalLoader &ev, const unsigned int arrayDepth);
    virtual ~BasicParametricProduction();

    virtual ProductChooser* pass(LSentence *const lsentence, const unsigned int i, float * V, unsigned int arrayDepth) override;
protected:
    void updateValHolder(LSentence *const lsentence, const unsigned int sentenceIndice, const unsigned int valHolderIndice, float * V, unsigned int arrayDepth);
};

class ParametricProduction:public BasicParametricProduction, public ProductionContext
{
public:
    ParametricProduction(ProductionData&, ParamNumMapping &pnm,
                          std::set<char>& skip, EvalLoader &ev, const unsigned int arrayDepth);
    ~ParametricProduction();

    ProductChooser* pass(LSentence *const lsentence, const unsigned int i, float * V, unsigned int arrayDepth) override;
};

}// namespace LSYSTEM

#endif //PARAMETRIC_PRODUCTION_HPP