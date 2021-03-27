#ifndef RPN_LIST_FUNCS_HPP
#define RPN_LIST_FUNCS_HPP

#include "RPNToken.hpp"

namespace EVAL
{

static std::map<char,opFnPtr> binaryOpMap;
static std::map<char,opFnPtr> unaryOpMap;
static std::map<char,opFnPtr> funcOpMap;

static std::map<char, bool> opLeftAssociativityMap;
static std::map<char, int> opPriorityMap;

bool isUnary(const char op);
bool isAnOp(const char op);
bool isAFunc(const char op);

void expand(RPNList &rpnList, RPNList &rpnListFinal);

void printStack(std::stack<char> &opStack);
void printList(const RPNList &rpnList);

} //namespace EVAL

#endif //RPN_LIST_FUNCS_HPP