#include "EvalLoader.hpp"
#include "Evaluator.hpp"

#include "../Logger/Logger.hpp"
#include "ShuntYardAlgorithm.hpp"
#include "../Parsing/RPNTokenize.hpp"

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
Evaluator* RuntimeLoader::load(const std::string &expression, const VarIndiceMap &varMap, int maxVarDepth, const std::string &comment)
//Evaluator *RuntimeLoader::load(const std::string &expression, const RPNList &tokenizedExp, const std::string &comment)
{
    RPNList tokenizedExp = EVALPARSE::tokenize(expression, varMap, maxVarDepth, *globalMapPtr);

    //go through the tokenized list and see what type it resolves to...
    uint numVars = 0;
    bool hasGlobals = false;
    bool hasRand = false;
    for(auto &tok : tokenizedExp)
    {
        if(tok.type == RPNToken::TYPE::VAR)
            ++numVars;
        else if(tok.type == RPNToken::TYPE::GLOBAL)
            hasGlobals = true;
        else if(tok.type == RPNToken::TYPE::OP && tok.token == 9)//random
            hasRand = true;
    }

    ShuntYardAlgorithm SYA;
    RPNList &treeList = SYA.apply(tokenizedExp);

    //create appropiate Evaluator
    Evaluator * retEvalPtr;
    if(numVars == 0 && !hasRand)//simplifies to a simple constant
        retEvalPtr = new ConstEvaluator(expression, std::move(treeList), hasGlobals);
    else if(tokenizedExp.size()==1)//just the one variable number
        retEvalPtr = new SimpleEvaluator(expression, std::move(treeList));//Size has to be one.
    else
        retEvalPtr = new ComplexEvaluator(expression, std::move(treeList), hasGlobals, m_offset);

    m_evaluators.push_back(retEvalPtr);
    return retEvalPtr;
}

Loader::~Loader()
{
    for(auto eval : m_evaluators)
        delete eval;
}

//kinda ties it to the lsystem, but that's ok, they are not too expensive
RuntimeLoader::RuntimeLoader():Loader(), m_offset(0), m_maxStackSz(0), globalMapPtr(nullptr){}

void RuntimeLoader::setOffset(uint offset)
{
    m_offset = offset;
}

void RuntimeLoader::setGlobalMap(const std::unordered_map<char,float> &globalMap)
{
    globalMapPtr = &globalMap;
}

uint RuntimeLoader::getMaxStackSz()
{
    return m_maxStackSz;
}

void RuntimeLoader::update()
{
    m_maxStackSz = 0;
    for(auto eval : m_evaluators)
    {
        eval->update(*globalMapPtr);
        uint stackSz = eval->maxStackSz();
        if(stackSz > m_maxStackSz) m_maxStackSz = stackSz;
    }
}

} //namespace EVAL