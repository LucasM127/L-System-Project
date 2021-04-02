#include "ShuntYardAlgorithm.hpp"
#include "RPNListFuncs.hpp"
#include "../Logger/Logger.hpp"
#include <cassert>

//AND THIS WORKS FOR WHAT I NEED IT TO DO SO I AM HAPPY :)

namespace EVAL
{

RPNList &ShuntYardAlgorithm::apply(const RPNList &tokenizedExpression)
{
    while(!m_rpnListStack.empty()) m_rpnListStack.pop();
    m_rpnListStack.push(RPNList());
    auto rpnList = &m_rpnListStack.top();
//make sure is -1 if want to use >= type operators, 
    const int startOpLvl = -1;//-1 comparison operators
    bool lastWasANumber = false;

    std::stack<int> curOpLvlStack;
    curOpLvlStack.push(startOpLvl);
    int curOpLvl = curOpLvlStack.top();

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
#ifdef EVAL_DEBUG
        if(next.isOp)
            if(next.token < 30)
	    		LOG("op'",(int)next.token,"' : ");
            else
                LOG("op'", next.token, "' : ");
		else if(next.isConst)
			LOG("val'",next.value,"' : ");
        else
            LOG("val'",next.token,"' : ");
#endif
        if(next.type == RPNToken::TYPE::SYMBOL && next.token == '(')
        {//push Reset to initial conditions
            if(lastWasANumber)
                tryPush('*');

            opStack.push('(');

            pushReset();

            lastWasANumber = false;
        }
        else if(next.type == RPNToken::TYPE::OP)
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
                pushReset();
            }

            tryPush(next.token);
            lastWasANumber = false;
        }
        else //is a number one way or another
        {
            RPNToken numToken;
            
            if(next.type == RPNToken::TYPE::SYMBOL && next.token == ')')//closing bracket
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
            else if(next.type == RPNToken::TYPE::SYMBOL && next.token == ',')//argument list
            {
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
                //push it in the arguments list function stack level

                //pop to function rpnlist level
                m_rpnListStack.pop();
                rpnList = &m_rpnListStack.top();
                curOpLvl = curOpLvlStack.top();
                curOpLvlStack.pop();

                rpnList->emplace_back(numToken);

                //push back to () bracket level
                pushReset();
                
                lastWasANumber = false;
                continue;
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
                    RPNList tempRPNList;
                    tempRPNList.emplace_back(numToken);
                    while(!opStack.empty() && isUnary(opStack.top()))
                    {
                        tempRPNList.emplace_back(opStack.top(),OP);
                        opStack.pop();
                    }
                    numToken = RPNToken(tempRPNList);//new copy... or move???
                }
                else if(isAFunc(opStack.top()))//assumed funcargs cleared, as '(' was cleared
                {
                    
                    //NumToken holds my arguments!
                    rpnList->emplace_back(numToken);
                    rpnList->emplace_back(opStack.top(),OP);
                    opStack.pop();

                    //new Complex token to put in lower level list, done with the function
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

    return m_rpnListStack.top();
}

void ShuntYardAlgorithm::evaluate(std::stack<char>& opStack)
{//no try simplify here yet...
    auto &rpnList = m_rpnListStack.top();

    char op = opStack.top();
    opStack.pop();
    
    rpnList.emplace_back(op,OP);
}

}//namespace EVAL

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
*/