#ifndef PARAMETRIC_PRODUCTION_HPP
#define PARAMETRIC_PRODUCTION_HPP

#include "Production.hpp"

#include "../Evaluator/EvalLoader.hpp"

namespace LSYSTEM
{

class BasicParametricProduction:public BasicProduction
{
public:
    BasicParametricProduction(const ProductionData&, EVAL::Loader &ev, const unsigned int arrayDepth);
    virtual ~BasicParametricProduction();

    virtual Product* pass(const LSentence &lsentence, const unsigned int i, float * V, unsigned int *indiceHolder) override;
protected:
    void updateValHolder(const LSentence &lsentence, const unsigned int sentenceIndice, const unsigned int valHolderIndice, float * V);
};

class ParametricProduction:public BasicParametricProduction, public ProductionContext
{
public:
    ParametricProduction(const ProductionData&, EVAL::Loader &ev, const unsigned int arrayDepth,
                          const std::set<char>& skip);
    ~ParametricProduction();

    Product* pass(const LSentence &lsentence, const unsigned int i, float * V, unsigned int *indiceHolder) override;
};

}// namespace LSYSTEM

#endif //PARAMETRIC_PRODUCTION_HPP