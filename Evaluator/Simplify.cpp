#include "Simplify.hpp"
#include "RPNListFuncs.hpp"
#include "../Logger/Logger.hpp"

#include <cassert>

namespace EVAL
{

const size_t OUT_OF_RANGE = -1;

//Helper function
size_t locateNextConstant(const RPNList& rpnList, size_t k)
{
    for(size_t i = k;i < rpnList.size()-1; ++i)
    {
        const RPNToken &curToken = rpnList[i];
        if(curToken.type == RPNToken::TYPE::OP)
            continue;
        if(curToken.type == RPNToken::TYPE::CONST)
        {
            return i;
        }
    }
    return OUT_OF_RANGE;
};

void Simplify::operator()(RPNList &rpnList)
{
    simplify(rpnList);
}

void Simplify::simplify(RPNList& rpnList)
{
    LOG("IN SIMPLIFY");
    printList(rpnList);
    LOG("");
    //first simplify any complex numbers that may be in the list
    for(std::size_t i = 0; i < rpnList.size(); ++i)
    {
        RPNToken &token = rpnList[i];
        if(token.type == RPNToken::TYPE::COMPLEX)
        {
            simplify(*token.rpnList);
            if(token.rpnList->size() == 1)
            {
                RPNToken tempToken = token.rpnList->front();
                delete token.rpnList;
                token = tempToken;
            }
            else if(rpnList.size() > 2 &&
                    opPriorityMap[token.rpnList->back().token] == opPriorityMap[rpnList.back().token] )
            {//ie, if 2+(3x+1) => push 2 push [] pop '+' vs push[3x] push 1 pop '+', both have '+' at end
            //(unarys, functions, selfcontained in brackets so doesn't affect)
                distribute(rpnList, i);
            }
        }
    }

    const RPNToken &T = rpnList.back();
    if(T.type != RPNToken::TYPE::OP)
        return;

    if(isUnary(T.token))
        simplifyUnary(rpnList);
    else if(isAFunc(T.token))
        simplifyFunc(rpnList);
    else
        simplifyBinary(rpnList);
}

//push pop + n*(pop)
void Simplify::simplifyUnary(RPNList& rpnList)
{//if const unary, calculate it out till hit rand function
    LOG("IN SIMPLIFY UNARY");
    printList(rpnList);
    LOG("");

    RPNToken numToken = rpnList.front();
    if(numToken.type != RPNToken::TYPE::CONST)//!numToken.isConst)
        return;

    unsigned int top = 0;
    m_floatStack[top++] = numToken.value;//.push(numToken.value);
    size_t i = 1;
    for(; i < rpnList.size(); ++i)
    {
        RPNToken & nextPop = rpnList[i];
        char nextOp = nextPop.token;
        if(nextOp == 9) break;//random op. Any future pops will no longer be const
        unaryOpMap[nextOp](m_floatStack,top);
    }
    float number = m_floatStack[0];//.top();
    //m_floatStack.pop();

    rpnList.erase(rpnList.begin()+1, rpnList.begin()+i);//dont erase random op
    
    rpnList.front() = RPNToken(number);//,NUM);
}

//push * n_args pop
void Simplify::simplifyFunc(RPNList& rpnList)
{
    LOG("IN SIMPLIFY FUNC");
    printList(rpnList);
    LOG("");
    assert(rpnList.size()==3);//2 args and function call

    unsigned int constArgCtr = 0;
    unsigned int numArgs = 2;

    for(unsigned int i = 0; i < numArgs; ++i)
    {
        RPNToken &token = rpnList[i];
        if(token.type == RPNToken::TYPE::COMPLEX)
        {
            simplify(*token.rpnList);
            if(token.rpnList->size() == 1)
            {
                RPNToken tempToken = token.rpnList->front();
                delete token.rpnList;
                token = tempToken;
            }
        }
        if(token.type == RPNToken::TYPE::CONST)
            ++constArgCtr;
    }

    if(constArgCtr < numArgs) return;

    opFnPtr fnPtr = funcOpMap.at(rpnList[2].token);
    float lhs = rpnList[0].value;
    float rhs = rpnList[1].value;
    unsigned int top = 0;
    m_floatStack[top++] = lhs;
    m_floatStack[top++] = rhs;
    fnPtr(m_floatStack, top);
    float result = m_floatStack[0];
    rpnList.resize(1);
    rpnList[0] = RPNToken(result);
}

//ASSUMES that there is no mixing of associativity at each level of operators. +- left   */ left   ^ right
//push push pop + n*(push pop) LEFT_ASSOCIATIVE
//push + n*(push) + n*(pop) RIGHT_ASSOCIATIVE
void Simplify::simplifyBinary(RPNList& rpnList)
{
    LOG("IN SIMPLIFY BINARY");
    printList(rpnList);
    LOG("");
    RPNToken * lhsNumPtr = nullptr;
    RPNToken * rhsNumPtr = nullptr;
    size_t lhsIt = OUT_OF_RANGE;
    size_t rhsIt = OUT_OF_RANGE;
    bool isLeftAssociative = opLeftAssociativityMap[rpnList.back().token];

    lhsIt = locateNextConstant(rpnList,0);
    if(lhsIt == OUT_OF_RANGE)
        return;
    lhsNumPtr = &rpnList[lhsIt];
    
    while(!isLeftAssociative)//for now just the power ^ operator
    {
        RPNToken &nextToken = rpnList[lhsIt+1];
        if(nextToken.type == RPNToken::TYPE::OP)
            return;//end of the pushes
        else if(nextToken.type != RPNToken::TYPE::CONST)//  !nextToken.isConst)
        {
            lhsIt = locateNextConstant(rpnList,lhsIt+2);
            if(lhsIt == OUT_OF_RANGE)
                return;
            lhsNumPtr = &rpnList[lhsIt];
        }
        else//can simplify!
        {
            rhsIt = lhsIt+1;
            rhsNumPtr = &nextToken;
            unsigned int top = 0;
            m_floatStack[top++] = lhsNumPtr->value;
            m_floatStack[top++] = rhsNumPtr->value;
            //int opIt = rpnList.size() - rhsIt;//but as all are ^ no matter
            binaryOpMap['^'](m_floatStack,top);
            float number = m_floatStack[0];
            //erase rhs and pop
            rpnList.erase(rpnList.begin() + rhsIt);
            rpnList.pop_back();
            rpnList[lhsIt].value = number;
        }
    }

    //for +-*/
    while(isLeftAssociative)
    {
        //seek next constant!
        rhsIt = locateNextConstant(rpnList,lhsIt+1);
        if(rhsIt == OUT_OF_RANGE)
            return;
        //can simplify!
        rhsNumPtr = &rpnList[rhsIt];
        unsigned int top = 0;
        m_floatStack[top++] = lhsNumPtr->value;
        m_floatStack[top++] = rhsNumPtr->value;
        char op = rpnList[rhsIt+1].token;
        binaryOpMap[op](m_floatStack,top);
        float number = m_floatStack[0];
        rpnList.erase(rpnList.begin() + rhsIt, rpnList.begin() + rhsIt + 2);
        rpnList[lhsIt].value = number;
    }

    return;
}

//simplify a leaf node, then afterwords distribute to main branch
//call on numPtr that passes isComplex() test
void Simplify::distribute(RPNList &rpnList, int complexIt)//for left Associative terms -> can make assumptions
{
    auto negate = [](RPNList &rpnList) -> void
    {
        for(auto &T : rpnList)
        {
            if(T.type == RPNToken::TYPE::OP)
            {
                     if(T.token == '+')
                         T.token = '-';
                else if(T.token == '-')
                         T.token = '+';
                else if(T.token == '*')
                         T.token = '/';
                else if(T.token == '/')
                         T.token = '*';
            }
        }
    };

    auto combine = [](const char a, const char b) -> char
    {
        if( (a == '+' && b == '+') ||
            (a == '-' && b == '-') )
            return '+';
        if( (a == '+' && b == '-') ||
            (a == '-' && b == '+') )
            return '-';
        if( (a == '*' && b == '*') ||
            (a == '/' && b == '/') )
            return '*';
        if( (a == '*' && b == '/') ||
            (a == '/' && b == '*') )
            return '/';
        return 0;
    };

    RPNList &complexList = *rpnList[complexIt].rpnList;
    
    bool isLeftAssociative = opLeftAssociativityMap[complexList.back().token];

    size_t constIt = locateNextConstant(complexList, 0);
    if(constIt == OUT_OF_RANGE)
        return;

    RPNToken constNumToken = complexList[constIt];

    if(!isLeftAssociative)//only the ^ operator at the moment...
    {
        if(constIt == 0)//can pull to the left...
        {
            rpnList.insert(rpnList.begin() + complexIt, constNumToken);//need to make 'adjacent'
            rpnList.emplace_back('^',OP);//just push back for now instead of inserting at 'appropiate spot
            complexList.erase(complexList.begin());
            complexList.pop_back();
            ++complexIt;
        }
        //advance it to see if can pull to the right
        while(true)
        {
            if(complexList[constIt+1].type == RPNToken::TYPE::OP) //.isOp)
            {
                //pull to right
                rpnList.insert(rpnList.begin() + complexIt + 1, constNumToken);
                rpnList.emplace_back('^',OP);
                complexList.erase(complexList.begin()+constIt);
                complexList.erase(complexList.begin()+constIt);//the next pop
                return;
            }
            constIt = locateNextConstant(complexList, constIt+1);
            if(constIt == OUT_OF_RANGE)
                return;
        }
        return;
    }

    //IS LEFT ASSOCIATIVE CODE BELOW....
    //scenarios...
    //constant first term in bracketed expression (T/F)
    //bracketed expression first term in rpnList (T/F)
    bool constFirstTerm = (constIt == 0);
    bool complexFirstTerm = (complexIt == 0);

    //operators acting on the (...) complex expression and on the constant within the bracketed expression respectively
    char complexOp;
    char constOp;

    int opLvl = opPriorityMap[rpnList.back().token];

    if(constFirstTerm && complexFirstTerm)
    {
        if(opLvl == 1)
        {
            complexOp = '+';
            constOp = '+';
        }
        else
        {
            complexOp = '*';
            constOp = '*';
        }
        //may negate
        char nextExpOp = complexList[constIt+2].token;//push push pop
        bool mustNegate = (nextExpOp == '-' || nextExpOp == '/');
        //insert and erase
        rpnList.insert(rpnList.begin() + 1, RPNToken(nextExpOp,OP));
        rpnList.insert(rpnList.begin(), constNumToken);
        complexList.erase(complexList.begin() + constIt);
        complexList.erase(complexList.begin() + constIt+1);//first push and pop --> (x push) push (x pop)
        if(mustNegate)
            negate(complexList);
    }
    if(constFirstTerm && !complexFirstTerm)
    {
        if(opLvl == 1)
        {
            constOp = '+';
        }
        else
        {
            constOp = '*';
        }
        complexOp = rpnList[complexIt+1].token;
        char nextExpOp = complexList[constIt+2].token;
        bool mustNegate = (nextExpOp == '-' || nextExpOp == '/');
        char newComplexOp = combine(complexOp, nextExpOp);

        //change it (before iterators invalidated)
        rpnList[complexIt+1].token = newComplexOp;
        //insert and erase
        rpnList.insert(rpnList.begin() + complexIt,{constNumToken, RPNToken(complexOp,OP)});
        complexList.erase(complexList.begin() + constIt);
        complexList.erase(complexList.begin() + constIt+1);//first push and pop --> (x push) push (x pop)
        if(mustNegate)
            negate(complexList);
    }
    if(!constFirstTerm && complexFirstTerm)
    {
        if(opLvl == 1)
        {
            complexOp = '+';
        }
        else
        {
            complexOp = '*';
        }
        constOp = complexList[constIt+1].token;
        rpnList.insert(rpnList.begin()+1,{constNumToken, RPNToken(constOp,OP)});
        complexList.erase(complexList.begin() + constIt, complexList.begin() + constIt + 2);
    }
    if(!constFirstTerm && !complexFirstTerm)
    {//
        constOp = complexList[constIt+1].token;
        complexOp = rpnList[complexIt+1].token;
        char newOp = combine(constOp, complexOp);
        rpnList.insert(rpnList.begin() + complexIt, {constNumToken, RPNToken(newOp,OP)});//why not NEEDED???
        complexList.erase(complexList.begin() + constIt, complexList.begin() + constIt + 2);
    }
}
/*
#ifdef EVAL_DEBUG
void Simplify::testDistribute()
{
    //3-(x-1)
    RPNList rpnList1 = 
    {
        RPNToken(3.f),//,NUM),
        RPNToken(
            {
                RPNToken('x',VAR),
                RPNToken( 1.f),// ,NUM),
                RPNToken('-',OP),
                RPNToken('y',VAR),
                RPNToken('-',OP)
            }
        ),
        RPNToken('-',OP)
    };

    //(x-1)-3
    RPNList rpnList2 = 
    {
        RPNToken(
            {
                RPNToken('x',VAR),
                RPNToken( 1.f),// ,NUM),
                RPNToken('-',OP),
                RPNToken('y',VAR),
                RPNToken('-',OP)
            }
        ),
        RPNToken(3.f),//, NUM),
        RPNToken('-',OP)
    };

    //3-(1-x)
    RPNList rpnList3 = 
    {
        RPNToken(3.f),//,NUM),
        RPNToken(
            {
                RPNToken( 1.f),// ,NUM),
                RPNToken('x',VAR),
                RPNToken('-',OP),
                RPNToken('y',VAR),
                RPNToken('-',OP)
            }
        ),
        RPNToken('-',OP)
    };

    //(1-x)-3
    RPNList rpnList4 = 
    {
        RPNToken(
            {
                RPNToken( 1.f),// ,NUM),
                RPNToken('x',VAR),
                RPNToken('-',OP),
                RPNToken('y',VAR),
                RPNToken('-',OP)
            }
        ),
        RPNToken( 3.f),// ,NUM),
        RPNToken('-',OP)
    };

    printList(rpnList1);
    LOG("\n");
    distribute(rpnList1,1);
    printList(rpnList1);
    LOG("\n3-(x-1-y)\n");

    printList(rpnList2);
    LOG("\n");
    distribute(rpnList2,0);
    printList(rpnList2);
    LOG("\n(x-1-y)-3\n");

    printList(rpnList3);
    LOG("\n");
    distribute(rpnList3,1);
    printList(rpnList3);
    LOG("\n3-(1-x-y)\n");

    printList(rpnList4);
    LOG("\n");
    distribute(rpnList4,0);
    printList(rpnList4);
    LOG("\n(1-x-y)-3\n");

    //cleanUp
    deleteList(rpnList1);
    deleteList(rpnList2);
    deleteList(rpnList3);
    deleteList(rpnList4);
}
#endif
*/

} //namespace EVAL