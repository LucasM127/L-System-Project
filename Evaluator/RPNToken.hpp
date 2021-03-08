#ifndef EVAL_RPNTOKEN_HPP
#define EVAL_RPNTOKEN_HPP

#ifdef EVAL_DEBUG
#define LOG_DEBUG_STDOUT
#endif

#include <cstdint>
#include <stack>
#include <vector>
#include <iostream>
#include <Logger.hpp>
#include "Numbers.hpp"

//RPN Reverse Polish Notation

struct RPNToken;

typedef void(*opFnPtr)(std::stack<float>&);
typedef std::vector<RPNToken> RPNList;

struct RPNToken//for 64 bit
{
    RPNToken(){}
    RPNToken(char c, bool _isOp) : isOp(_isOp), isConst(false), isComplex(false), token(c)
    {
        //LOG("Make char '", c);
    }
	RPNToken(float v) : isOp(false), isConst(true), isComplex(false), value(v)
    {
        //LOG("Make float ", v);
    }
    RPNToken(const RPNList &list) : isOp(false), isConst(false), isComplex(true), rpnList(new RPNList(list)) {}
	RPNToken(const RPNList &&list) : isOp(false), isConst(false), isComplex(true), rpnList(new RPNList(list)) {}
	
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

#endif //EVAL_RPNTOKEN_HPP