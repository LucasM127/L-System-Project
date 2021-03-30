#ifndef LS_PRODUCTION_PARSER_HPP
#define LS_PRODUCTION_PARSER_HPP

#include "../Containers/ProductionData.hpp"

//Convert from LSData to LSystemData (Stolen from by LSystem upon creation)
//Throw away class
struct LSDataParser
{
    void parse(const LSYSTEM::LSData &lsData);
    LSYSTEM::LSystemData data;
private:
    void decomposeProductionString(const std::string &productionString, LSYSTEM::ProductionData &pd);
    void loadProductionDatas(const std::vector<std::string> &productionStrings, std::vector<LSYSTEM::ProductionData> &productionDatas,
                                LSYSTEM::Alphabet &abc);
    bool stochasticMatch(std::vector<LSYSTEM::ProductionData>&, LSYSTEM::ProductionData&);
    void sortProductions(std::vector<LSYSTEM::ProductionData>& productions);

    std::string parseContextString(const std::string &contextString, uint &varIndex, VarIndiceMap& varIndiceMap, LSYSTEM::Alphabet &abc);
    std::string getProductEvalStrings(const std::string& rawProductString,std::vector<std::vector<std::string> >&, LSYSTEM::Alphabet &abc);

    void assertAlphabet(const LSYSTEM::Alphabet &abc);
};

#endif //LS_PRODUCTION_PARSER_HPP