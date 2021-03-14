#ifndef EVAL_RPNTOKEN_HPP
#define EVAL_RPNTOKEN_HPP

#ifdef EVAL_DEBUG
#define LOG_DEBUG_STDOUT
#endif

#include <cstdint>
#include <stack>
#include <vector>

#include "Numbers.hpp"

//RPN Reverse Polish Notation

namespace EVAL
{

struct RPNToken;

typedef void(*opFnPtr)(std::stack<float>&);
typedef std::vector<RPNToken> RPNList;

extern const bool OP;
extern const bool VAR;

struct RPNToken//for 64 bit
{
    RPNToken();
    RPNToken(char c, bool _isOp);
	RPNToken(float v);
    RPNToken(const RPNList &list);
	RPNToken(const RPNList &&list);
	
	bool isOp;
    bool isConst;
    bool isComplex;
	union
	{
        char token;
        float value;
        RPNList *rpnList;
        Number *numPtr;
        opFnPtr fnPtr;
	};
};

}//namespace EVAL

#endif //EVAL_RPNTOKEN_HPP