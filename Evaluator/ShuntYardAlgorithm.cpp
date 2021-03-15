#include "EvalLoader.hpp"
#include "../Logger/Logger.hpp"
#include <cassert>

//AND THIS WORKS FOR WHAT I NEED IT TO DO SO I AM HAPPY :)

namespace EVAL
{

const size_t OUT_OF_RANGE = -1;

#ifdef EVAL_DEBUG
void printStack(std::stack<char> &opStack)
{
    LOG_S("opStack is: ");
    {
        std::stack<char> tempStack;
        while(opStack.size())
        {
            if(opStack.top() < 30)
                LOG_S("op",int(opStack.top())," ");
            else
                LOG_S(opStack.top()," ");
            tempStack.push(opStack.top());
            opStack.pop();
        }
        while(tempStack.size())
        {
            opStack.push(tempStack.top());
            tempStack.pop();
        }
    }
    LOG_S("\n");
}

void printList(const RPNList &rpnList)
{
    for(auto &T : rpnList)
    {
        LOG_S(" ");
        if(T.isOp)
        {
            if(T.token < 30)
                LOG_S("op",int(T.token));
            else
                LOG_S(char(T.token));
        }
        else
        { 
            if(T.isConst)
                LOG_S(T.value);
            else if(T.isComplex)
            {
                LOG_S("[");
                printList(*T.rpnList);
                LOG_S("]");
            }
            else
                LOG_S(char(T.token));
        }
    }
}
#else
void printStack(std::stack<char> &opStack){}
void printList(const RPNList &rpnList){}
#endif

//maybe I am just trying too hard...
void Loader::shuntYardAlgorithm(const std::vector<RPNToken> &tokenizedExpression)
{
    while(!m_rpnListStack.empty()) m_rpnListStack.pop();
    m_rpnListStack.push(RPNList());
    auto rpnList = &m_rpnListStack.top();

    const int startOpLvl = 0;//-1 comparison operators
    bool lastWasANumber = false;

    std::stack<int> curOpLvlStack;
    curOpLvlStack.push(startOpLvl);
    int curOpLvl = curOpLvlStack.top();

    uint funcArgCtr = 0;
    std::stack<uint> funcArgCtrStack;

    std::stack<char> opStack;

    auto popLvl = [&]() -> void
    {
        RPNToken token(*rpnList);
        m_rpnListStack.pop();
        rpnList = &m_rpnListStack.top();
        rpnList->push_back(token);
    };

    auto popLvls = [&](int targetLvl) -> void
    {
        while(targetLvl < curOpLvl)
        {
            --curOpLvl;
            popLvl();
        }
    };

    auto pushReset = [&]() -> void
    {
        m_rpnListStack.push(RPNList());
        rpnList = &m_rpnListStack.top();
        curOpLvlStack.push(curOpLvl);
        curOpLvl = startOpLvl;
    };

    auto tryPush = [&](char op) -> void
    {//push or pop stack 'state' to proper level
        int opLvl = opPriorityMap[op];
        if(isUnary(op) || isAFunc(op))
        {//they do their own thing.  not associative, just take their arguments and run.
            opStack.push(op);
            return;
        }
        while(opLvl > curOpLvl)
        {//last push really belonged to the next level (ie. 4+3*? => 4+[3*?])
            ++curOpLvl;
            m_rpnListStack.push(RPNList());
            m_rpnListStack.top().push_back(rpnList->back());
            rpnList->pop_back();
            rpnList = &m_rpnListStack.top();
        }
        while(opLvl < curOpLvl)
        {//pop (ie. 3*x-? => 4+[3*x]-) (to continue my example)
            while(!opStack.empty() &&
                    (opPriorityMap[opStack.top()] == curOpLvl) )
            {//ensure each list has appropiate op tokens
                evaluate(opStack);
            }
            --curOpLvl;

            RPNToken token;
            if(rpnList->size() > 1)
                token = RPNToken(*rpnList);
            else
                token = rpnList->back();
            m_rpnListStack.pop();
            rpnList = &m_rpnListStack.top();
            rpnList->push_back(token);
        }
        if(!opStack.empty())
        {
            bool isLeftAssociative = opLeftAssociativityMap[op];
            char lastOp = opStack.top();
            int lastPriority = opPriorityMap[lastOp];
            int nextPriority = opPriorityMap[op];
            while((isLeftAssociative && lastPriority >= nextPriority) ||
                  (!isLeftAssociative && lastPriority > nextPriority))
            {
                if(lastOp == '(') break;
                evaluate(opStack);
                if(opStack.empty()) break;
                lastOp = opStack.top();
                lastPriority = opPriorityMap[lastOp];
            }
        }
        opStack.push(op);
    };

    for(unsigned int i = 0; i < tokenizedExpression.size(); ++i)
    {
        RPNToken next = tokenizedExpression[i];

        if(next.isOp)
            if(next.token < 30)
	    		LOG("op'",(int)next.token,"' : ");
            else
                LOG("op'", next.token, "' : ");
		else if(next.isConst)
			LOG("val'",next.value,"' : ");
        else
            LOG("val'",next.token,"' : ");
        
        if(next.isOp && next.token == '(')//next == '(')
        {//push Reset to initial conditions
            if(lastWasANumber)
                tryPush('*');

            opStack.push('(');

            pushReset();

            lastWasANumber = false;
        }
        else if(next.isOp && (next.token != ')' && next.token != ','))//isAnOp(next))
        {
            //5sin(x)  5(-1) (x+1)sin(5)
            if((isUnary(next.token) || isAFunc(next.token)) && lastWasANumber == true)
                tryPush('*');
            if(lastWasANumber == false)
            {
                if(next.token == '-')
                    next.token = 10;//unary negate operator
                if(next.token == '+')
                {
                    continue;//unary positive operator does nothing
                }
            }

            if(isAFunc(next.token))
            {
                funcArgCtrStack.push(funcArgCtr);
                funcArgCtr = 0;
                pushReset();
            }

            tryPush(next.token);
            lastWasANumber = false;
        }
        else //is a number one way or another
        {
            RPNToken numToken;
            
            if(next.isOp && next.token == ')')//closing bracket
            {
                while (true)
                {
                    if(opStack.top() == '(')
                    {
                        opStack.pop();
                        break;
                    }
                    
                    char op = opStack.top();
                    int opLvl = opPriorityMap[op];
                    popLvls(opLvl);
                    
                    evaluate(opStack);
                }
                if(rpnList->size() > 1)
                    numToken = RPNToken(*rpnList);
                else
                    numToken = rpnList->back();

                popLvls(startOpLvl);

                m_rpnListStack.pop();
                rpnList = &m_rpnListStack.top();
                curOpLvl = curOpLvlStack.top();
                curOpLvlStack.pop();

                lastWasANumber = false;
            }
            else if(next.isOp && next.token == ',')
            {
                ++funcArgCtr;
                while (true)
                {
                    if(opStack.top() == '(')//assumed
                    {
                        break;
                    }
                    
                    char op = opStack.top();
                    int opLvl = opPriorityMap[op];
                    popLvls(opLvl);
                    
                    evaluate(opStack);
                }
                if(rpnList->size() > 1)
                    numToken = RPNToken(*rpnList);
                else
                    numToken = rpnList->back();

                popLvls(startOpLvl);

                //numToken holds what we need to use.
                //rpnList is better though!

                //pop to function rpnlist level
                m_rpnListStack.pop();
                rpnList = &m_rpnListStack.top();
                curOpLvl = curOpLvlStack.top();
                curOpLvlStack.pop();

                rpnList->emplace_back(numToken);

                //push back to () bracket level
                pushReset();
                
                lastWasANumber = false;
                continue;//
            }
            else
            {
                numToken = next;
            }

            //Handle unary or functions here???
            if(!opStack.empty())
            {
                if(isUnary(opStack.top()))
                {
                    std::vector<RPNToken> tempRPNList;
                    tempRPNList.emplace_back(numToken);
                    while(!opStack.empty() && isUnary(opStack.top()))
                    {
                        tempRPNList.emplace_back(opStack.top(),OP);
                        opStack.pop();
                    }
                    numToken = RPNToken(tempRPNList);
                }
                else if(isAFunc(opStack.top()))//assumed funcargs cleared, as '(' was cleared
                {
                    
                    //NumToken holds my arguments!
                    //numToken.rpnList->emplace_back(opStack.top(),OP);
                    //opStack.pop();
                    //rpnList->push_back(RPNToken(numToken));
                    rpnList->emplace_back(numToken);
                    rpnList->emplace_back(opStack.top(),OP);
                    opStack.pop();
//                    rpnList->push_back(numToken);
                    LOG_S("IN FUNC: ");
                    printList(*rpnList);
                    LOG_S("\n");
                    numToken = RPNToken(*rpnList);

                    //pop out of function rpnlist level
                    m_rpnListStack.pop();
                    rpnList = &m_rpnListStack.top();
                    curOpLvl = curOpLvlStack.top();
                    curOpLvlStack.pop();
                }
            }

            if(lastWasANumber)
                tryPush('*');

            rpnList->emplace_back(numToken);

            if(!next.isOp || next.token != ',')
                lastWasANumber = true;
        }

        //logging
        LOG_S("rpn list (",m_rpnListStack.size(),"): ");
        printList(*rpnList);
        LOG_S("\n");
        printStack(opStack);
    }

    while (!opStack.empty())
    {
        char op = opStack.top();
        int opLvl = opPriorityMap[op];
        popLvls(opLvl);
        
        evaluate(opStack);
    }

    popLvls(startOpLvl);
    
    LOG_S("END list is: ");
    printList(*rpnList);
    LOG_S("\n");

    assert(opStack.empty());
}


void Loader::expand(std::vector<RPNToken> &rpnList, std::vector<RPNToken> &rpnListFinal)
{
    for(std::size_t i = 0; i < rpnList.size(); ++i)//auto &T : rpnList)
    {
        auto &T = rpnList[i];
        if(T.isComplex)
        {
                expand(*T.rpnList, rpnListFinal);
                delete T.rpnList;//complex not needed????
        }
        else
        {
            rpnListFinal.push_back(T);
        }
    }
}

void Loader::evaluate(std::stack<char>& opStack)
{//no try simplify here yet...
    auto &rpnList = m_rpnListStack.top();

    char op = opStack.top();
    opStack.pop();
    
    rpnList.emplace_back(op,OP);
}

void Loader::simplify(RPNList& rpnList)
{
    LOG("IN SIMPLIFY");
    printList(rpnList);
    LOG("");
    //first simplify any complex numbers that may be in the list
    for(std::size_t i = 0; i < rpnList.size(); ++i)
    {
        RPNToken &token = rpnList[i];
        if(token.isComplex)
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
            {
                distribute(rpnList, i);
            }
        }
    }

    const RPNToken &T = rpnList.back();// rpnList[0];//rpnList.back();
    if(!T.isOp)
        return;

    if(isUnary(T.token))
        simplifyUnary(rpnList);
    else if(isAFunc(T.token))
        simplifyFunc(rpnList);
    else
        simplifyBinary(rpnList);
}

//push pop + n*(pop)
void Loader::simplifyUnary(RPNList& rpnList)
{//if const unary, calculate it out till hit rand function
    LOG("IN SIMPLIFY UNARY");
    printList(rpnList);
    LOG("");

    RPNToken numToken = rpnList.front();
    if(!numToken.isConst)
        return;

    m_floatStack.push(numToken.value);
    size_t i = 1;
    for(; i < rpnList.size(); ++i)
    {
        RPNToken & nextPop = rpnList[i];
        char nextOp = nextPop.token;
        if(nextOp == 9) break;//random op. Any future pops will no longer be const
        unaryOpMap[nextOp](m_floatStack);
    }
    float number = m_floatStack.top();
    m_floatStack.pop();

    rpnList.erase(rpnList.begin()+1, rpnList.begin()+i);//dont erase random op
    
    rpnList.front() = RPNToken(number);//,NUM);
}

//push * n_args pop
void Loader::simplifyFunc(RPNList& rpnList)
{
    LOG("IN SIMPLIFY FUNC");
    printList(rpnList);
    LOG("");
    assert(rpnList.size()==3);//2 args and function call

    uint constArgCtr = 0;
    uint numArgs = 2;

    for(uint i = 0; i < numArgs; ++i)
    {
        RPNToken &token = rpnList[i];
        if(token.isComplex)
        {
            simplify(*token.rpnList);
            if(token.rpnList->size() == 1)
            {
                RPNToken tempToken = token.rpnList->front();
                delete token.rpnList;
                token = tempToken;
            }
        }
        if(token.isConst)
            ++constArgCtr;
    }

    if(constArgCtr < numArgs) return;

    opFnPtr fnPtr = funcOpMap.at(rpnList[2].token);
    float lhs = rpnList[0].value;
    float rhs = rpnList[1].value;
    m_floatStack.push(lhs);
    m_floatStack.push(rhs);
    fnPtr(m_floatStack);
    float result = m_floatStack.top();
    m_floatStack.pop();
    rpnList.resize(1);
    rpnList[0] = RPNToken(result);
}

//Helper function
size_t locateNextConstant(const RPNList& rpnList, size_t k)
{
    for(size_t i = k;i < rpnList.size()-1; i++)
    {
        const RPNToken &curToken = rpnList[i];
        if(curToken.isOp)
            continue;
        if(curToken.isConst)
        {
            return i;
        }
    }
    return OUT_OF_RANGE;
};

//ASSUMES that there is no mixing of associativity at each level of operators. +- left   */ left   ^ right
//push push pop + n*(push pop) LEFT_ASSOCIATIVE
//push + n*(push) + n*(pop) RIGHT_ASSOCIATIVE
void Loader::simplifyBinary(RPNList& rpnList)
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
        if(nextToken.isOp)
            return;//end of the pushes
        else if(!nextToken.isConst)
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
            m_floatStack.push(lhsNumPtr->value);
            m_floatStack.push(rhsNumPtr->value);
            //int opIt = rpnList.size() - rhsIt;//but as all are ^ no matter
            binaryOpMap['^'](m_floatStack);
            float number = m_floatStack.top();
            m_floatStack.pop();
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
        m_floatStack.push(lhsNumPtr->value);
        m_floatStack.push(rhsNumPtr->value);
        char op = rpnList[rhsIt+1].token;
        binaryOpMap[op](m_floatStack);
        float number = m_floatStack.top();
        m_floatStack.pop();
        rpnList.erase(rpnList.begin() + rhsIt, rpnList.begin() + rhsIt + 2);
        rpnList[lhsIt].value = number;
    }

    return;
}

//simplify a leaf node, then afterwords distribute to main branch
//call on numPtr that passes isComplex() test
void Loader::distribute(RPNList &rpnList, int complexIt)//for left Associative terms -> can make assumptions
{
    auto negate = [](RPNList &rpnList) -> void
    {
        for(auto &T : rpnList)
        {
            if(T.isOp)
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
            complexIt++;
        }
        //advance it to see if can pull to the right
        while(true)
        {
            if(complexList[constIt+1].isOp)
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
void Loader::testDistribute()
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
}//namespace EVAL

/************************
void Loader::shuntYardAlgorithm(const std::vector<RPNToken> &tokenizedExpression)
{
    while (!m_rpnListStack.empty()) m_rpnListStack.pop();
    m_rpnListStack.push(RPNList());
    auto rpnList = &m_rpnListStack.top();
    auto rpnListBottom = rpnList;

    const int startOpLvl = -1;//0;//-1 for comparison operators
    bool lastWasANumber = false;

    std::stack<int> curOpLvlStack;//DO I NEED TO PUSH IT HERE???
    curOpLvlStack.push(startOpLvl);//reset this for ( )
    int curOpLvl = curOpLvlStack.top();//+ -

    uint funcArgCtr = 0;
    std::stack<uint> funcArgCtrStack;

    std::stack<char> opStack;

    auto popLvls = [&](int targetLvl) -> void
    {
        while(targetLvl < curOpLvl)
        {
            --curOpLvl;
            RPNToken token(*rpnList);
            m_rpnListStack.pop();
            rpnList = &m_rpnListStack.top();
            rpnList->push_back(token);//that's the one I have troubles with...
        }
    };

    auto tryPush = [&](char op) -> void
    {
        int opLvl = opPriorityMap[op];
        if(/*opLvl == 4 && * /isUnary(op) || isAFunc(op))//unary ops I just make a complex number as find them. (all right associative in this code base)
        {
            opStack.push(op);
            return;
        }
        //push or pop stack 'state' to proper level
        while(opLvl > curOpLvl)
        {//last push really belonged to the next level (ie. 4+3*? => 4+[3*?])
            curOpLvl++;
            m_rpnListStack.push(RPNList());
            auto topRPNList = &m_rpnListStack.top();
            topRPNList->push_back(rpnList->back());
            rpnList->pop_back();
            rpnList = topRPNList;
        }
        while(opLvl < curOpLvl)
        {//pop (ie. 3*x-? => 4+[3*x]-) (to continue my example)
            while (!opStack.empty() && 
                  (opPriorityMap[opStack.top()] == curOpLvl) )
            {//ensure each list has the appropiate op tokens
                evaluate(opStack);
            }
            curOpLvl--;
            
            RPNToken tempToken;//an assignable token????????

            if(rpnList->size() > 1)
                tempToken = RPNToken(*rpnList);
            else
                tempToken = rpnList->back();
            m_rpnListStack.pop();
            rpnList = &m_rpnListStack.top();
            rpnList->push_back(tempToken);
        }
        if(!opStack.empty())
        {
            bool isLeftAssociative = opLeftAssociativityMap[op];
            char lastOp = opStack.top();
            int lastPriority = opPriorityMap[lastOp];
            int nextPriority = opPriorityMap[op];
            while((isLeftAssociative && lastPriority >= nextPriority) ||
                  (!isLeftAssociative && lastPriority > nextPriority))
            {
                if(lastOp == '(') break;
                evaluate(opStack);
                if(opStack.empty()) break;
                lastOp = opStack.top();
                lastPriority = opPriorityMap[lastOp];
            }
        }
        opStack.push(op);
    };
/*  How would have gone without using oplvl stack for comparison
    auto tryPush = [&](char op) -> void
    {
        if(!opStack.empty())
        {
            char lastOp = opStack.top();
            int lastPriority = opPriorityMap[lastOp];
            int nextPriority = opPriorityMap[op];
            bool isLeftAssociative = opLeftAssociativityMap[op];
            while((isLeftAssociative && lastPriority >= nextPriority) ||
                  (!isLeftAssociative && lastPriority > nextPriority))
            {
                if(lastOp == '(') break;
                evaluate(opStack);
                if(opStack.empty()) break;
                lastOp = opStack.top();
                lastPriority = opPriorityMap[lastOp];
            }
        }
        opStack.push(op);
    };
* /

    for(unsigned int i = 0; i < tokenizedExpression.size(); )
    {
        RPNToken next = tokenizedExpression[i];

        if(next.isOp)
            if(next.token < 30)
	    		LOG("op'",(int)next.token,"' : ");
            else
                LOG("op'", next.token, "' : ");
		else if(next.isConst)
			LOG("val'",next.value,"' : ");
        else
            LOG("val'",next.token,"' : ");
        
        if(next.isOp && next.token == '(')//next == '(')
        {//push Reset to initial conditions
            if(lastWasANumber)
                tryPush('*');

            opStack.push('(');

            m_rpnListStack.push(RPNList());
            rpnList = &m_rpnListStack.top();
            curOpLvlStack.push(curOpLvl);
            curOpLvl = startOpLvl;

            lastWasANumber = false;
            ++i;
        }
        else if(next.isOp && (next.token != ')' && next.token != ','))//isAnOp(next))
        {
            //5sin(x)  5(-1) (x+1)sin(5)
            if((isUnary(next.token) || isAFunc(next.token)) && lastWasANumber == true)
                tryPush('*');
            if(lastWasANumber == false)
            {
                if(next.token == '-')
                    next.token = 10;//unary negate operator
                if(next.token == '+')
                {
                    continue;//unary positive operator does nothing
                }
            }

            if(isAFunc(next.token))
            {
                m_rpnListStack.push(RPNList());
                rpnList = &m_rpnListStack.top();
                curOpLvlStack.push(curOpLvl);
                curOpLvl = startOpLvl;//push

                funcArgCtrStack.push(funcArgCtr);
                funcArgCtr = 0;
            }//before tryPush

            tryPush(next.token);
            lastWasANumber = false;
            ++i;
        }
        else //is a number one way or another
        {
            RPNToken numToken;
            
            if(next.isOp && next.token == ')')
            {
                while (true)
                {
                    if(opStack.top() == '(')
                    {
                        opStack.pop();
                        break;
                    }
                    
                    char op = opStack.top();
                    int opLvl = opPriorityMap[op];
                    popLvls(opLvl);
                    
                    evaluate(opStack);
                }
                if(rpnList->size() > 1)
                    numToken = RPNToken(*rpnList);
                else
                    numToken = rpnList->back();

                popLvls(startOpLvl);
//                printList(*rpnList);LOG("IN AREA");

                m_rpnListStack.pop();
                rpnList = &m_rpnListStack.top();
                curOpLvl = curOpLvlStack.top();
                curOpLvlStack.pop();
/*
                if(opStack.size() && isAFunc(opStack.top()))
                {//final pop ???
                    RPNToken tempToken;//an assignable token????????

                    //if(rpnList->size() > 1)
                        tempToken = RPNToken(*rpnList);
                    //else
                    //    tempToken = rpnList->back();
                    m_rpnListStack.pop();
                    rpnList = &m_rpnListStack.top();
                    rpnList->push_back(tempToken);
                    evaluate(opStack);
                    printList(*rpnList);LOG("IN AREA2");
                }* /

                lastWasANumber = false;
            }
            else if(next.isOp && next.token == ',')
            {
                ++funcArgCtr;
                while (true)
                {
                    if(opStack.top() == '(')//assumed
                    {
                        //if(funcArgCtr == 2)
                        //    opStack.pop();
                        break;
                    }
                    
                    char op = opStack.top();
                    int opLvl = opPriorityMap[op];
                    popLvls(opLvl);
                    
                    evaluate(opStack);
                }
                if(rpnList->size() > 1)
                    numToken = RPNToken(*rpnList);
                else
                    numToken = rpnList->back();

                popLvls(startOpLvl);
                
                if(funcArgCtr < 2)
                {//ie pop and push
                    m_rpnListStack.top().clear();
                    curOpLvl = startOpLvl;
                    funcArgCtr = funcArgCtrStack.top();
                    funcArgCtrStack.pop();
                }//reset
                
                lastWasANumber = false;
            }
            else
            {
                numToken = next;
            }

            //create complex number from a unary
            //as a unary complex can be a term(lvl1) 1+sin90+2, product(lvl2) 4*sin90*2, or power(lvl3) (sin90)^2 2^-x
            if(!opStack.empty() && isUnary(opStack.top()))
            {
                std::vector<RPNToken> tempRPNList;
                tempRPNList.emplace_back(numToken);
                while(!opStack.empty() && isUnary(opStack.top()))
                {
                    tempRPNList.emplace_back(opStack.top(),OP);
                    opStack.pop();
                }
                numToken = RPNToken(tempRPNList);
            }

            if(lastWasANumber)
                tryPush('*');

            rpnList->emplace_back(numToken);
            lastWasANumber = true;
            if(next.isOp && next.token == ',')
                lastWasANumber = false;
            //then here???

            ++i;
        }

        //logging
        LOG_S("rpn list (",m_rpnListStack.size(),"): ");
        printList(*rpnList);
        LOG_S("\n");
        printList(*rpnListBottom);
        LOG_S("\n");
        printStack(opStack);
    }

    while (!opStack.empty())
    {
        char op = opStack.top();
        int opLvl = opPriorityMap[op];
        popLvls(opLvl);
        
        evaluate(opStack);
    }

    popLvls(startOpLvl);
    
    LOG_S("END list is: ");
    printList(*rpnList);
    LOG_S("\n");

    assert(opStack.empty());
}
*************************************************************/