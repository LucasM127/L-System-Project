#include "RPNTokenize.hpp"
#include "../Parsing/ParsingFuncs.hpp"

#include <unordered_set>

namespace EVALPARSE
{

//sorted by decreasing string size
static const std::vector< std::pair<std::string, char> > tokenStringReplaceVector =
{
    {"cotangent",	8},
    {"cosecant",	6 },
    {"rand()%",		9 },
    {"secant", 		7 },
    {"cosec",		6 },
    {"cotan", 		8 },
    {"asin", 		6 },
    {"acos", 		7 },
    {"atan", 		8 },
    {"sinf",  		3 },
    {"rand",        9 },
    {"cosf",  		4 },
    {"cot", 		8 },
    {"csc",			6 },
    {"sec", 		7 },
    {"pow", 	   '^'},
    {"sin",  		3 },
    {"cos",  		4 },
    {"tan",  		5 },
    {"max",         13},
    {"==",			0 },
    {">=",			1 },
    {"<=",			2 },
    //if lhs(!= 1 && rhs!=1 
    {"&&",			11},
    {"||",			12}
};

static const std::unordered_set<char> opSet = 
{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, '>', '<', '+', '-', '*', '/', '^'
};

//Just support 'single' char globals for now.
EVAL::RPNList tokenize(const std::string &expression, const VarIndiceMap &varMap, const uint varDepth,
                        const std::unordered_map<char,float> &globalMap)
{
    std::string exp = expression;
    //TODO... maybe have a better way of tokenizing instead of just using Find and replace all the time...
    LSPARSE::findAndReplace(exp, tokenStringReplaceVector);//ops
    //LSPARSE::findAndReplace(exp, globalNameMap);//globals 'many words'

    auto isAnOp = [](char c)->bool
    {
        return opSet.count(c) > 0;
    };

    auto isAGlob = [&](char c)->bool
    {
        return globalMap.count(c) > 0;
    };

    std::vector<EVAL::RPNToken> tokens;
    //just digits left
    uint i = -1;
    char c_next;
    while(LSPARSE::next(exp, i, c_next))
    {
        ++i;
        if(c_next == ' ') continue;
        if(c_next == '(' || c_next == ')' || c_next == ',')
            tokens.emplace_back(c_next, EVAL::SYM);
        else if(isAnOp(c_next))//'token' op map...
            tokens.emplace_back(c_next, EVAL::OP);
        else if(varMap.find(c_next) != varMap.end())
        {
            uint index = varMap.at(c_next).letterNum * varDepth + varMap.at(c_next).paramNum;
            tokens.emplace_back(index);
        }
        else if(isAGlob(c_next))
        {
            tokens.emplace_back(c_next, EVAL::GLB);
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

}//namespace EVALPARSE