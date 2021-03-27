#include "EvalLoader.hpp"
#include "../Parsing/ParsingFuncs.hpp"

#include "RPNListFuncs.hpp"
/******
//convert to token form.  Replace strings with chars for easier parsing.
//std::vector<std::pair<std::string, char>>
//Send over the globalMap??? ok
//EvalTokenizer Class almost -sheesh-
std::vector<EVAL::RPNToken> EVAL::Loader::tokenize(const std::string &expression, const VarIndiceMap &varMap)
{
    std::string exp = expression;
    LSPARSE::findAndReplace(exp, tokenStringReplaceVector);//ops
//    LSPARSE::findAndReplace(exp, globalReplaceMap);//globals 'many words'
//not necessary yet

    std::vector<EVAL::RPNToken> tokens;
    //just digits left
    uint i = -1;
    char c_next;
    while(LSPARSE::next(exp, i, c_next))
    {
        ++i;
        if(c_next == ' ') continue;
        if(c_next == '(' || c_next == ')' || c_next == ',')
            tokens.emplace_back(c_next, SYM);
        else if(isAnOp(c_next))
            tokens.emplace_back(c_next,OP);
        else if(varMap.find(c_next) != varMap.end())
        {
            tokens.emplace_back(c_next,VAR);
        }
        else if(globalMap.find(c_next) != globalMap.end())
        {
            tokens.emplace_back(c_next, GLB);//globalMap.at(c_next));//float*
        }
        else if(isdigit(c_next) || c_next == '.')
        {
            float number;
            LSPARSE::readNumber(exp, i, number);
            tokens.emplace_back(number);
        }
        else throw std::runtime_error("Unrecognized symbol " + std::string(&c_next,1) + " in exp: " + expression);
    }

    return tokens;
}//RPN Token Vector that we can thus play with...
*****/