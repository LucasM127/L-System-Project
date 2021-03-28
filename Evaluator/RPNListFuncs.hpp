#ifndef RPN_LIST_FUNCS_HPP
#define RPN_LIST_FUNCS_HPP

#include "RPNToken.hpp"

namespace EVAL
{

extern std::map<char,opFnPtr> binaryOpMap;
extern std::map<char,opFnPtr> unaryOpMap;
extern std::map<char,opFnPtr> funcOpMap;

extern std::map<char, bool> opLeftAssociativityMap;
extern std::map<char, int> opPriorityMap;

bool isUnary(const char op);
bool isAnOp(const char op);
bool isAFunc(const char op);


void printStack(std::stack<char> &opStack);
void printList(const RPNList &rpnList);

//used in Evaluator.cpp
void convertGlobalsToConst(RPNList &rpnList, const std::map<char, float*> &globalMap);

} //namespace EVAL

#endif //RPN_LIST_FUNCS_HPP