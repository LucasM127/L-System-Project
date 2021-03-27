#include "Evaluator.hpp"

#include "Simplify.hpp"
#include "RPNListFuncs.hpp"
#include <cassert>

namespace EVAL
{

Evaluator::Evaluator(const std::string &exp, bool _isConst, bool global, RPNList &&refList)
                     : expression(exp), isConst(_isConst), hasGlobal(global), m_refList(std::move(refList))
{}

void Evaluator::update(const VarIndiceMap &varMap, const uint depth, const std::map<char, float*> &globalMap)
{
    m_tempList.clear();
    RPNList tempTreeList;
    deepCopy(m_refList, tempTreeList);
    convertGlobalsToConst(tempTreeList, globalMap);
    Simplify S;
    S(tempTreeList);//all is const now again!
    expand(tempTreeList, m_tempList);//flattens the tree
    //convert
    updateLocal(varMap, depth);
}

uint Evaluator::maxStackSz(){return 1;}

ConstEvaluator::ConstEvaluator(const std::string &exp, RPNList &&refList, bool global) : Evaluator(exp, true, global, std::move(refList)) {}
float ConstEvaluator::evaluate(float *v) {return m_val;}


void ConstEvaluator::updateLocal(const VarIndiceMap &varMap, const uint depth)
{
    assert(m_tempList.size()==1);
    m_val = m_tempList[0].value;
}

SimpleEvaluator::SimpleEvaluator(const std::string &exp, RPNList &&refList) : Evaluator(exp, false, false, std::move(refList)) {}
float SimpleEvaluator::evaluate(float *v) {return v[m_index];}

void SimpleEvaluator::updateLocal(const VarIndiceMap &varMap, const uint depth)
{
    assert(m_tempList.size()==1);
    VarIndice V = varMap.at(m_tempList[0].token);
    m_index = V.letterNum * depth + V.paramNum;
}

ComplexEvaluator::ComplexEvaluator(const std::string &exp, RPNList &&refList, bool global, uint offset)
                    : Evaluator(exp, false, global, std::move(refList)), m_offset(offset){}

float ComplexEvaluator::evaluate(float *v)
{
    uint top = m_offset;
    for(auto &T : m_rpnList)
    {
        switch(T.type)
        {
            case RPN::TYPE::OP:
                T.fnPtr(v,top);
                break;
            case RPN::TYPE::CONST:
                v[top++] = T.value;
                break;
            case RPN::TYPE::VAR:
                v[top++] = v[T.index];
                break;
            default:
                break;
        }
    }

    return v[m_offset];
}

void ComplexEvaluator::updateLocal(const VarIndiceMap &varMap, const uint depth)
{
    m_rpnList.clear();
    for(auto &T : m_tempList)
    {
        if(T.type == RPNToken::TYPE::OP)
        {
            if(unaryOpMap.find(T.token) != unaryOpMap.end())
                m_rpnList.emplace_back(unaryOpMap.at(T.token));
            else if(isAFunc(T.token))
                m_rpnList.emplace_back(funcOpMap.at(T.token));
            else
                m_rpnList.emplace_back(binaryOpMap.at(T.token));
        }
        else
        {
            if(T.type == RPNToken::TYPE::CONST)
                m_rpnList.emplace_back(T.value);
            else
            {
                VarIndice V = varMap.at(T.token);
                m_rpnList.emplace_back(V.letterNum * depth + V.paramNum);
            }
        }
    }
}

//after update() call so templist is valid.
uint ComplexEvaluator::maxStackSz()
{
    uint maxSz = 0;
    uint curSz = 0;
    for(auto T : m_tempList)
    {
        if(T.type == RPNToken::TYPE::OP)
        {
            if(!isUnary(T.token))
                --curSz;//currently only supports 2 parameters.. would be more if a function, but overkill is ok for now.
        }
        else
        {
            ++curSz;
            if(curSz > maxSz) maxSz = curSz;
        }
    }
    return maxSz;
}

}//namespace EVAL