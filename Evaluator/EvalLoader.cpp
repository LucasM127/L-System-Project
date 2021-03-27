#include "EvalLoader.hpp"
#include "Evaluator.hpp"

#include "../Logger/Logger.hpp"
#include "ShuntYardAlgorithm.hpp"

#include "RPNListFuncs.hpp"

namespace EVAL
{

//'linked' with the LSystem

Evaluator *Loader::getBasicEval(const std::string &exp, const float f)
{
    Evaluator * eval = new ConstEvaluator(exp, {{f}}, false);
    m_evaluators.push_back(eval);
    return eval;
}

//call AFTER setOffset()
//Evaluator* RuntimeLoader::load(const std::string &expression, const VarIndiceMap &varMap, int maxVarDepth, const std::string &comment)
Evaluator *RuntimeLoader::load(const std::string &expression, const RPNList &tokenizedExp, const std::string &comment)
{
    //go through the tokenized list and see what type it resolves to...
    uint numGlobals = 0;
    uint numVars = 0;
    uint numVals = 0;
    for(auto &tok : tokenizedExp)
    {
        if(tok.type == RPNToken::TYPE::CONST)
            ++numVals;
        else if(tok.type == RPNToken::TYPE::GLOBAL)
            ++numGlobals;
        else if(tok.type == RPNToken::TYPE::VAR)
            ++numVars;    
    }

    ShuntYardAlgorithm SYA;
    RPNList &treeList = SYA.apply(tokenizedExp);

    //create appropiate Evaluator
    Evaluator * retEvalPtr;
    if(numVars == 0)//simplifies to a simple constant
        retEvalPtr = new ConstEvaluator(expression, std::move(treeList), numGlobals > 0);
    else if(numGlobals == 0 && numVals == 0 && numVars == 1)//just the one variable number
        retEvalPtr = new SimpleEvaluator(expression, std::move(treeList));
    else
        retEvalPtr = new ComplexEvaluator(expression, std::move(treeList), numGlobals > 0, m_offset);

    m_evaluators.push_back(retEvalPtr);
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

void RuntimeLoader::update(const VarIndiceMap &varMap, const uint depth, const std::map<char, float*> &globalMap)
{
    m_maxStackSz = 0;
    for(auto eval : m_evaluators)
    {
        eval->update(varMap, depth, globalMap);
        uint stackSz = eval->maxStackSz();
        if(stackSz > m_maxStackSz) m_maxStackSz = stackSz;
    }
}

} //namespace EVAL