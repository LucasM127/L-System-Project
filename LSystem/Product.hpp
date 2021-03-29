#ifndef LSYSTEM_PRODUCT_HPP
#define LSYSTEM_PRODUCT_HPP

#include "../Containers/ProductionData.hpp"
#include "../Containers/LSentence.hpp"

#include "../Evaluator/EvalLoader.hpp"

namespace LSYSTEM
{

//only one type of 'Product'
class Product
{
public:
    Product(const ProductData &pd, EVAL::Loader &ev, const unsigned int arrayDepth);
    ~Product();
    void apply(LSentence &lsentence, float * V);
    float getWeight(float *V);
    bool isValid(float *V);
    void update();
    bool isFixedWeight(){return stochasticWeightEvaluator->isConst;}
private:
    void createProductEvaluations(const ProductData &pd, EVAL::Loader &ev, const unsigned int arrayDepth);
    std::vector< std::vector<EVAL::Evaluator*> > productParameterEvaluators;
    EVAL::Evaluator* stochasticWeightEvaluator;
    EVAL::Evaluator* conditionalEvaluator;
    const std::string m_product;
    bool amParametric;
};

} // namespace LSYSTEM

#endif //LSYSTEM_PRODUCT_HPP