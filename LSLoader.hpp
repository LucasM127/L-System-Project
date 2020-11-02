#ifndef LSLOADER_HPP
#define LSLOADER_HPP

#include <fstream>
#include <string>
#include "LSentence.hpp"
#include "Containers.hpp"
#include <set>
#include "LSystem.hpp"

namespace LSYSTEM
{

class LSystem;
//load the file, parse it -> separate sub parsers...
class LSLoader
{
public:
    LSLoader();
    LSystem *loadFile(const std::string &fileName);
private:
    std::ifstream m_stream;
    const std::string m_taglines[5] = {"Axiom:", "Skippable Letters:", "Productions:", "Decomposition Productions:", "Homomorphic Productions:"};
    std::string m_tempLine;
    int m_tagIndex;
    std::string m_axiom;
    std::set<char> m_skippableLetters;
    std::vector<std::string> m_productionStrings;
    std::vector<std::string> m_homomorphicProductionStrings;
    std::vector<std::string> m_decompositionProductionStrings;
    std::unordered_map<std::string,std::string> m_defineMapping;
    ParamNumMapping m_paramNumMap;
    bool amParametric;
    int m_lineNum;

    bool getNextLine();
    void applyDefines(std::string& source);
    void removeSpaces(std::string&);
    void assertTagLine(const std::string& tagLine);
    void prepSyntax();
    void ensureBracketsMatch(const std::string& sentence);
    void getProductionLines(std::vector<std::string>&);

    void getProductionData(const std::string& productionString, ProductionData& pd);
    void getParametricProductionData(const std::string& productionString, ProductionData& ppd);
    bool stochasticMatch(std::vector<ProductionData>&, ProductionData&);
    void sortProductions(std::vector<ProductionData>&);

    std::string getLetters(const std::string& in);
    void fillVarMap(const std::string&,int&, VarIndiceMapping&);
    void getProductEvalStrings(const std::string& rawProductString,std::vector<std::vector<std::string> >&);
    void fillParamNumMap(const std::string& string);
};

} //namespace LSYSTEM

#endif //LSLOADER_HPP