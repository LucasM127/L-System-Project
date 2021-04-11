#include "EvalLoader.hpp"
#include "RPNListFuncs.hpp"
#include "Evaluator.hpp"

#include <cmath>
#include <random>

#include "../Logger/Logger.hpp"

namespace EVAL
{

const float DegToRad = 0.01745329252f;

//Shunting Yard
std::map<char, int> opPriorityMap
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
    { 12,  -1},//||
    { 13,   4}//max(a,b) like unary?
    //{'(',	-1}//so will apply
};

//fix the while loop parsing... for this...
std::map<char, bool> opLeftAssociativityMap =
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
    { 12,	true},//||
    { 13,   true}//f1 f2 a , b , c => a b f2 c f1 (left associative)
};

//uses ascii code range 0-31 (unprintable control characters)
//for where the tokens do not have a direct single character equivalent
std::map<char,opFnPtr> binaryOpMap =
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
std::map<char,opFnPtr> unaryOpMap =
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

std::map<char,opFnPtr> funcOpMap =
{
    { 13 , Evaluator::maxFn}
};
//Map reverse map??? Function -> opPrority

void Evaluator::add(float *stackVals, unsigned int &top)// '+'
{
    --top;
    float rhs = stackVals[top];
    float lhs = stackVals[top-1];
    LOG("Adding ", lhs, " and ", rhs);
    stackVals[top-1] = lhs + rhs;
}

void Evaluator::subtract(float *stackVals, unsigned int &top)//-
{
    --top;
    float rhs = stackVals[top];
    float lhs = stackVals[top-1];
    LOG("Subtracting ", lhs, " and ", rhs);
    stackVals[top-1] = lhs - rhs;
}

void Evaluator::multiply(float *stackVals, unsigned int &top)//*
{//it should be a zero...
    --top;
    float rhs = stackVals[top];
    float lhs = stackVals[top-1];
    LOG("Multiplying ", lhs, " and ", rhs);
    stackVals[top-1] = lhs * rhs;
}

void Evaluator::divide(float *stackVals, unsigned int &top)// /
{
    --top;
    float rhs = stackVals[top];
    float lhs = stackVals[top-1];
    LOG("Dividing ", lhs, " and ", rhs);
    //if(rhs = 0.f) numberStack.push
    stackVals[top-1] = lhs / rhs;
}

void Evaluator::raiseByExponent(float *stackVals, unsigned int &top)//^
{
    --top;
    float rhs = stackVals[top];
    float lhs = stackVals[top-1];
    LOG("Raising ", lhs, " to the power of ", rhs);
    stackVals[top-1] = powf(lhs,rhs);
}

void Evaluator::sin(float *stackVals, unsigned int &top)//S
{
    float num = stackVals[top-1];
    LOG("Taking the sin of ", num);
    stackVals[top-1] = sinf(num * DegToRad);
}

void Evaluator::cos(float *stackVals, unsigned int &top)//C
{
    float num = stackVals[top-1];
    LOG("Taking the cos of ", num);
    stackVals[top-1] = cosf(num * DegToRad);
}

void Evaluator::tan(float *stackVals, unsigned int &top)//T
{
    float num = stackVals[top-1];
    LOG("Taking the tan of ", num);
    stackVals[top-1] = tanf(num * DegToRad);
}//use op ... command style ?

void Evaluator::cosecant(float *stackVals, unsigned int &top)//S
{
    float num = stackVals[top-1];
    LOG("Taking the cosecant of ", num);
    stackVals[top-1] = asinf(num * DegToRad);
}
//abs() |x|

void Evaluator::secant(float *stackVals, unsigned int &top)//C
{
    float num = stackVals[top-1];
    LOG("Taking the secant of ", num);
    stackVals[top-1] = acosf(num * DegToRad);
}

void Evaluator::cotangent(float *stackVals, unsigned int &top)//T
{
    float num = stackVals[top-1];
    LOG("Taking the cotangent of ", num);
    stackVals[top-1] = atanf(num * DegToRad);
}

void Evaluator::random(float *stackVals, unsigned int &top)//R
{
    float num = stackVals[top-1];
    LOG("Rand ", num);
    float r = float(rand())/float(RAND_MAX) * num;
    stackVals[top-1] = roundf(r);
}


void Evaluator::negate(float *stackVals, unsigned int &top)// _0
{
    float num = stackVals[top-1];
    LOG("Negating ", num);
    stackVals[top-1] = -num;
}

//fix with more float friendly functions later
void Evaluator::testIfEqual(float *stackVals, unsigned int &top)//=
{
    --top;
    float rhs = stackVals[top];
    float lhs = stackVals[top-1];
    LOG("Testing if ", lhs, " equals ", rhs);
    stackVals[top-1] = (roundf(lhs) == roundf(rhs)) ? 1 : 0;
}

void Evaluator::testIfGreaterThan(float *stackVals, unsigned int &top)//>
{
    --top;
    float rhs = stackVals[top];
    float lhs = stackVals[top-1];
    LOG("Testing if ", lhs, " is greater than ", rhs);
    stackVals[top-1] = (roundf(lhs) > roundf(rhs)) ? 1 : 0;
}

void Evaluator::testIfLessThan(float *stackVals, unsigned int &top)//<
{
    --top;
    float rhs = stackVals[top];
    float lhs = stackVals[top-1];
    LOG("Testing if ", lhs, " is less than ", rhs);
    stackVals[top-1] = (roundf(lhs) < roundf(rhs)) ? 1 : 0;
}

void Evaluator::testIfGreaterOrEqualThan(float *stackVals, unsigned int &top)
{
    --top;
    float rhs = stackVals[top];
    float lhs = stackVals[top-1];
    LOG("Testing if ", lhs, " is greater or equal to ", rhs);
    stackVals[top-1] = (roundf(lhs) >= roundf(rhs)) ? 1 : 0;
}

void Evaluator::testIfLessThanOrEqual(float *stackVals, unsigned int &top)
{
    --top;
    float rhs = stackVals[top];
    float lhs = stackVals[top-1];
    LOG("Testing if ", lhs, " is less or equal to ", rhs);
    stackVals[top-1] = (roundf(lhs) <= roundf(rhs)) ? 1 : 0;
}

void Evaluator::testAnd(float *stackVals, unsigned int &top)
{
    --top;
    float rhs = stackVals[top];
    float lhs = stackVals[top-1];
    LOG("Testing if ", lhs, " is less or equal to ", rhs);
    stackVals[top-1] = (roundf(lhs) && roundf(rhs)) ? 1 : 0;
}

void Evaluator::testOr(float *stackVals, unsigned int &top)
{
    --top;
    float rhs = stackVals[top];
    float lhs = stackVals[top-1];
    LOG("Testing if ", lhs, " is less or equal to ", rhs);
    stackVals[top-1] = (roundf(lhs) || roundf(rhs)) ? 1 : 0;
}

void Evaluator::maxFn(float *stackVals, unsigned int &top)
{
    --top;
    float rhs = stackVals[top];
    float lhs = stackVals[top-1];
    LOG("Getting max of ", lhs, " or ", rhs);
    stackVals[top-1] = fmaxf(lhs,rhs);
}

} //namespace EVAL