#ifndef CONTAINERS_HPP
#define CONTAINERS_HPP

#include <string>
#include <vector>
#include "Evaluator.hpp"

namespace LSYSTEM
{

struct ProductionData;

struct ProductData
{
    std::string product;
    std::string productWeight;
    std::string conditionalExpression;
    std::vector< std::vector<std::string> > evalStrings;
    ProductionData *production;
};

struct ProductionData
{
    virtual ~ProductionData(){}
    std::string rawStatement;
    std::string lContext;
    std::string letter;
    std::string rContext;
    std::vector<ProductData> products;
    
    VarIndiceMapping varIndiceMap;
};

} // namespace LSYSTEM

#endif //CONTAINERS_HPP