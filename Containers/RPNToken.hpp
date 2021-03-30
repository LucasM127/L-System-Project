#ifndef EVAL_RPNTOKEN_HPP
#define EVAL_RPNTOKEN_HPP

#ifdef EVAL_DEBUG
#define LOG_DEBUG_STDOUT
#endif

#include <vector>
//#include "VarIndice.hpp"

//RPN Reverse Polish Notation

namespace EVAL
{

struct RPNToken;

//typedef void(*opFnPtr)(std::stack<float>&);
typedef void(*opFnPtr)(float*, unsigned int&);
typedef std::vector<RPNToken> RPNList;

void destroy(RPNList &list);
void deepCopy(const RPNList &list, RPNList &target);
void expand(const RPNList &rpnList, RPNList &rpnListFinal);

//Two stages ... tokenized, vs instantiated ('+' -> pointer to add function)
struct RPNToken
{
    enum class TYPE
    {
        OP,
        SYMBOL,//comma or brackets
        CONST,
        VAR,
        GLOBAL,
        COMPLEX
    } type;

    RPNToken();
    RPNToken(char c, TYPE type);
    RPNToken(unsigned int i);
	RPNToken(float v);
    RPNToken(const RPNList &list);
	RPNToken(const RPNList &&list);

	union
	{
        char token;
        unsigned int index;
        float value;//I need these two
        RPNList *rpnList;    
	};
};

extern const RPNToken::TYPE OP;
//extern const RPNToken::TYPE VAR;
extern const RPNToken::TYPE SYM;
extern const RPNToken::TYPE GLB;

struct RPN
{
    enum class TYPE
    {
        OP,
        CONST,
        VAR
    } const type;

    RPN(float v);
    RPN(unsigned int i);
    RPN(opFnPtr p);

    union
	{
        const float value;//Global or constant
        const unsigned int index;//LVariable
        const opFnPtr fnPtr;//op Function
	};
};

}//namespace EVAL

#endif //EVAL_RPNTOKEN_HPP