#include "Evaluator.hpp"

#include "RPNListFuncs.hpp"

namespace EVAL
{

//from simplified expressions!!!
//Evaluator function.
//RPNList std::vector<RPN> m_rpnList;
//Calculate after simplifying, but before converting
uint calcMaxStackSz()
{
    RPNList m_rpnList;

    uint maxSz = 0;
    uint curSz = 0;
    for(auto T : m_rpnList)
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