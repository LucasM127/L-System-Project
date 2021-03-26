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

//typedef void(*opFnPtr)(std::stack<float>&);
typedef void(*opFnPtr)(float*, uint&);
typedef std::vector<RPNToken> RPNList;

extern const bool OP;
extern const bool VAR;

void deepCopy(const RPNList &list, RPNList &target);

//Two stages ... tokenized, vs instanciated ('+' -> pointer to add function)
struct RPNToken
{
    RPNToken();
    RPNToken(char c, bool isOp);
	RPNToken(float v);
    RPNToken(float *v);
    RPNToken(const RPNList &list);
	RPNToken(const RPNList &&list);

    enum class TYPE
    {
        OP,
        CONST,
        VAR,
        GLOBAL,
        COMPLEX
    } type;

	union
	{
        char token;
        float value;
        float *global;
        RPNList *rpnList;
	};
};

struct RPN
{
    enum class TYPE
    {
        OP,
        CONST,
        VAR
    } const type;
    union
	{
        const float value;//Global or constant
        const uint index;//LVariable
        const opFnPtr fnPtr;//op Function
	};
};

}//namespace EVAL

#endif //EVAL_RPNTOKEN_HPP