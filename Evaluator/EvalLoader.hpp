#ifndef EVAL_LOADER_HPP
#define EVAL_LOADER_HPP

#include <map>
#include <vector>

#include "Evaluator.hpp"
#include "RPNToken.hpp"

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
    //Parsing stuffs here
    static std::vector<std::pair<std::string, char> > tokenStringReplaceVector;
    std::vector<RPNToken> tokenize(const std::string &expression, const VarIndiceMap &varMap);
    bool hasGlobal;//tokenize() sets this true/false per 'evaluation'

    std::vector<Evaluator*> m_evaluators;
};

//this one and the other one that is 'Dependant' on the varindicemapping
class RuntimeLoader : public Loader
{
public:
    RuntimeLoader();
    Evaluator* load(const std::string &expression, const VarIndiceMap &varMap, int maxVarDepth, const std::string &comment) override;
    void setOffset(uint offset);
    void updateGlobals();
    uint getMaxStackSz();
private:
    uint m_offset;
    uint m_maxStackSz;

    uint getMaxStackSz(const RPNList &rpnlist);
};

}//namespace EVAL

#endif //EVAL_LOADER_HPP