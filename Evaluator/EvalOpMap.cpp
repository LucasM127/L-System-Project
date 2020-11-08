#include "Evaluator.hpp"
#include <cmath>
#include <random>

#include <iostream>

const float DegToRad = 0.01745329252;

//sorted by decreasing string size (can't use a map)
std::vector< std::pair<std::string, char> > EvalLoader::tokenStringReplaceVector =
{
	{"cotangent",	8},
	{"cosecant",	6 },
	{"secant", 		7 },
	{"cosec",		6 },
	{"cotan", 		8 },
	{"asin", 		6 },
	{"acos", 		7 },
	{"atan", 		8 },
	{"cot", 		8 },
	{"csc",			6 },
	{"sec", 		7 },
	{"pow", 	   '^'},
	{"sin",  		3 },
	{"cos",  		4 },
	{"tan",  		5 },
	{"==",			0 },
	{">=",			1 },
	{"<=",			2 }
};

std::map<char, std::string> EvalLoader::reverseTokenMapToCFunction = 
{
	{0, "=="},
	{1, ">="},
	{2, "<="},
	{3, "sinf"},
	{4, "cosf"},
	{5, "tanf"},
	{6, "asinf"},
	{7, "acosf"},
	{8, "atanf"},
	{9, "rand()%"},
	{10, "-"}
};

std::map<char, int> EvalLoader::opPriorityMap
{
//	{')',  -1},//NEVER EVEN ON THE STACK
	{ 0 ,	0},// ==
	{ 1 ,	0},// >=
	{ 2 ,   0},// <=
	{'+',	1},
	{'-',	1},
	{'*',	2},
	{'/',	2},
	{'^',	3},
	{ 3 ,   5},//unary operators 3-10
	{ 4 ,   5},
	{ 5 ,   5},
	{ 6 ,   5},
	{ 7 ,   5},
	{ 8 ,   5},
	{ 9 ,   5},
	{ 10,   5},
	{'(',	-1}//so will apply
};

//fix the while loop parsing... for this...
std::map<char, bool> EvalLoader::opLeftAssociativityMap =
{
	{ 0 ,	true},// ==
	{ 1 ,	true},// >=
	{ 2 ,   true},// <=
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
	{ 10,   false}
};

std::map<void(*)(std::stack<float>&), std::string> EvalLoader::reverseOpMap = 
{
	{Evaluator::add							,"+"},
    {Evaluator::subtract					,"-"},
    {Evaluator::multiply					,"*"},
    {Evaluator::divide						,"/"},
    {Evaluator::raiseByExponent				,"^"},
    {Evaluator::testIfEqual					,"==" },
    {Evaluator::testIfGreaterThan			,">"},
    {Evaluator::testIfLessThan				,"<"},
	{Evaluator::testIfGreaterOrEqualThan	,">="},
    {Evaluator::testIfLessThanOrEqual		,"<="},
	{Evaluator::sin							,"sin"},
    {Evaluator::cos							,"cos"},
    {Evaluator::tan							,"tan"},
	{Evaluator::cosecant					,"csc"},
    {Evaluator::secant						,"sec"},
    {Evaluator::cotangent					,"cot"},
    {Evaluator::random						,"rnd"},
	{Evaluator::negate						,"neg"}
};
/*
std::map<void(*)(std::stack<float>&), char> reverseOpPriorityMap = 
{
	{Evaluator::add							,'+'},
    {Evaluator::subtract					,'-'},
    {Evaluator::multiply					,'*'},
    {Evaluator::divide						,'/'},
    {Evaluator::raiseByExponent				,'^'},
    {Evaluator::testIfEqual					, 0 },
    {Evaluator::testIfGreaterThan			,'>'},
    {Evaluator::testIfLessThan				,'<'},
	{Evaluator::testIfGreaterOrEqualThan	, 1 },
    {Evaluator::testIfLessThanOrEqual		, 2 },
	{Evaluator::sin							, 3 },
    {Evaluator::cos							, 4 },
    {Evaluator::tan							, 5 },
	{Evaluator::cosecant					, 6 },
    {Evaluator::secant						, 7 },
    {Evaluator::cotangent					, 8 },
    {Evaluator::random						, 9 },
	{Evaluator::negate						, 10}
};
*/
//uses ascii code range 0-31 (unprintable control characters)
//for where the tokens do not have a direct single character equivalent
std::map<char,void(*)(std::stack<float>&)> EvalLoader::binaryOpMap =
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
    { 2  , Evaluator::testIfLessThanOrEqual}
};

//associativity???  so far all right associative...
std::map<char,void(*)(std::stack<float>&)> EvalLoader::unaryOpMap =
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
	std::cout<<"adding "<<lhs<<" and "<<rhs<<std::endl;
	numberStack.push(lhs + rhs);
}

void Evaluator::subtract(std::stack<float>& numberStack)//-
{
	float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	std::cout<<"subtracting "<<lhs<<" minus "<<rhs<<std::endl;
	numberStack.push(lhs - rhs);
}

void Evaluator::multiply(std::stack<float>& numberStack)//*
{
	float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	std::cout<<"multiplying "<<lhs<<" and "<<rhs<<std::endl;
	numberStack.push(lhs * rhs);
}

void Evaluator::divide(std::stack<float>& numberStack)// /
{
    float rhs = numberStack.top();
    numberStack.pop();
    float lhs = numberStack.top();
    numberStack.pop();
    std::cout<<"dividing "<<lhs<<" by "<<rhs<<std::endl;
    //if(rhs = 0.f) numberStack.push
    numberStack.push(lhs / rhs);
}

void Evaluator::raiseByExponent(std::stack<float>& numberStack)//^
{
	float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	std::cout<<"raising "<<lhs<<" to the power of "<<rhs<<std::endl;
	numberStack.push(pow(lhs,rhs));
}

void Evaluator::sin(std::stack<float>& numberStack)//S
{
	float num = numberStack.top();
	numberStack.pop();
	std::cout<<"taking the sin of "<<num<<std::endl;
	numberStack.push(sinf(num * DegToRad));
}

void Evaluator::cos(std::stack<float>& numberStack)//C
{
	float num = numberStack.top();
	numberStack.pop();
	std::cout<<"taking the cos of "<<num<<std::endl;
	numberStack.push(cosf(num * DegToRad));
}

void Evaluator::tan(std::stack<float>& numberStack)//T
{
	float num = numberStack.top();
	numberStack.pop();
	std::cout<<"taking the tan of "<<num<<std::endl;
	numberStack.push(tanf(num * DegToRad));
}//use op ... command style ?

void Evaluator::cosecant(std::stack<float>& numberStack)//S
{
	float num = numberStack.top();
	numberStack.pop();
	std::cout<<"taking the cosecant of "<<num<<std::endl;
	numberStack.push(asinf(num * DegToRad));
}
//abs() |x|

void Evaluator::secant(std::stack<float>& numberStack)//C
{
	float num = numberStack.top();
	numberStack.pop();
	std::cout<<"taking the secant of "<<num<<std::endl;
	numberStack.push(acosf(num * DegToRad));
}

void Evaluator::cotangent(std::stack<float>& numberStack)//T
{
	float num = numberStack.top();
	numberStack.pop();
	std::cout<<"taking the cotangent of "<<num<<std::endl;
	numberStack.push(atanf(num * DegToRad));
}

void Evaluator::random(std::stack<float>& numberStack)//R
{
	int num = numberStack.top();
	numberStack.pop();
	if(num <= 0) numberStack.push(0.f);
	else numberStack.push(rand()%num);
}


void Evaluator::negate(std::stack<float>& numberStack)// _0
{
	float num = numberStack.top();
	numberStack.pop();
	std::cout<<"negating "<<num<<std::endl;
	numberStack.push(-num);
}

void Evaluator::testIfEqual(std::stack<float>& numberStack)//=
{
    float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	//std::cout<<"testing if "<<lhs<<" equals "<<rhs<<std::endl;
	(roundf(lhs) == roundf(rhs)) ? numberStack.push(1) : numberStack.push(0);
}

void Evaluator::testIfGreaterThan(std::stack<float>& numberStack)//>
{
    float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	//std::cout<<"testing if "<<lhs<<" is greater than "<<rhs<<std::endl;
	(roundf(lhs) > roundf(rhs)) ? numberStack.push(1) : numberStack.push(0);
}

void Evaluator::testIfLessThan(std::stack<float>& numberStack)//<
{
    float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	//std::cout<<"testing if "<<lhs<<" is less than "<<rhs<<std::endl;
	(roundf(lhs) < roundf(rhs)) ? numberStack.push(1) : numberStack.push(0);
}

void Evaluator::testIfGreaterOrEqualThan(std::stack<float>& numberStack)
{
	float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	//std::cout<<"testing if "<<lhs<<" is greater than "<<rhs<<std::endl;
	(roundf(lhs) >= roundf(rhs)) ? numberStack.push(1) : numberStack.push(0);
}

void Evaluator::testIfLessThanOrEqual(std::stack<float>& numberStack)
{
	float rhs = numberStack.top();
	numberStack.pop();
	float lhs = numberStack.top();
	numberStack.pop();
	//std::cout<<"testing if "<<lhs<<" is less than "<<rhs<<std::endl;
	(roundf(lhs) <= roundf(rhs)) ? numberStack.push(1) : numberStack.push(0);
}