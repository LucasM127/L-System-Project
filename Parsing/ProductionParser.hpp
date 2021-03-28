#ifndef LS_PRODUCTION_PARSER_HPP
#define LS_PRODUCTION_PARSER_HPP

#include "../Containers/ProductionData.hpp"

//use by LSYSTEM constructor (loader?)... to validate, etc...
struct LSDataParser
{
    void parse(const LSYSTEM::LSData &lsData);
//globals applied????
    std::vector<LSYSTEM::ProductionData> productionDatas;
    std::vector<LSYSTEM::ProductionData> decompositionProductionDatas;
    std::vector<LSYSTEM::ProductionData> homomorphicProductionDatas;
    std::unordered_map<std::string, char> globalNameMap;//give globals a unique token id, to use by evaluator.
    std::unordered_map<char, float> globalVarMap;
    std::set<char> skippableLetters;
    LSYSTEM::Alphabet alphabet;
private:
    void mapGlobals(const std::vector<std::pair<std::string, float> > &globalPairs);
    void decomposeProductionString(const std::string &productionString, LSYSTEM::ProductionData &pd);
    void loadProductionDatas(const std::vector<std::string> &productionStrings, std::vector<LSYSTEM::ProductionData> &productionDatas,
                                LSYSTEM::Alphabet &abc);
    bool stochasticMatch(std::vector<LSYSTEM::ProductionData>&, LSYSTEM::ProductionData&);
    void sortProductions(std::vector<LSYSTEM::ProductionData>& productions);

//    void fillVarMap(const std::string&,uint&,VarIndiceMap&,LSYSTEM::Alphabet &pnm);
    std::string parseContextString(const std::string &contextString, uint &varIndex, VarIndiceMap& varIndiceMap, LSYSTEM::Alphabet &abc);
    std::string getProductEvalStrings(const std::string& rawProductString,std::vector<std::vector<EVAL::RPNList> >&,
                                        VarIndiceMap& varIndiceMap, LSYSTEM::Alphabet &pnm);

    void assertAlphabet();
};

#endif //LS_PRODUCTION_PARSER_HPP