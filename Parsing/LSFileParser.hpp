#ifndef LS_FILE_PARSER_HPP
#define LS_FILE_PARSER_HPP

#include "../Containers/LSFileData.hpp"
#include "../Containers/ProductionData.hpp"
#include <unordered_map>
#include <set>
//parse a LSentence ???
class LSParser
{
public:
    LSParser(){}
    //void load(const std::string &fileName);
    void parseFile(const std::string &fileName, LSYSTEM::LSFileData &fd, LSYSTEM::LSystemData &lsData);
private:
    void convertFileData(LSYSTEM::LSFileData &fd, LSYSTEM::LSystemData &lsData);
    void loadDefines(const std::vector<std::string> &defineStrings, std::unordered_map<std::string, std::string> &defineMapping);
    void loadGlobals(const std::vector<std::string> &globalStrings, std::vector<std::pair<char, float> > &globalPairs,
                    std::unordered_map<std::string, std::string> &defineMapping);
    void loadSkippableLetters(const std::string &skippableString, std::set<char> &skippableLetters);
    void decomposeProductionString(const std::string &productionString, LSYSTEM::ProductionData &pd);
    void loadProductionDatas(const std::vector<std::string> &productionStrings, std::vector<LSYSTEM::ProductionData> &productionDatas,
                                LSYSTEM::Alphabet &abc);
    void applyDefines(LSYSTEM::LSFileData &fd, const std::unordered_map<std::string, std::string> &defineMapping);
    bool stochasticMatch(std::vector<LSYSTEM::ProductionData>&, LSYSTEM::ProductionData&);
    void sortProductions(std::vector<LSYSTEM::ProductionData>& productions);
    void updateAlphabet(const std::string &axiom, LSYSTEM::Alphabet &abc);

    std::string getLetters(const std::string& sentence);

    void fillVarMap(const std::string&,uint&,VarIndiceMap&,LSYSTEM::Alphabet &pnm);
    std::string getProductEvalStrings(const std::string& rawProductString,std::vector<std::vector<std::string> >&, LSYSTEM::Alphabet &pnm);
};


#endif //LS_FILE_PARSER_HPP