#ifndef PRODUCTION_DATA_HPP
#define PRODUCTION_DATA_HPP

#include "VarIndice.hpp"
#include "Alphabet.hpp"
#include <string>
#include <vector>
#include <set>
//#include "LSentence.hpp"

namespace LSYSTEM
{
/*
struct ProductionData;

struct ProductionRawStringData
{
    std::string lContext;
    std::string predicate;
    std::string rContext;
    std::string conditional;
    std::string product;
    std::string stochasticWeight;
};
*/
struct ProductData
{
    std::string rawStatement;
    std::string product;
    std::string productWeight;
    std::string conditional;
    std::vector< std::vector<std::string> > evalStrings;
    VarIndiceMap varIndiceMap;//send this to the 'ProductData' side
};

struct ProductionData
{
    std::string lContext;
    std::string letter;
    std::string rContext;
    std::vector<ProductData> products;
};

//typedef std::unordered_map<char, unsigned int> Alphabet;
//uses LSystemData to create the Lsystem... or PLSystem (if parametric)
//parametric = pnm.size()
struct LSystemData
{
    std::set<char> skippableLetters;
    std::vector<ProductionData> productionDatas;
    std::vector<ProductionData> decompositionProductionDatas;
    std::vector<ProductionData> homomorphicProductionDatas;
    Alphabet abc;
};

} // namespace LSYSTEM

#endif //PRODUCTION_DATA_HPP