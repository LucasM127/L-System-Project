#include "Evaluator.hpp"
#include <sstream>
#include <iostream>

const float LVariable::getVal(const float *v, const unsigned int depth) const
{
	return v[m_varIndice.letterNum * depth + m_varIndice.paramNum];
}

const float Global::getVal(const float *v, const unsigned int depth) const
{
	return v[m_index];
}

ComplexEvaluator::ComplexEvaluator(const std::string& expression, unsigned int depth,
									std::vector<Number*>& numbers, std::vector< void(*)(std::stack<float>&) >& ops,
									std::vector<bool>& steps)
                                  :Evaluator(expression, depth),m_numList(numbers),opList(ops),isNextStepAPop(steps)
{}

ComplexEvaluator::~ComplexEvaluator()
{
    for(Number* numPtr : m_numList) delete numPtr;
}

//3 iterators is a bit complicated, but is ok.  
//can simplify if have to to reverse polish notation more,,, but then would have to dynamic cast instead
float ComplexEvaluator::evaluate(const float *v)
{
	unsigned int OpIt = 0;
	unsigned int NumIt = 0;

	for(bool isAPop : isNextStepAPop)
	{
		if(isAPop)
		{
		    std::cout<<"popping: ";//<<std::endl;
			opList[OpIt](stackNum);
			OpIt++;
		}
		else
		{
		    std::cout<<"pushing "<<m_numList[NumIt]->getVal(v, arrayDepth)<<std::endl;
			stackNum.push(m_numList[NumIt]->getVal(v, arrayDepth));
			NumIt++;
		}
	}

	float answer = stackNum.top();
	stackNum.pop();//don't forget to pop at the end
	return answer;
}

//http://bits.mdminhazulhaque.io/cpp/find-and-replace-all-occurrences-in-cpp-string.html
//or should i just use a vector , i have a feeling map will fail me with its sorting algorithm...
void findAndReplace(std::string& source, std::vector<std::pair<std::string, char> > &stringReplacementChars)
{
    std::string findString;
    char replaceChar;
    for(auto define : stringReplacementChars)
    {
        findString = define.first;
        replaceChar = define.second;
        for(std::string::size_type i = 0; (i = source.find(findString, i)) != std::string::npos;)
        {
            source.replace(i, findString.length(), &replaceChar, 1);
            ++i;//i += replaceString.length();
        }
    }
}

/*
bool next(const std::string &string, const unsigned int index, char &c)
{
    if(index+1 < string.size())
    {
        c = string[index+1];
        return true;
    }
    return false;
}*/

//add support for globals, a globalIndice map or something like that...
//multiple character variables??? ... propably can find/replace them beforehand with char values 128-255
//Rewrite??? so i know what it does as I forget...
void expand(const std::vector<Number*> &numList, const std::vector< void(*)(std::stack<float>&) > &opList, const std::vector<bool> &isNextStepAPop,
			std::vector<Number*> &numListFinal, std::vector< void(*)(std::stack<float>&) > &opListFinal, std::vector<bool> &isPopListFinal);

//unary of a variable is a complex number too
Evaluator* EvalLoader::load(const std::string &expression, std::map<char, VarIndice> &varIndiceMap, unsigned int depth)
{
	//don't delete our numList pointers as passed ownership to number
	while(!numListStack.empty()) numListStack.pop();
	while(!opListStack.empty())  opListStack.pop();
	while(!isNextStepAPopStack.empty()) isNextStepAPopStack.pop();

	numListStack.push(std::vector<Number*>());
	opListStack.push( std::vector< void(*)(std::stack<float>&) >());
	isNextStepAPopStack.push( std::vector<bool>());
	//numList.clear();
	//opList.clear();
	//isNextStepAPop.clear();
	auto numList = &numListStack.top();
	auto opList = &opListStack.top();
	auto isNextStepAPop = &isNextStepAPopStack.top();

	m_expression = expression;
	//Tokenize
	findAndReplace(m_expression, tokenStringReplaceVector);

	//as allow unary + - operands, but any more than one and my logic goes flawed
	if(m_expression.find("---") != std::string::npos)
		throw std::runtime_error("Invalid syntax");
	if(m_expression.find("+++") != std::string::npos)
		throw std::runtime_error("Invalid syntax");
	
	//++ --y decrement, increment operators? or multiple negate posate operators?
	//THINKING...

	float number;
	Number *numPtr;
	char lastOp, next;
	bool lastCharacterWasANumber = false;

	std::stringstream sstream;
	sstream<<m_expression;

	std::stack<char> opStack;

	auto tryPush = [&](char op)->void
	{
		if(!opStack.empty())
		{
			lastOp = opStack.top();//say last op is '('
			while(!nextOpHasPriority(op, lastOp) && lastOp != '(')//is next higher than last (negated)  is * > ( -> * <= (
			{
				evaluate(opStack);
				if(opStack.empty()) break;
				lastOp = opStack.top();
			}
		}
		opStack.push(op);
	};

	while (true)
	{
		next = sstream.peek();
		std::cout<<"Loading "<<next<<"\n";
		printout(*numList, *opList, *isNextStepAPop);

		if(next == EOF) break;

		if(next == ' ')
		{
			sstream.get(next);
			continue;
		}
//push a new stack frame thing to simplify, new numList, OpList, etc...
		if(next == '(')
		{
			//every time we try and push, we should ensure that nothing of higher precedence is below!
			if(lastCharacterWasANumber)//? sin90(...) (evaluate sin90 first before *)  or x(...) 5*(...) 5!(...)<!  when we evaluate it, the last character is a number...
			{
				tryPush('*');
			}
			opStack.push('(');

			numListStack.push(std::vector<Number*>());
			opListStack.push( std::vector< void(*)(std::stack<float>&) >());
			isNextStepAPopStack.push( std::vector<bool>());
			numList = &numListStack.top();
			opList = &opListStack.top();
			isNextStepAPop = &isNextStepAPopStack.top();

			sstream>>next;
			lastCharacterWasANumber = false;
			continue;
		}
//pop the stack frame thing... if size==1, push the number
//if size > 1, is a complex number, push the complex number, so simplification code won't think can just go into it without further checks
//is why (x+1)(2+2) tries to simplify 1 * 4 instead of (x+1) * 4
//what am i doing
		else if(next == ')')//evaluate it out to (
		{

			while (true)
			{
				if(opStack.top() == '(')
				{
					opStack.pop();
					break;
				}
				evaluate(opStack);
			}//try and simplify out.. but no '('

			//then see if is simple or complex, ... and put the number in as if is a regular number, with implied multiplication here
			//can simplify code to be just insert a number test code again...

			if(isNextStepAPop->size() > 1) numPtr = new ComplexNumber(*numList, *opList, *isNextStepAPop);
			else numPtr = numList->back();

			numListStack.pop();
			opListStack.pop();
			isNextStepAPopStack.pop();

			numList = &numListStack.top();
			opList = &opListStack.top();
			isNextStepAPop = &isNextStepAPopStack.top();
////////repeated
			lastOp = opStack.top();
			if(unaryOpMap.find(lastOp)!=unaryOpMap.end())
			{
				if(!numPtr->isConstant()){
				//is unary of a variable,
				opStack.pop();
				numPtr = new ComplexNumber({numPtr}, {unaryOpMap[lastOp]}, {false, true});
				}
				else
				{
					int asfdsf = 2;
				}
				
			}

			//insert number here
			numList->push_back(numPtr);
			isNextStepAPop->push_back(false);

			lastCharacterWasANumber = true;

			sstream>>next;
			continue;
		}
		else if(isAnOp(next))
		{
			//next is a unary right associative operator -x
			if(unaryOpMap.find(next)!=unaryOpMap.end() && lastCharacterWasANumber == true)
			{
				tryPush('*');
			}
			if(lastCharacterWasANumber == false)
			{
				if(next == '-')
					next = 10;//unary negate operator
				if(next == '+')
				{
					sstream>>next;
					continue;//unary positive operator does nothing
				}
			}
			//kind of works???  4sin90  but what about sin90x ? (insert a number after a number!)
			//xsin90  pushes the sin before the multiply though... i think this is right
			
			//unless last op was a unary...?
			//lastOp was 3(sin) next is 10(negate)
			//is greater or equal???
			if(opStack.size()) lastOp = opStack.top();
			if(opStack.size() && unaryOpMap.find(next)!=unaryOpMap.end() && unaryOpMap.find(lastOp)!=unaryOpMap.end())
			{
				opStack.push(next);
			}
			else
				tryPush(next);

			sstream>>next;
			lastCharacterWasANumber = false;
			continue;
		}
		else if(varIndiceMap.find(next)!=varIndiceMap.end())
		{
			numPtr = new LVariable(varIndiceMap[next]);

//REPEAT THIS CODE SECTION WITH ) TOO
			lastOp = opStack.top();
			if(unaryOpMap.find(lastOp)!=unaryOpMap.end())
			{
				//is unary of a variable,
				opStack.pop();
				numPtr = new ComplexNumber({numPtr}, {unaryOpMap[lastOp]}, {false, true});
			}


			numList->push_back(numPtr);//BEFORE THE * TEST????  Not doing that with the parenthesis....!!!!

			//DO THIS BEFORE Pushing the number
			//We may have to apply an operator of higher precedence.
			//(ie) 3^5x => (3^5)*x not 3^(5*x)
			if(lastCharacterWasANumber)
				tryPush('*');
			
			isNextStepAPop->push_back(false);
			lastCharacterWasANumber = true;
			sstream>>next;
		}
		else//assumed is numeric value else throw
		{
			if(!isdigit(next) && next != '.')
			{
				std::stringstream s;
				s<<"invalid token "<<next<<" ... can't prep expression "<<expression<<std::endl;
				throw std::runtime_error(s.str());
			}
			sstream>>number;//float
			numPtr = new Constant(number);
			
			//same logic as above
			numList->push_back(numPtr);

			if(lastCharacterWasANumber)
				tryPush('*');

			isNextStepAPop->push_back(false);
			lastCharacterWasANumber = true;
			//NO sstream next here
		}
	}
	
	while(!opStack.empty())
		evaluate(opStack);

	//opStack is empty here!
	//numStack is empty here!
	//use the vectors....
	//just append to final...
	std::vector<Number*> numListFinal;
	std::vector< void(*)(std::stack<float>&) > opListFinal;
	std::vector<bool> isPopListFinal;

	expand(*numList, *opList, *isNextStepAPop, numListFinal, opListFinal, isPopListFinal);

	printout(*numList, *opList, *isNextStepAPop);
	printout(numListFinal, opListFinal, isPopListFinal);
	/*
	//EXPAND IT OUT AT THE END
	//ALMOST LIKE A FAKE EVALUATOR...
	std::vector<bool> isPopF;
	std::stack< void(*)(std::stack<float>&) > opStackF;
	std::stack<Number*> numStackF;

	//recursively
	//how many times....
	//numList, opList... shouldn't we just copy it over instead???
	//expand(isNextStepAPop, opStack, numStack, isPopF, opStackF, numStackF)
	{
		for(auto pop : isPopF)
		{
			if(pop)
			{
				opStackF.push(opStack->top());
				opStack->pop();
			}
		}
	}
	*/

	//create our corresponding evaluator
	if((isNextStepAPop->size() == 1))
    {
        //return new SimpleEvaluator(expression, depth, (*numList)[0]);
		return new SimpleEvaluator(expression, depth, numListFinal[0]);
    }
//	else return new ComplexEvaluator(expression,depth,*numList,*opList,*isNextStepAPop);
	else return new ComplexEvaluator(expression,depth,numListFinal,opListFinal,isPopListFinal);
}

void expand(const std::vector<Number*> &numList, const std::vector< void(*)(std::stack<float>&) > &opList, const std::vector<bool> &isNextStepAPop,
			std::vector<Number*> &numListFinal, std::vector< void(*)(std::stack<float>&) > &opListFinal, std::vector<bool> &isPopListFinal)
{
	uint numIt = 0;
	uint opIt = 0;
	for(bool isPop : isNextStepAPop)
	{
		if(isPop)
		{
			//pop it...
			opListFinal.push_back(opList[opIt++]);
			isPopListFinal.push_back(isPop);
		}
		else
		{
			Number *N = numList[numIt++];
			if(N->isComplex())
			{
				ComplexNumber *CN = dynamic_cast<ComplexNumber*>(N);
				expand(CN->numList, CN->opList, CN->isNextStepAPop, numListFinal, opListFinal, isPopListFinal);
			}
			else
			{
				numListFinal.push_back(N);
				isPopListFinal.push_back(isPop);
			}
		}
	}
}

Evaluator* EvalLoader::load(const float value)
{
    Number* numPtr = new Constant(value);
    return new SimpleEvaluator("default value",0,numPtr);
}

//simplify functions simplify the expression if the operation is acting on constants only, to reduce number of steps
bool EvalLoader::simplifyBinary(void(*op)(std::stack<float>& ))
{
	auto &numList = numListStack.top();
	auto &isNextStepAPop = isNextStepAPopStack.top();

    int size = numList.size();
    Number* rhsNum = numList[size-1];
    Number* lhsNum = numList[size-2];
    if(rhsNum->isConstant() && lhsNum->isConstant())
    {
		std::cout<<"Simplifying binary op: ";
        stackNum.push(lhsNum->getVal());
        stackNum.push(rhsNum->getVal());
        op(stackNum);
        Number* newNum = new Constant( stackNum.top() );
        stackNum.pop();
        delete rhsNum;
        delete lhsNum;
        numList[size-2] = newNum;
        numList.resize(size-1);
        isNextStepAPop.resize(isNextStepAPop.size()-1);//had pushed two values for this op to pop, but now we won't need to.
        return true;
    }
    return false;
}

bool EvalLoader::simplifyUnary (void(*op)(std::stack<float>& ))
{
	auto &numList = numListStack.top();

    int size = numList.size();
    Number* oldNum = numList[size-1];
    if(oldNum->isConstant())
    {
		std::cout<<"Simplifying unary op: ";
        stackNum.push(oldNum->getVal());
        op(stackNum);
        Number* newNum = new Constant( stackNum.top() );
        stackNum.pop();
        delete oldNum;
        numList[size-1] = newNum;
        return true;
    }
    return false;
}

//push the numbers in the stack only once, pull as needed...
void EvalLoader::evaluate(std::stack<char>& stackOp)
{
	auto &opList = opListStack.top();
	auto &isNextStepAPop = isNextStepAPopStack.top();
	//use reverse stack to ... for 5x3 => 15x or x15
	char operation = stackOp.top();
	stackOp.pop();
	char lastOp = operation;//INITIALIZED from visual studio runtime error
	if(stackOp.size()) lastOp = stackOp.top();//lastOp or is it nextOp?

	bool wasSimplified = false;

	auto opMapIt = binaryOpMap.find(operation);
	if(opMapIt != binaryOpMap.end())
    {
        //only try and simplify if is priority, otherwise ie) (x+1)*2 will try and simplify 1*2 even though need to add x+1 first with x being a variable
		//we have higher priority than ( though???
        if(opPriorityMap[operation]>=opPriorityMap[lastOp])//!nextOpHasPriority(operation,lastOp))
        {
            if(!simplifyBinary(opMapIt->second)) opList.push_back(opMapIt->second);
            else wasSimplified = true;
        }
        else
        {
            opList.push_back(opMapIt->second);
        }
    }
    else
    {
        opMapIt = unaryOpMap.find(operation);
        if(opMapIt == unaryOpMap.end())
        {
            std::stringstream s;
            s<<"Invalid operator "<<operation<<" in: "<<m_expression<<std::endl;
            throw std::runtime_error(s.str());
        }
        //Unary ops if can be simplified won't mess up the priority I don't think?
        if(!simplifyUnary(opMapIt->second)) opList.push_back(opMapIt->second);
        else wasSimplified = true;
    }

	if(!wasSimplified) isNextStepAPop.push_back(true);
}

//other option just assign op priority ranking values to the ops
//next * last '(' 3 > 99 false
//should always return true
bool EvalLoader::nextOpHasPriority(char nextOp, char lastOp)
{
//less than, else if >= will push it and store it, not evaluate as we find, (5-(3-2)=>4 instead of (5-3)-2)
	return(opPriorityMap[nextOp] > opPriorityMap[lastOp]);//use >= for right associativity

	if(lastOp == '(') return true;//so should never have to evaluate to left of it until is cleared by ')'
	if((nextOp == '*' || nextOp == '/') && (lastOp == '+' || lastOp == '-')) return true;
	if(nextOp == '^') return true;//raise the exponent and then multiply it out
	if(lastOp == '<' || lastOp ==  '>' || lastOp ==  '=') return true;//everything else has priority...
	//if(lastOp == 'S') return false;//redundant? Unary ops we just evaluate as is
	return false;
}

bool EvalLoader::isAnOp(const char& op)// + - * / ^ S C T = < > R <- CurrentOpList
{
    if(binaryOpMap.find(op)!= binaryOpMap.end() ||
       unaryOpMap.find(op) != unaryOpMap.end() )
        return true;
    return false;
}


void EvalLoader::printout(const std::vector<Number*> &numList, std::vector< void(*)(std::stack<float>&) > &opList, const std::vector<bool> &popList)
{
	std::cout<<"NumList: ";
	for(auto N : numList)
	{
		if(N->isConstant()) std::cout<<N->getVal()<<" ";
		else std::cout<<"X ";
	}
	std::cout<<"\nOpList: ";
	for(auto O : opList)
	{
		std::cout<<reverseOpMap[O]<<" ";
	}
	std::cout<<"\npopList: ";
	for(auto B : popList)
	{
		if(B) std::cout<<"pop ";
		else std::cout<<"push ";
	}
	std::cout<<"\n";
}

/*
Evaluator* EvalLoader::load(const std::string& expression, std::map<char, VarIndice>& varIndiceMap, unsigned int depth)
{
    numList.clear();
    opList.clear();
    isNextStepAPop.clear();

    m_expression = expression;
	//prep sentence, replace operator string tokens with character tokens...
	//TOKENIZE
	findAndReplace(m_expression, tokenStringReplaceVector);

	float number;
	Number* numPtr;
	char lastOp, next;
	bool lastCharacterWasANumber = false;
	std::stringstream sstream;
	sstream<<m_expression;
	std::stack<char> stackOp;
	
	while(true)
	{
		next = sstream.peek();

		if(next == EOF) break;

		/ *if(next == ' ') //removed spaces in lsloader parser
		{
			sstream.get(next);
			continue;
		}* /

		if(next == '(')//starting into brackets...
		{
			if(lastCharacterWasANumber) stackOp.push('*');//implied multiplication 5(2) same as 5*2
			stackOp.push(next);
			sstream>>next;
			lastCharacterWasANumber = false;
			continue;
		}
		else if(next == ')')//ensure what is in brackets is all solved first before continuing
		{
			/ *if(!stackOp.size())
			{//as there HAS to be a '(' already in the expression from before
				std::cout<<"Mismatched ( brackets ). Unable to prep expression "<<myExpression<<std::endl;
				return false;
			}* /
			while(true)
			{
				if(stackOp.top() == '(')
				{
					stackOp.pop();
					break;//done
				}
				evaluate(stackOp);
			}

			sstream>>next;
			lastCharacterWasANumber = true;
		}
		else if(isAnOp(next))
		{
		    if(!lastCharacterWasANumber)
            {
                if(next == '-')//ie -sinX //??? +-1 or -1 + -4 same as 0-1 or 0-1 + 0-4
                {//inserts implied zero
					next = 10;//the unary negate operator
					/ *
                    numList.push_back(new Constant(0.f));
                    isNextStepAPop.push_back(false);
                    lastCharacterWasANumber = true;
                    continue;* /
                }
            }
			/ *
			else
			{
				//ie 5sin30 or 2cos,, only if a unary
				//so far all unary ops right associative
				//need to push a * then push the sin so sin is on top???
				//or is it push sin then push *
				
				if(unaryOpMap.find(next)!=unaryOpMap.end())
				{
					if(!stackOp.empty())
				{
					lastOp = stackOp.top();
					while(!nextOpHasPriority('*',lastOp))
					{
						evaluate(stackOp);
						if(stackOp.empty()) break;
						lastOp = stackOp.top();
					}
				}
				stackOp.push('*');
				}
				stackOp.push(next);
				stackOp.push('*');
//					stackOp.push('*');
//				if(isUnary(next)) stackOp.push('*');
				}
			* /

			if(!stackOp.empty())
			{
				lastOp = stackOp.top();
				while(!nextOpHasPriority(next,lastOp))
				{
					evaluate(stackOp);
					if(stackOp.empty()) break;
					lastOp = stackOp.top();
				}
			}
			stackOp.push(next);

			//this kind of works..., KIND OF>>>
			if(lastCharacterWasANumber)
			{
				if(unaryOpMap.find(next)!=unaryOpMap.end())
				{
					//no eval???
					stackOp.push('*');
				}
			}

			sstream>>next;
			lastCharacterWasANumber = false;
		}
		else if(varIndiceMap.find(next)!=varIndiceMap.end())
		{
			//we create a new variable with it's associated location...//yeah, then can pass our currentVariables later
			numPtr = new LVariable(varIndiceMap[next]);
			numList.push_back(numPtr);
			isNextStepAPop.push_back(false);

			if(lastCharacterWasANumber)//ie 5y is same as 5*y
			{
				if(!stackOp.empty())
				{
					lastOp = stackOp.top();
					while(!nextOpHasPriority('*',lastOp))
					{
						evaluate(stackOp);
						if(stackOp.empty()) break;
						lastOp = stackOp.top();
					}
				}
				stackOp.push('*');
			}
			lastCharacterWasANumber = true;
			sstream>>next;
		}
		else//assumed is a number value...
		{//do same thing here as above with the variables...
			if(next < '0' || next > '9')
			{
			    if(next != '.'){
				//error... throw an exception?
				std::stringstream s;
				s<<"invalid variable "<<next<<" ... can't prep expression "<<expression<<std::endl;
				throw std::runtime_error(s.str());}
			}
			sstream>>number;//MAGIC
			numPtr = new Constant(number);
			numList.push_back(numPtr);
			isNextStepAPop.push_back(false);

			if(lastCharacterWasANumber)//ie y5 -> y*5
			{
				if(!stackOp.empty())
				{
					lastOp = stackOp.top();
					while(!nextOpHasPriority('*',lastOp))
					{
						evaluate(stackOp);
						if(stackOp.empty()) break;
						lastOp = stackOp.top();
					}
				}
				stackOp.push('*');
			}
			lastCharacterWasANumber = true;
		}
	}

	while(!stackOp.empty())
	{
		evaluate(stackOp);
	}

	//create our corresponding evaluator
	if((isNextStepAPop.size() == 1))
    {
        return new SimpleEvaluator(expression, depth, numList[0]);
    }
	else return new ComplexEvaluator(expression,depth,numList,opList,isNextStepAPop);
}
*/