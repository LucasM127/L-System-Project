#include "EvalLoader.hpp"
#include "Evaluator.hpp"

#include "../Logger/Logger.hpp"

namespace EVAL
{

Evaluator *Loader::getBasicEval(const std::string &exp, const float f)
{
    Evaluator * eval = new ConstEvaluator(exp, f);
    m_evaluators.push_back(eval);
    return eval;
}

Evaluator* RuntimeLoader::load(const std::string &expression, const VarIndiceMap &varMap, int maxVarDepth, const std::string &comment)
{
    RPNList rpnList;
    shuntYardAlgorithm(tokenize(expression, varMap));
    simplify();
    expand(rpnList);

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

bool Loader::isUnary(const char op)
{
    return (unaryOpMap.find(op) != unaryOpMap.end());
}

bool Loader::isAnOp(const char op)
{
    return 	(unaryOpMap.find(op) != unaryOpMap.end()) ||
            (binaryOpMap.find(op) != binaryOpMap.end()) ||
            (funcOpMap.find(op) != funcOpMap.end());
}

bool Loader::isAFunc(const char op)
{
    return (funcOpMap.find(op) != funcOpMap.end());
}

void Loader::simplify()
{
    simplify(m_rpnListStack.top());
}

void Loader::expand(RPNList &target)
{
    expand(m_rpnListStack.top(), target);
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

uint RuntimeLoader::getMaxStackSz(const RPNList &rpnlist)
{
    uint maxSz = 0;
    uint curSz = 0;
    for(auto T : rpnlist)
    {
        if(!T.isOp)
        {
            ++curSz;
            if(curSz > maxSz) maxSz = curSz;
        }
        else
        {
            if(!isUnary(T.token))
                --curSz;//currently only supports 2 parameters.. would be more if a function, but overkill is ok for now.
        }
    }
    return maxSz;
}

} //namespace EVAL