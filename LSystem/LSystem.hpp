#ifndef LSYSTEM_HPP
#define LSYSTEM_HPP

#include "ParametricProduction.hpp"
#include "../Containers/ProductionData.hpp"

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
    LSystem(const LSystemData &lsData);
    ~LSystem();
    void iterate(const LSentence &sentence, LSentence &newSentence);
    void applyHomomorphisms(const LSentence &sentence, LSentence &newSentence);
    void applyHomomorphisms(const LSentence &sentence, unsigned int i, LSentence &product);
    const Alphabet alphabet;//can't use a conflicting alphabet or the productions may try to write to a non-existant parameter
    const std::set<char> skippableLetters;//for context matching, cut symbol cut off branch?
private:
    void applyCut(const LSentence &oldSentence, LSentence &newSentence, unsigned int &i);
    Product * findMatch(const unsigned int i, const LSentence &refLS, std::unordered_map<char, std::vector<BasicProduction*> > &productionMap, float *V);
    void applyProduct(const LSentence &oldSentence, LSentence &newSentence, unsigned int &curIndex,
                                std::unordered_map<char, std::vector<BasicProduction*> > &productMap, float *V);
    void applyProductionRecursively(const LSentence &sentence, const unsigned int curIndex, LSentence &newSentence,
                                        ProductionMap &pm, float *V);
    void decompose(const LSentence &undecomposedSentence, LSentence &newSentence, float *V);
    void decompose(const LSentence &undecomposedSentence, LSentence &newSentence, const unsigned int curIndex, float *V);
    void applyBasicProduct(const LSentence &oldSentence, LSentence &newSentence, const unsigned int curIndex);
    std::unordered_map<char, std::vector<BasicProduction*> > m_productionMap;
    std::unordered_map<char, std::vector<BasicProduction*> > m_decompositionMap;
    std::unordered_map<char, std::vector<BasicProduction*> > m_homomorphismMap;

    EvalLoader *m_evalLoader;//not necessarily 'owned' though is atm
    unsigned int m_maxDepth, m_maxWidth;//smallest dimensions of m_valArray to prevent access violations
    float *m_valArray;//utility, not multithreading safe, each thread should have own copy
};

} // namespace LSYSTEM

#endif //LSYSTEM_HPP