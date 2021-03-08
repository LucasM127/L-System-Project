#ifndef LSYSTEM_HPP
#define LSYSTEM_HPP

#include "ParametricProduction.hpp"
#include "LSInterpreter.hpp"
//#include "EnvironmentalProgram.hpp"

namespace LSYSTEM
{
//just a set of production rules is a lsystem thing
struct Letter
{
    char id;
    float * vals;
    unsigned int numVals;
};

typedef std::unordered_map<char, std::vector<BasicProduction*> > ProductionMapping;

class PLSystem;
//iterate...
//apply homomorphic productions... Separate ? function
//Multiple LSystems based on same production rules...
//Functional more...
//apply decomposition productions.... (boolean???)
//Iterate(LSentence &sentence, uint nCount); -> create v* buffer internally to the function, for multithread safety, each thread, own buffer.
//and for the homomorphic productions???
//Can LSInterpreter.feed(char, variables) directly
//or for each letter, feed the homomorphic production of (char, variables)
//don't need the axiom then. but saave the max v* size for later!

class LSystem
{
public:
    LSystem(const std::string& axiom, std::set<char> &skip,
                      std::vector<ProductionData> &productionDatas,
                      std::vector<ProductionData> &decompositionProductionDatas,
                      std::vector<ProductionData> &homomorphicProductionDatas);
    LSystem(PLSystem *PL, const std::string& axiom, std::set<char> &skip);
    virtual ~LSystem();
    void iterate();
    void reset();
    virtual void feed(LSInterpreter &T);
//    virtual void feed(TurtleGL &T, EnvironmentalProgram &E);
//    virtual void link(EnvironmentalProgram &E);
protected:
    LSentence *lsentenceA;
    LSentence *lsentenceB;
    LSentence *oldSentence, *newSentence;

    std::unordered_map<char, std::vector<BasicProduction*> > productions;
    std::unordered_map<char, std::vector<BasicProduction*> > decompositionProductions;
    std::unordered_map<char, std::vector<BasicProduction*> > homomorphicProductions;

    int m_it_ctr;
    bool atMaxIteration;
    const std::string m_axiomString;

    Product * findMatch(const unsigned int i, LSentence * refLS, std::unordered_map<char, std::vector<BasicProduction*> > &productionMap);
    void applyProduct(const unsigned int i);
    virtual void applyHomomorphicProduction(LSInterpreter &T, LSentence*, const unsigned int i);
    void decompose();
    virtual void applyDecompositionProduction(LSentence *, const unsigned int i);

    float * V;
    unsigned int m_arrayDepth;

    std::set<char> m_skippableLetters;
    void applyCut(unsigned int &i);
//Environmental without parameters?  I don't see why not.
};

class PLSystem : public LSystem
{
public:
    PLSystem(const std::string& axiom, std::set<char> &skip,
                      std::vector<ProductionData> &productionDatas,
                      std::vector<ProductionData> &decompositionProductionDatas,
                      std::vector<ProductionData> &homomorphicProductionDatas,
                      ParamNumMapping &pnm);
    ~PLSystem();
private:
    ParamNumMapping m_pnm;

    EvalLoader *m_evalLoader;

    void applyHomomorphicProduction(LSInterpreter &T, LSentence*, const unsigned int i) override;
    void applyDecompositionProduction(LSentence *, const unsigned int i) override;
};

} // namespace LSYSTEM

#endif //LSYSTEM_HPP