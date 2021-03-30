#ifndef LSYSTEM_HPP
#define LSYSTEM_HPP

#include "ParametricProduction.hpp"
#include "../Containers/ProductionData.hpp"
#include "LSInterpreter.hpp"

#include "../Parsing/ProductionParser.hpp"

//More as in production application functions
//cut symbol is a special production that doesn't lend itself to a product, more so a procedure
//productions & decompositions linked as a singular step
//homomorphisms used by interpreter

namespace LSYSTEM
{

typedef std::unordered_map<char, std::vector<BasicProduction*> > ProductionMap;

class LSystem
{
public:
    LSystem(const LSData &lsData);
    ~LSystem();
    void iterate(const VLSentence &sentence, VLSentence &newSentence);
    void interpret(VLSentence &sentence, LSInterpreter &I, LSReinterpreter &R);
    void interpret(VLSentence &sentence, LSInterpreter &I);

    void updateGlobal(char id, const float val);
    void update();

    const std::unordered_map<char,float> getGlobalMap();
private:
    Product * findMatch(const unsigned int i, const LSentence &refLS, std::unordered_map<char, std::vector<BasicProduction*> > &productionMap,
                        float *V, unsigned int *indiceHolder);
    void applyCut(const LSentence &oldSentence, unsigned int &i);
    void applyBasicProduct(const LSentence &oldSentence, LSentence &newSentence, const unsigned int curIndex);
    void applyProduct(const LSentence &oldSentence, LSentence &newSentence, unsigned int &curIndex,
                                std::unordered_map<char, std::vector<BasicProduction*> > &productMap, float *V, unsigned int *indiceHolder);
    void applyProductionRecursively(const LSentence &sentence, const unsigned int curIndex, LSentence &newSentence,
                                        ProductionMap &pm, float *V, unsigned int *indiceHolder);
    void decompose(const LSentence &undecomposedSentence, LSentence &newSentence, float *V, unsigned int *indiceHolder);
    
    std::unordered_map<char, std::vector<BasicProduction*> > m_productionMap;
    std::unordered_map<char, std::vector<BasicProduction*> > m_decompositionMap;
    std::unordered_map<char, std::vector<BasicProduction*> > m_homomorphismMap;

    void contract(LSYSTEM::VLSentence &vlsentence);

    Alphabet m_alphabet;//can't use a conflicting alphabet or the productions may try to write to a non-existant parameter

    EVAL::RuntimeLoader m_evalLoader;

    unsigned int m_maxDepth, m_maxWidth;//smallest dimensions of m_valArray to prevent access violations
    unsigned int m_maxStackSz;

    std::unordered_map<char, float> m_globalMap;

    void loadProductions(LSDataParser &lsdp);
    bool amSimple;
};

} // namespace LSYSTEM

#endif //LSYSTEM_HPP