#ifndef EVAL_LOADER_HPP
#define EVAL_LOADER_HPP

#include <map>
#include <vector>

#include <unordered_set>

#include "Evaluator.hpp"
#include "../Containers/RPNToken.hpp"
#include "../Containers/VarIndice.hpp"

namespace EVAL
{

//The Link between LSystem Productions and Evaluations... ???
//Provide the Evaluator *
//Request to update globals, getMaxStackSz, etc...
class Loader
{
public:
    Loader(){}
    virtual ~Loader();
    virtual void init(){}
    virtual void generate(){}
    virtual void close(){}
    //uses the VarIndiceMap 'container'
    virtual Evaluator* load(const std::string &expression, const VarIndiceMap &varMap, int maxVarDepth, const std::string &comment) = 0;
    Evaluator *getBasicEval(const std::string &exp, const float f);
protected:
/*
    //Parsing stuffs here
    //HMMM
    //Parsing to token form (RPNToken vector in infix notation) as a ... of productData
    //then send the tokenized expresssion in...
    //then we update the mapping -> implementation and simplify.  (or just send in the float* then anyways.. yeah)
    static std::vector<std::pair<std::string, char> > tokenStringReplaceVector;
    std::vector<RPNToken> tokenize(const std::string &expression, const VarIndiceMap &varMap);
    //bool hasGlobal;//tokenize() sets this true/false per 'evaluation'
    
    std::vector<std::pair<std::string, char> > &globalReplaceMap;
    std::map<char, float*> &globalMap;
    //or can just loop through the list and see if there is a global at all...
*/
    std::vector<Evaluator*> m_evaluators;
};

//this one and the other one that is 'Dependant' on the varindicemapping
class RuntimeLoader : public Loader
{
public:
    RuntimeLoader(const std::unordered_set<char> &_globalIds);
    //Evaluator* load(const std::string &expression, const RPNList &tokenizedExp, const std::string &comment) override;
    Evaluator* load(const std::string &expression, const VarIndiceMap &varMap, int maxVarDepth, const std::string &comment) override;
    void setOffset(uint offset);
    //call each ...
    void update(const std::map<char, float*> &globalMap);
    uint getMaxStackSz();
private:
    uint m_offset;
    uint m_maxStackSz;

    uint getMaxStackSz(const RPNList &rpnlist);

    const std::unordered_set<char> &globalIds;
    //RPNList tokenize(const std::string &expression, const VarIndiceMap &varMap, int maxVarDepth);
};

}//namespace EVAL

#endif //EVAL_LOADER_HPP