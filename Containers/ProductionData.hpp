#ifndef PRODUCTION_DATA_HPP
#define PRODUCTION_DATA_HPP

#include "VarIndice.hpp"
#include "Alphabet.hpp"
#include "RPNToken.hpp"//hmm???
#include <string>
#include <vector>
#include <set>
//#include "LSentence.hpp"

namespace LSYSTEM
{

//we parse the file... it contains the LSDATA below... ... parse LSDATA -> LSParser
//Uses it to create the productionMapping in LSystem (new delete) and set its alphabet
//we parse the file... it contains Interpreter data... ... parse INTERPRETDATA -> InterpreterParser
//just one of each -potentially- for now
//we parse the file... it could contain Axiom String Data... -> loadLSentence function and set its alphabet
struct LSData;//and remove LSystemData as loaded in house, validated with the functions...

//can load from file, or personally, and put in LSystem loader...
//Fine, 'user-friendly' one
struct LSData
{//global mapping//constants??? defines???
    std::string label;
    std::vector<std::string> productions;
    std::vector<std::string> decompositions;
    std::vector<std::string> homomorphisms;
    std::string skippableLetters;
    std::unordered_map<char, float> globalMap;
    //std::vector<std::pair<std::string, float> > globals;
};

//Send to loader.  Let the 'evalLoader' worry about tokenizing it.  too complicated otherwise
struct ProductData
{
    std::string rawStatement;
    std::string product;//no parameters
    std::string productWeight;
    std::string conditional;
    std::vector< std::vector<std::string> > evalStrings;
    VarIndiceMap varIndiceMap;
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
    std::unordered_map<char, float> globalMap;
    Alphabet abc;
};

} // namespace LSYSTEM

#endif //PRODUCTION_DATA_HPP