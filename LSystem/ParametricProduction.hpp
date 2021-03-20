#ifndef PARAMETRIC_PRODUCTION_HPP
#define PARAMETRIC_PRODUCTION_HPP

#include "Production.hpp"

#include "../Evaluator/EvalLoader.hpp"

namespace LSYSTEM
{

class ParametricProduct : public Product
{
public:
    ParametricProduct(const ProductData &pd, const Alphabet &pnm, VarIndiceMap &varIndiceMap, EVAL::Loader &ev, const unsigned int arrayDepth);
    ~ParametricProduct(){}
    void apply(LSentence &lsentence, float * V) override;
    const float calcWeight(float *) override;
    bool isValid(float *) override;
    bool fixedWeight(){return stochasticWeightEvaluator->isConst;}
private:
    void createProductEvaluations(const ProductData &pd, EVAL::Loader &ev, VarIndiceMap &varIndiceMap, const unsigned int arrayDepth);
    std::vector< std::vector<EVAL::Evaluator*> > productParameterEvaluators;
    EVAL::Evaluator* stochasticWeightEvaluator;
    EVAL::Evaluator* conditionalEvaluator;
};

class BasicParametricProduction:public BasicProduction
{
public:
    BasicParametricProduction(const ProductionData&, const Alphabet &pnm, EVAL::Loader &ev, const unsigned int arrayDepth);
    virtual ~BasicParametricProduction();

    virtual ProductChooser* pass(const LSentence &lsentence, const unsigned int i, float * V, unsigned int arrayDepth) override;
protected:
    void updateValHolder(const LSentence &lsentence, const unsigned int sentenceIndice, const unsigned int valHolderIndice, float * V, unsigned int arrayDepth);
};

class ParametricProduction:public BasicParametricProduction, public ProductionContext
{
public:
    ParametricProduction(const ProductionData&, const Alphabet &pnm,
                          const std::set<char>& skip, EVAL::Loader &ev, const unsigned int arrayDepth);
    ~ParametricProduction();

    ProductChooser* pass(const LSentence &lsentence, const unsigned int i, float * V, unsigned int arrayDepth) override;
};

}// namespace LSYSTEM

#endif //PARAMETRIC_PRODUCTION_HPP