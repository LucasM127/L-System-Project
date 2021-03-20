#ifndef EVAL_LOADER_HPP
#define EVAL_LOADER_HPP

#include <map>
#include <vector>

#include "Evaluator.hpp"
#include "RPNToken.hpp"

namespace EVAL
{

//class RPNGenerator;
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
    inline const RPNList &list(){return m_rpnListStack.top();}
#ifdef EVAL_DEBUG
    void testDistribute();
#endif
protected:
    void simplify();
    void expand(RPNList &target);
    inline RPNList &get(){return m_rpnListStack.top();}
    void shuntYardAlgorithm(const std::vector<RPNToken> &tokenizedExpression);

    static std::map<char,opFnPtr> binaryOpMap;
    static std::map<char,opFnPtr> unaryOpMap;
    static std::map<char,opFnPtr> funcOpMap;
//private:
    static std::vector<std::pair<std::string, char> > tokenStringReplaceVector;
    static std::map<char, bool> opLeftAssociativityMap;
    static std::map<char, int> opPriorityMap;

    std::vector<RPNToken> tokenize(const std::string &expression, const VarIndiceMap &varMap);
    
    void evaluate(std::stack<char>& opStack);
    
    bool isUnary(const char op);
    bool isAnOp(const char op);
    bool isAFunc(const char op);

    void expand(RPNList &rpnList, RPNList &rpnListFinal);
    void simplify(RPNList& rpnList);
    void simplifyBinary(RPNList& rpnList);
    void simplifyUnary(RPNList& rpnList);
    void simplifyFunc(RPNList& rpnList);
    void distribute(RPNList &rpnList, int complexIt);
    //std::stack<float> m_floatStack;
    float m_floatStack[2];//just for simplifies only two operands

    std::stack< RPNList > m_rpnListStack;

    std::vector<Evaluator*> m_evaluators;
};
//this one and the other one that is 'Dependant' on the varindicemapping
class RuntimeLoader : public Loader
{
public:
    RuntimeLoader();
    Evaluator* load(const std::string &expression, const VarIndiceMap &varMap, int maxVarDepth, const std::string &comment) override;
    void setOffset(uint offset);
    uint getMaxStackSz();
private:
    uint m_offset;
    uint m_maxStackSz;

    uint getMaxStackSz(const RPNList &rpnlist);
};

}//namespace EVAL

#endif //EVAL_LOADER_HPP