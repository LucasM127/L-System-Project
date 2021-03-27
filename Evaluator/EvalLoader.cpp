#include "EvalLoader.hpp"
#include "Evaluator.hpp"

#include "../Logger/Logger.hpp"

namespace EVAL
{

//'linked' with the LSystem

Evaluator *Loader::getBasicEval(const std::string &exp, const float f)
{
    Evaluator * eval = new ConstEvaluator(exp, f);
    m_evaluators.push_back(eval);
    return eval;
}

Evaluator* RuntimeLoader::load(const std::string &expression, const VarIndiceMap &varMap, int maxVarDepth, const std::string &comment)
{
    RPNList rpnList;
    RPNList refList;
    shuntYardAlgorithm(tokenize(expression, varMap)); //more like a function that returns a rpnList in 'tree' form from tokenized expression
    //list is in m_rpnListStack.top()
    //bool hasGlobal
//have to simplify and expand to see what type we make, but also we need to preserve the old.
    deepCopy(m_rpnListStack.top(),refList);
    simplify();
    expand(rpnList);//deletes
    
    //Need the varMap, globalMap to convert our data over... ???
    //varMap for the index mapping. the 'new LVariable(varMap.at(T.token))
    //don't need the global map if globals stay confirmed.
    //but if they don't... !

    Evaluator * retEvalPtr;
    if(rpnList.size()==1)
    {
        const RPNToken &T = rpnList.back();
        if(rpnList.back().isConst)
            retEvalPtr = new ConstEvaluator(expression, T.value);
        else
            retEvalPtr = new SimpleEvaluator(expression, new LVariable(varMap.at(T.token),maxVarDepth));
        m_evaluators.push_back(retEvalPtr);
        return retEvalPtr;
    }
    //Is a complex expression...
    uint stackSz = getMaxStackSz(rpnList);//before conversion occurs but after simplified/expanded
    if(stackSz > m_maxStackSz) m_maxStackSz = stackSz;
    //convert from id values to actual implementations
    for(auto &T : rpnList)
    {
        if(T.isOp)
        {
            if(unaryOpMap.find(T.token) != unaryOpMap.end())
                T.fnPtr = unaryOpMap.at(T.token);
            else if(isAFunc(T.token))
                T.fnPtr = funcOpMap.at(T.token);
            else
                T.fnPtr = binaryOpMap.at(T.token);
        }
        else
        {
            if(T.isConst)
                T.numPtr = new Constant(T.value);
            else
                T.numPtr = new LVariable(varMap.at(T.token),maxVarDepth);
        }
    }
    retEvalPtr = new ComplexEvaluator(expression, rpnList, m_offset);
    LOG("New complex eval for exp: ",expression);
    m_evaluators.emplace_back(retEvalPtr);
    return retEvalPtr;
}

Loader::~Loader()
{
    for(auto eval : m_evaluators)
        delete eval;
}

RuntimeLoader::RuntimeLoader():Loader(), m_offset(0), m_maxStackSz(0){}

void RuntimeLoader::setOffset(uint offset)
{
    m_offset = offset;
}

uint RuntimeLoader::getMaxStackSz()
{
    return m_maxStackSz;
}


void RuntimeLoader::updateGlobals()
{
    //
}

} //namespace EVAL