#include "EvalLoader.hpp"
#include <cmath>
#include <random>

#include <Logger.hpp>

const float DegToRad = 0.01745329252;

//sorted by decreasing string size (can't use a map)
std::vector< std::pair<std::string, char> > EvalLoader::tokenStringReplaceVector =
{
	{"cotangent",	8},
	{"cosecant",	6 },
	{"rand()%",		9 },
	{"secant", 		7 },
	{"cosec",		6 },
	{"cotan", 		8 },
	{"asin", 		6 },
	{"acos", 		7 },
	{"atan", 		8 },
	{"sinf",  		3 },
	{"cosf",  		4 },
	{"cot", 		8 },
	{"csc",			6 },
	{"sec", 		7 },
	{"pow", 	   '^'},
	{"sin",  		3 },
	{"cos",  		4 },
	{"tan",  		5 },
	{"==",			0 },
	{">=",			1 },
	{"<=",			2 },
	//if lhs(!= 1 && rhs!=1 
	{"&&",			11},
	{"||",			12}
};

std::map<char, int> EvalLoader::opPriorityMap
{
//	{')',  -1},//NEVER EVEN ON THE STACK
	{ 0 ,	0},// ==
	{ 1 ,	0},// >=
	{ 2 ,   0},// <=
	{'>',   0},
	{'<',   0},
	{'+',	1},
	{'-',	1},
	{'*',	2},
	{'/',	2},
	{'^',	3},
	{ 3 ,   4},//unary operators 3-10
	{ 4 ,   4},
	{ 5 ,   4},
	{ 6 ,   4},
	{ 7 ,   4},
	{ 8 ,   4},
	{ 9 ,   4},
	{ 10,   4},
	{ 11,  -1},//&&//Need to evaluat > < first before this...
	{ 12,  -1}//||
	//{'(',	-1}//so will apply
};

//fix the while loop parsing... for this...
std::map<char, bool> EvalLoader::opLeftAssociativityMap =
{
	{ 0 ,	true},// ==
	{ 1 ,	true},// >=
	{ 2 ,   true},// <=
	{'>',   true},
	{'<',   true},
	{'+',	true},
	{'-',	true},
	{'*',	true},
	{'/',	true},
	{'^',	false},//ie 2^3^4 != (2^3)^4 4096 = 2,14e29
	{ 3 ,   false},//unary operators 3-10
	{ 4 ,   false},
	{ 5 ,   false},
	{ 6 ,   false},
	{ 7 ,   false},
	{ 8 ,   false},
	{ 9 ,   false},
	{ 10,   false},
	{ 11,	true},//&&
	{ 12,	true}//||
};


//uses ascii code range 0-31 (unprintable control characters)
//for where the tokens do not have a direct single character equivalent
std::map<char,opFnPtr> EvalLoader::binaryOpMap =
{
    {'+' , Evaluator::add},
    {'-' , Evaluator::subtract},
    {'*' , Evaluator::multiply},
    {'/' , Evaluator::divide},
    {'^' , Evaluator::raiseByExponent},
    { 0  , Evaluator::testIfEqual},
    {'>' , Evaluator::testIfGreaterThan},
    {'<' , Evaluator::testIfLessThan},
	{ 1  , Evaluator::testIfGreaterOrEqualThan},
    { 2  , Evaluator::testIfLessThanOrEqual},
	{ 11 , Evaluator::testAnd},
	{ 12 , Evaluator::testOr}
};

//associativity???  so far all right associative...
std::map<char,opFnPtr> EvalLoader::unaryOpMap =
{
    { 3  , Evaluator::sin},
    { 4  , Evaluator::cos},
    { 5  , Evaluator::tan},
	{ 6  , Evaluator::cosecant},
    { 7  , Evaluator::secant},
    { 8  , Evaluator::cotangent},
    { 9  , Evaluator::random},
	{ 10 , Evaluator::negate}
};

//Map reverse map??? Function -> opPrority

void Evaluator::add(std::stack<float>& numberStack)// '+'
{
	float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	LOG("Adding ", lhs, " and ", rhs);
	//std::cout<<"adding "<<lhs<<" and "<<rhs<<std::endl;
	numberStack.push(lhs + rhs);
}

void Evaluator::subtract(std::stack<float>& numberStack)//-
{
	float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	LOG("Subtracting ", lhs, " and ", rhs);
	//std::cout<<"subtracting "<<lhs<<" minus "<<rhs<<std::endl;
	numberStack.push(lhs - rhs);
}

void Evaluator::multiply(std::stack<float>& numberStack)//*
{
	float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	LOG("Multiplying ", lhs, " and ", rhs);
	//std::cout<<"multiplying "<<lhs<<" and "<<rhs<<std::endl;
	numberStack.push(lhs * rhs);
}

void Evaluator::divide(std::stack<float>& numberStack)// /
{
    float rhs = numberStack.top();
    numberStack.pop();
    float lhs = numberStack.top();
    numberStack.pop();
    LOG("Dividing ", lhs, " and ", rhs);
	//std::cout<<"dividing "<<lhs<<" by "<<rhs<<std::endl;
    //if(rhs = 0.f) numberStack.push
    numberStack.push(lhs / rhs);
}

void Evaluator::raiseByExponent(std::stack<float>& numberStack)//^
{
	float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	LOG("Raising ", lhs, " to the power of ", rhs);
	//std::cout<<"raising "<<lhs<<" to the power of "<<rhs<<std::endl;
	numberStack.push(pow(lhs,rhs));
}

void Evaluator::sin(std::stack<float>& numberStack)//S
{
	float num = numberStack.top();
	numberStack.pop();
	LOG("Taking the sin of ", num);
	//std::cout<<"taking the sin of "<<num<<std::endl;
	numberStack.push(sinf(num * DegToRad));
}

void Evaluator::cos(std::stack<float>& numberStack)//C
{
	float num = numberStack.top();
	numberStack.pop();
	LOG("Taking the cos of ", num);
	//std::cout<<"taking the cos of "<<num<<std::endl;
	numberStack.push(cosf(num * DegToRad));
}

void Evaluator::tan(std::stack<float>& numberStack)//T
{
	float num = numberStack.top();
	numberStack.pop();
	LOG("Taking the tan of ", num);
	//std::cout<<"taking the tan of "<<num<<std::endl;
	numberStack.push(tanf(num * DegToRad));
}//use op ... command style ?

void Evaluator::cosecant(std::stack<float>& numberStack)//S
{
	float num = numberStack.top();
	numberStack.pop();
	LOG("Taking the cosecant of ", num);
	//std::cout<<"taking the cosecant of "<<num<<std::endl;
	numberStack.push(asinf(num * DegToRad));
}
//abs() |x|

void Evaluator::secant(std::stack<float>& numberStack)//C
{
	float num = numberStack.top();
	numberStack.pop();
	LOG("Taking the secant of ", num);
	//std::cout<<"taking the secant of "<<num<<std::endl;
	numberStack.push(acosf(num * DegToRad));
}

void Evaluator::cotangent(std::stack<float>& numberStack)//T
{
	float num = numberStack.top();
	numberStack.pop();
	LOG("Taking the cotangent of ", num);
	//std::cout<<"taking the cotangent of "<<num<<std::endl;
	numberStack.push(atanf(num * DegToRad));
}

void Evaluator::random(std::stack<float>& numberStack)//R
{
	float num = numberStack.top();
	numberStack.pop();
	float r = float(rand())/float(RAND_MAX) * num;
	r = roundf(r);
	numberStack.push(r);
//	if(num <= 0) numberStack.push(0.f);
//	else numberStack.push(rand()%num);
	LOG("Rand ", num);
}


void Evaluator::negate(std::stack<float>& numberStack)// _0
{
	float num = numberStack.top();
	numberStack.pop();
	LOG("Negating ", num);
	//std::cout<<"negating "<<num<<std::endl;
	numberStack.push(-num);
}

void Evaluator::testIfEqual(std::stack<float>& numberStack)//=
{
    float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	LOG("Testing if ", lhs, " equals ", rhs);
	//std::cout<<"testing if "<<lhs<<" equals "<<rhs<<std::endl;
	(roundf(lhs) == roundf(rhs)) ? numberStack.push(1) : numberStack.push(0);
}

void Evaluator::testIfGreaterThan(std::stack<float>& numberStack)//>
{
    float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	LOG("Testing if ", lhs, " is greater than ", rhs);
	//std::cout<<"testing if "<<lhs<<" is greater than "<<rhs<<std::endl;
	(roundf(lhs) > roundf(rhs)) ? numberStack.push(1) : numberStack.push(0);
}

void Evaluator::testIfLessThan(std::stack<float>& numberStack)//<
{
    float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	LOG("Testing if ", lhs, " is less than ", rhs);
	//std::cout<<"testing if "<<lhs<<" is less than "<<rhs<<std::endl;
	(roundf(lhs) < roundf(rhs)) ? numberStack.push(1) : numberStack.push(0);
}

void Evaluator::testIfGreaterOrEqualThan(std::stack<float>& numberStack)
{
	float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	LOG("Testing if ", lhs, " is greater or equal to ", rhs);
	//std::cout<<"testing if "<<lhs<<" is greater than "<<rhs<<std::endl;
	(roundf(lhs) >= roundf(rhs)) ? numberStack.push(1) : numberStack.push(0);
}

void Evaluator::testIfLessThanOrEqual(std::stack<float>& numberStack)
{
	float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	LOG("Testing if ", lhs, " is less or equal to ", rhs);
	//std::cout<<"testing if "<<lhs<<" is less than "<<rhs<<std::endl;
	(roundf(lhs) <= roundf(rhs)) ? numberStack.push(1) : numberStack.push(0);
}

void Evaluator::testAnd(std::stack<float>& numberStack)
{
	float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	LOG("Testing if ", lhs, " is less or equal to ", rhs);
	//std::cout<<"testing if "<<lhs<<" is less than "<<rhs<<std::endl;
	(roundf(lhs) && roundf(rhs)) ? numberStack.push(1) : numberStack.push(0);
}

void Evaluator::testOr(std::stack<float>& numberStack)
{
	float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	LOG("Testing if ", lhs, " is less or equal to ", rhs);
	//std::cout<<"testing if "<<lhs<<" is less than "<<rhs<<std::endl;
	(roundf(lhs) || roundf(rhs)) ? numberStack.push(1) : numberStack.push(0);
}