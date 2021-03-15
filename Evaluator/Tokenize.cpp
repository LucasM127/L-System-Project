#include "EvalLoader.hpp"
#include "../Parsing/ParsingFuncs.hpp"

std::vector<EVAL::RPNToken> EVAL::Loader::tokenize(const std::string &expression, const VarIndiceMap &varMap)
{
    std::string exp = expression;
    LSPARSE::findAndReplace(exp, tokenStringReplaceVector);
    //LSPARSE::findAndReplace(exp, variableStrings)//user defined variable names > 1 character
    //LSPARSE::findAndReplace(exp, globalStrings)//defined globals

    std::vector<EVAL::RPNToken> tokens;
    //just digits left
    uint i = -1;
    char c_next;
    while(LSPARSE::next(exp, i, c_next))
    {
        ++i;
        if(c_next == ' ') continue;
        if(c_next == '(' || c_next == ')' || c_next == ',')
            tokens.emplace_back(c_next, OP);
        else if(isAnOp(c_next))
            tokens.emplace_back(c_next,OP);
        else if(varMap.find(c_next) != varMap.end())
            tokens.emplace_back(c_next,VAR);
        else if(isdigit(c_next) || c_next == '.')
        {
            float number;
            LSPARSE::readNumber(exp, i, number);
            tokens.emplace_back(number);
        }
        else throw;
    }

    return tokens;
}